// ============================================================
//  数据录入回归测试（offscreen 运行）
//  用法: autofill_test.exe <workdir> <real_src> [caseFilter]
//    <workdir>   : 测试工作根（创建的用例目录都在这下面）
//    <real_src>  : 真实 TPF2 工程数据副本（只读复制出来的，绝不碰原目录）
//  说明：对话框由 watchdog 自动处理（记录 + 点“确定/确认”），
//        结果写入 <workdir>/report.txt（UTF-8）。
// ============================================================
#include <QtWidgets>
#include <QRegularExpression>
#include <QDirIterator>
#include <cstdio>
#include <functional>
#include <array>
#include "util.h"
#include "xlsxdocument.h"
#define private public
#include "mainui.h"
#include "data_add.h"
#undef private

// util.cpp 内部函数（非 static）——复用于输出解析
bool find_next_entry(const std::string& text, size_t start, size_t end,
                     int& lid, size_t& entryStart, size_t& bodyStart, size_t& bodyEnd,
                     bool& quotedKey);
size_t find_entry_end(const std::string& text, size_t bodyStart);

namespace fs = std::filesystem;

static QString g_work;
static QString g_realSrc;
static QString g_filter;
static QFile   g_report;
static QStringList g_dialogs;
static int g_pass = 0;
static int g_fail = 0;

// ---------------- 基础辅助 ----------------
static void rep(const QString& s)
{
    if(g_report.isOpen()) { g_report.write((s + "\n").toUtf8()); g_report.flush(); }
}
static void perr(const QString& s)
{
    fprintf(stderr, "%s\n", s.toUtf8().constData());
    fflush(stderr);
    rep(s);
}

static QByteArray rd(const QString& p)
{
    QFile f(p);
    if(!f.open(QIODevice::ReadOnly)) return {};
    return f.readAll();
}
static QString rdQ(const QString& p) { return QString::fromUtf8(rd(p)); }
static bool wr(const QString& p, const QByteArray& d)
{
    QFile f(p);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    return f.write(d) == d.size();
}
static bool cpDir(const QString& src, const QString& dst)
{
    QDir().mkpath(dst);
    QDirIterator it(src, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        const QString s = it.next();
        const QString rel = QDir(src).relativeFilePath(s);
        const QString t = dst + "/" + rel;
        QDir().mkpath(QFileInfo(t).absolutePath());
        if(QFile::exists(t)) QFile::remove(t);
        if(!QFile::copy(s, t)) return false;
    }
    return true;
}
static QString newCaseDir(const QString& name)
{
    QString d = g_work + "/cases/" + name;
    QDir(d).removeRecursively();
    QDir().mkpath(d);
    return d;
}
static int countMatches(const QString& text, const QString& pattern)
{
    QRegularExpression re(pattern);
    int k = 0;
    auto it = re.globalMatch(text);
    while(it.hasNext()) { it.next(); ++k; }
    return k;
}
static bool strHas(const std::string& s, const std::string& sub) { return s.find(sub) != std::string::npos; }
static std::string qs2s(const QString& q) { return q.toStdString(); }

// ---------------- 对话框自动处理 ----------------
class Watchdog : public QObject
{
public:
    Watchdog() { t.setInterval(60); connect(&t, &QTimer::timeout, this, &Watchdog::tick); t.start(); }
    QTimer t;
    void tick()
    {
        QWidget *w = QApplication::activeModalWidget();
        if(!w) return;
        if(qobject_cast<QProgressDialog*>(w)) return;     // 进度框不处理
        if(qobject_cast<QFileDialog*>(w))                 // 意外文件对话框：记录并关闭
        {
            g_dialogs << QString("[QFileDialog] %1 (auto-closed)").arg(w->windowTitle());
            w->close();
            return;
        }
        QString title = w->windowTitle();
        QString body;
        if(auto *mb = qobject_cast<QMessageBox*>(w))
        {
            body = mb->text();
            if(auto *cb = mb->checkBox())
                body += QString("\n[checkbox \"%1\" %2]").arg(cb->text(), cb->isChecked() ? "checked" : "unchecked");
        }
        for(QLabel *l : w->findChildren<QLabel*>())
            if(!l->text().isEmpty() && !body.contains(l->text()))
                body += (body.isEmpty() ? "" : "\n") + l->text();
        for(QTextEdit *x : w->findChildren<QTextEdit*>())
            if(!x->toPlainText().isEmpty())
                body += "\n[TEXT]\n" + x->toPlainText().left(6000);
        g_dialogs << QString("[%1]\n%2").arg(title, body.left(8000));
        // 优先点击 确认/确定，其次第一个非取消按钮
        QList<QAbstractButton*> btns;
        if(auto *mb = qobject_cast<QMessageBox*>(w))
            btns = mb->buttons();
        else
            btns = w->findChildren<QAbstractButton*>();
        QAbstractButton *best = nullptr;
        int bestRank = 100;
        for(QAbstractButton *b : btns)
        {
            const QString tx = b->text();
            int rank = 5;
            if(tx.contains(QString::fromUtf8("确定")) || tx.contains(QString::fromUtf8("确认"))) rank = 0;
            else if(tx.contains(QString::fromUtf8("取消"))) rank = 9;
            if(rank < bestRank) { bestRank = rank; best = b; }
        }
        if(best) { best->click(); return; }
        w->close();
    }
};

// ---------------- 用例框架 ----------------
struct CR
{
    QString name;
    QStringList notes;
    bool pass = true;
    void ck(bool ok, const QString& what) { if(!ok) { pass = false; notes << ("FAIL: " + what); } }
    void nt(const QString& s) { notes << s; }
};

typedef std::function<void(CR&)> CaseFn;

static void baseSetup(mainui &w, const QString& folder, const QString& sgName,
                      const QString& sgPath, int dver)
{
    w.sdata = my_data{};
    w.sdata.folder_dir = fs::u8path(qs2s(folder));
    w.sdata.folder_name = qs2s(QFileInfo(folder).fileName());
    w.sdata.sg_name = qs2s(sgName);
    w.sdata.sg_dir = fs::u8path(qs2s(sgPath));
    w.sdata.sys_save_dir = fs::u8path(qs2s(QFileInfo(sgPath).absolutePath()));
    w.sdata.d_version = dver;
    w.sdata.easy_if = true;
    w.sdata.xls_if = false;
    w.sdata.invalid_if = false;
    w.sdata.clear_if = 1;
    w.sdata.pile_if = false;
    w.sdata.cycle_index = 1;   // 2 小时
    // read_station_line()（非槽函数）内联：读 <sg_name>_station.xlsx / _line.xlsx
    readXlsx(w.sdata.folder_dir / fs::u8path(w.sdata.sg_name + "_station.xlsx"), w.sdata.station);
    readXlsx(w.sdata.folder_dir / fs::u8path(w.sdata.sg_name + "_line.xlsx"), w.sdata.line);

    // 让构造函数里排的 singleShot(init) 在受控时机执行：先写一份当前配置的 .dat
    // 再触发（模拟真实启动流程；否则它会在后面第一个对话框的嵌套事件循环里意外触发）
    refresh_file(w.sdata);
    QCoreApplication::processEvents();
}

// ---------------- 夹具生成 ----------------
struct TimeRow { QString st; QTime arr; QTime dep; };

static void writeNameIdXlsx(const QString& path, const std::vector<std::pair<QString, int>>& rows)
{
    QXlsx::Document x;
    int r = 1;
    for(const auto& p : rows) { x.write(r, 1, p.first); x.write(r, 2, p.second); ++r; }
    x.saveAs(path);
}
static void writeTimeXlsx(const QString& path, const QString& sheetName, const std::vector<TimeRow>& rows, bool firstCol = true)
{
    QXlsx::Document x;
    if(!sheetName.isEmpty() && !x.sheetNames().contains(sheetName))
    {
        x.addSheet(sheetName);
        x.selectSheet(sheetName);
        if(x.sheetNames().contains(QStringLiteral("Sheet1")) && sheetName != QStringLiteral("Sheet1"))
            x.deleteSheet(QStringLiteral("Sheet1"));
    }
    else if(!sheetName.isEmpty())
        x.selectSheet(sheetName);
    int r = 1;
    for(const auto& t : rows)
    {
        if(firstCol)
            x.write(r, 1, r);   // 第 1 列（可选；末行校验已改为看第 2、3 列）
        x.write(r, 2, t.st);
        x.write(r, 3, QDateTime(QDate(1980, 1, 1), t.arr));
        x.write(r, 4, QDateTime(QDate(1980, 1, 1), t.dep));
        ++r;
    }
    x.saveAs(path);
}
static void writeCsv(const QString& path, const std::vector<QStringList>& rows)
{
    QString t;
    for(const auto& r : rows) t += r.join(",") + "\n";
    wr(path, t.toUtf8());
}
static void writeListXlsx(const QString& path, const std::vector<std::array<QString, 3>>& rows)
{
    QXlsx::Document x;
    x.write(1, 1, QStringLiteral("线路"));
    x.write(1, 2, QStringLiteral("文件1"));
    x.write(1, 3, QStringLiteral("表单名称"));
    int r = 1;
    for(const auto& row : rows)
    {
        ++r;
        x.write(r, 1, row[0]);
        x.write(r, 2, row[1]);
        if(!row[2].isEmpty()) x.write(r, 3, row[2]);
    }
    x.saveAs(path);
}
static void mkTpf3(const QString& dir, int cycleSec = 7200)
{
    QString ex;
    ex += "function data()\nreturn { \n";
    ex += QString("\t\tcycleSec = %1,\n").arg(cycleSec);
    ex += "\t\tlineStops = { \n\t\t\t[\"1\"] = \"10,20,30\",\n\t\t\t[\"2\"] = \"10,20\",\n\t\t},\n";
    ex += "\t\tlines = { \n\t\t\t[\"1\"] = \"G1/2 上海-无锡\",\n\t\t\t[\"2\"] = \"1461/2 上海-苏州\",\n\t\t},\n";
    ex += "\t\tsaveId = \"timetable test\",\n";
    ex += "\t\tstations = { \n\t\t\t[\"10\"] = \"上海\",\n\t\t\t[\"20\"] = \"苏州\",\n\t\t\t[\"30\"] = \"无锡\",\n\t\t},\n";
    ex += "\t}\nend\n";
    wr(dir + "/export.lua", ex.toUtf8());
    writeNameIdXlsx(dir + "/tpf3_timetable_station.xlsx", {{QStringLiteral("上海"), 10}, {QStringLiteral("苏州"), 20}, {QStringLiteral("无锡"), 30}});
    writeNameIdXlsx(dir + "/tpf3_timetable_line.xlsx", {{QStringLiteral("G1"), 1}, {QStringLiteral("1461"), 2}});
    writeListXlsx(dir + "/tpf3_timetable_list.xlsx", {});
}
static void t1Csvs(const QString& dir)
{
    writeCsv(dir + "/G1.csv", {
        {"G1/2", "上海", "1:05:00", "1:06:00"},
        {"G1/2", "苏州", "1:15:00", "1:16:00"},
        {"G1/2", "无锡", "1:25:00", "1:26:00"},
    });
    writeCsv(dir + "/1461.csv", {
        {"1461/2", "上海", "1:00:00", "1:01:00"},
        {"1461/2", "苏州", "1:10:00", "1:11:00"},
    });
}
// 最小 TPF2 存档（quoted key）
static const char* kMinSave =
"function data()\n"
"return {\n"
"\t[\"other_mod.lua\"] = {\n"
"\t\tx = 1,\n"
"\t},\n"
"\t[\"timetable_gui.lua\"] = {\n"
"\t\ttimetable = {\n"
"\t\t\t[\"1\"] = {\n"
"\t\t\t\tcustom = \"keep1\",\n"
"\t\t\t\tfrequency = 7,\n"
"\t\t\t\thasTimetable = true,\n"
"\t\t\t\tstations = {\n"
"\t\t\t\t\t{ stationID = 99, },\n"
"\t\t\t\t},\n"
"\t\t\t},\n"
"\t\t\t[\"2\"] = {\n"
"\t\t\t\tfrequency = 2,\n"
"\t\t\t\thasTimetable = false,\n"
"\t\t\t\tother = \"keep2\",\n"
"\t\t\t},\n"
"\t\t\t[\"3\"] = {\n"
"\t\t\t\tfrequency = 3,\n"
"\t\t\t\thasTimetable = true,\n"
"\t\t\t\tstations = { },\n"
"\t\t\t},\n"
"\t\t},\n"
"\t},\n"
"}\n"
"end\n";
static void mkTpf2Synth(const QString& dir)
{
    writeNameIdXlsx(dir + "/test.sav_station.xlsx", {{QStringLiteral("上海"), 10}, {QStringLiteral("苏州"), 20}, {QStringLiteral("无锡"), 30}, {QStringLiteral("常州"), 40}});
    writeNameIdXlsx(dir + "/test.sav_line.xlsx", {{QStringLiteral("G1"), 1}, {QStringLiteral("D2"), 2}});
    wr(dir + "/test.sav.lua", QByteArray(kMinSave));
    writeListXlsx(dir + "/test.sav_list.xlsx", {});
}

// ---------------- 输出解析 ----------------
static bool sectionRange(const QByteArray& raw, size_t& s, size_t& e)
{
    std::string t(raw.constData(), raw.size());
    size_t pos = t.find("timetable = {");
    if(pos == std::string::npos) return false;
    size_t b = t.find('{', pos);
    int bc = 1;
    size_t i = b + 1;
    for(; i < t.size() && bc > 0; ++i) { if(t[i] == '{') ++bc; else if(t[i] == '}') --bc; }
    if(bc != 0) return false;
    s = pos;
    e = i;
    return true;
}
static std::string entryOf(const QByteArray& raw, int id, bool& found)
{
    found = false;
    std::string t(raw.constData(), raw.size());
    size_t s, e;
    if(!sectionRange(raw, s, e)) return {};
    size_t b = t.find('{', s);
    size_t scan = b + 1;
    int lid;
    size_t es, bs, be;
    bool q;
    while(find_next_entry(t, scan, e, lid, es, bs, be, q))
    {
        if(lid == id) { found = true; return t.substr(es, be - es + 1); }
        scan = be + 1;
    }
    return {};
}
static int entryCount(const QByteArray& raw)
{
    std::string t(raw.constData(), raw.size());
    size_t s, e;
    if(!sectionRange(raw, s, e)) return -1;
    size_t b = t.find('{', s);
    size_t scan = b + 1;
    int lid;
    size_t es, bs, be;
    bool q;
    int n = 0;
    while(find_next_entry(t, scan, e, lid, es, bs, be, q)) { ++n; scan = be + 1; }
    return n;
}
// 统计被清空成空表的条目（无 hasTimetable / stations 字段）
static int countEmptiedEntries(const QByteArray& raw)
{
    std::string t(raw.constData(), raw.size());
    size_t s, e;
    if(!sectionRange(raw, s, e)) return -1;
    size_t b = t.find('{', s);
    size_t scan = b + 1;
    int lid; size_t es, bs, be; bool q;
    int n = 0;
    while(find_next_entry(t, scan, e, lid, es, bs, be, q))
    {
        std::string body = t.substr(es, be - es + 1);
        if(body.find("hasTimetable") == std::string::npos &&
           body.find("stations") == std::string::npos)
            ++n;
        scan = be + 1;
    }
    return n;
}
// 从存档文件中删掉某条线路的整块条目（用于制造“该线路在存档中不存在”的场景）
static bool eraseEntry(const QString& path, int id)
{
    QByteArray raw = rd(path);
    std::string t(raw.constData(), raw.size());
    size_t s, e;
    if(!sectionRange(raw, s, e)) return false;
    size_t b = t.find('{', s);
    size_t scan = b + 1;
    int lid; size_t es, bs, be; bool q;
    while(find_next_entry(t, scan, e, lid, es, bs, be, q))
    {
        if(lid == id)
        {
            size_t end = be + 1;                        // '}' 之后
            if(end < t.size() && t[end] == ',') ++end;  // 逗号
            if(end < t.size() && t[end] == '\n') ++end;
            size_t start = es;
            while(start > 0 && t[start - 1] != '\n') --start;   // 行首
            t.erase(start, end - start);
            return wr(path, QByteArray::fromStdString(t));
        }
        scan = be + 1;
    }
    return false;
}
static int csvRowCount(const QString& p)
{
    int n = 0;
    for(const QString& line : rdQ(p).split('\n'))
        if(!line.trimmed().isEmpty()) ++n;
    return n;
}

// ============================================================
static void runCase(const QString& name, const CaseFn& fn)
{
    if(!g_filter.isEmpty() && !name.contains(g_filter)) return;
    CR r;
    r.name = name;
    g_dialogs.clear();
    perr("---- CASE " + name);
    try { fn(r); }
    catch(std::exception& e) { r.ck(false, QString("exception: %1").arg(e.what())); }
    catch(...) { r.ck(false, "unknown exception"); }
    if(!g_dialogs.isEmpty())
    {
        r.nt(QString("dialogs(%1):").arg(g_dialogs.size()));
        for(int i = 0; i < g_dialogs.size() && i < 8; ++i)
            r.nt("  [dlg] " + QString(g_dialogs[i]).left(400).replace("\n", " | "));
    }
    if(r.pass) ++g_pass; else ++g_fail;
    rep(QString("==== %1 : %2").arg(name, r.pass ? "PASS" : "FAIL"));
    for(const auto& n : r.notes) rep("    " + n);
    perr(QString("==== %1 %2").arg(name, r.pass ? "PASS" : "FAIL"));
}
static bool hasDialog(const QString& sub)
{
    for(const auto& d : g_dialogs) if(d.contains(sub)) return true;
    return false;
}

// ============================================================
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    if(argc < 3)
    {
        fprintf(stderr, "usage: autofill_test <workdir> <real_src> [caseFilter]\n");
        return 2;
    }
    g_work = QString::fromUtf8(argv[1]);
    g_realSrc = QString::fromUtf8(argv[2]);
    if(argc > 3) g_filter = QString::fromUtf8(argv[3]);
    QDir().mkpath(g_work);
    g_report.setFileName(g_work + "/report.txt");
    g_report.open(QIODevice::WriteOnly | QIODevice::Truncate);
    Watchdog wd;

    const QString FD = QStringLiteral("F:/code/qt/tpf2_autofill/");

    // ---------------- TPF3 ----------------
    runCase("T0_read_export", [&](CR& r) {
        QString dir = newCaseDir("T0_read_export");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        export_info info;
        bool ok = read_export_file(fs::u8path(qs2s(dir + "/export.lua")), info);
        r.ck(ok, "read_export_file ok");
        r.ck(info.stations.size() == 3, QString("stations=%1").arg(info.stations.size()));
        r.ck(info.lines.size() == 2, QString("lines=%1").arg(info.lines.size()));
        r.ck(info.cycle_sec == 7200, QString("cycle=%1").arg(info.cycle_sec));
        r.ck(info.save_id == "timetable test", QString("saveId=%1").arg(QString::fromStdString(info.save_id)));
        auto it = info.line_stops.find(1);
        r.ck(it != info.line_stops.end() && it->second.size() == 3, "lineStops[1] == 3");
        r.nt(QString("stops1=%1").arg(it != info.line_stops.end() ? (int)it->second.size() : -1));
    });

    runCase("T1_tpf3_sync_cover", [&](CR& r) {
        QString dir = newCaseDir("T1_tpf3_sync_cover");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        t1Csvs(dir);
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(!t.isEmpty(), "data.lua exists");
        r.ck(t.contains("cycleSec = 7200"), "cycleSec=7200");
        r.ck(t.contains("force = false"), "force=false");
        r.ck(t.contains(QString::fromUtf8("name = \"G1/2 上海-无锡\"")), "G1 full name");
        r.ck(t.contains(QString::fromUtf8("name = \"1461/2 上海-苏州\"")), "1461 full name");
        r.ck(t.contains("{ arrMin = 65, arrSec = 0, depMin = 66, depSec = 0 }"), "slot 1:05->65");
        r.ck(t.contains("{ arrMin = 85, arrSec = 0, depMin = 86, depSec = 0 }"), "slot 1:25->85");
        r.ck(!t.contains("clearLines"), "no clearLines");
        r.ck(!t.contains("clearAll"), "no clearAll");
        r.nt(QString("arrMin count=%1").arg(countMatches(t, "arrMin")));
    });

    runCase("T2_tpf3_over_cycle", [&](CR& r) {
        QString dir = newCaseDir("T2_tpf3_over_cycle");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        t1Csvs(dir);
        QFile::remove(dir + "/data.lua");
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.cycle_index = 0;   // 1h，1:05 越界
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表超出所选周期范围")), "over-cycle dialog");
        r.ck(!QFile::exists(dir + "/data.lua"), "data.lua not written");
    });

    runCase("T3_tpf3_stop_missing", [&](CR& r) {
        QString dir = newCaseDir("T3_tpf3_stop_missing");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeNameIdXlsx(dir + "/tpf3_timetable_station.xlsx", {{QStringLiteral("上海"), 10}, {QStringLiteral("苏州"), 20}, {QStringLiteral("无锡"), 30}, {QStringLiteral("常州"), 40}});
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "1:05:00", "1:06:00"},
            {"G1/2", "苏州", "1:15:00", "1:16:00"},
            {"G1/2", "无锡", "1:25:00", "1:26:00"},
            {"G1/2", "常州", "1:35:00", "1:36:00"},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("与游戏存档数据不一致")), "EXPORT_MISMATCH dialog");
        r.ck(!QFile::exists(dir + "/data.lua"), "data.lua not written");
    });

    runCase("T4_tpf3_stop_order", [&](CR& r) {
        QString dir = newCaseDir("T4_tpf3_stop_order");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "1:05:00", "1:06:00"},
            {"G1/2", "无锡", "1:25:00", "1:26:00"},
            {"G1/2", "苏州", "1:15:00", "1:16:00"},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("与游戏存档数据不一致")), "EXPORT_MISMATCH dialog");
    });

    runCase("T5_tpf3_line_missing", [&](CR& r) {
        QString dir = newCaseDir("T5_tpf3_line_missing");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeNameIdXlsx(dir + "/tpf3_timetable_line.xlsx", {{QStringLiteral("X"), 9}});
        writeNameIdXlsx(dir + "/tpf3_timetable_station.xlsx", {{QStringLiteral("上海"), 10}, {QStringLiteral("苏州"), 20}});
        writeCsv(dir + "/X.csv", {
            {"X", "上海", "1:05:00", "1:06:00"},
            {"X", "苏州", "1:15:00", "1:16:00"},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("在存档中不存在")), "line-missing dialog");
    });

    runCase("T6_tpf3_clear2", [&](CR& r) {
        QString dir = newCaseDir("T6_tpf3_clear2");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        t1Csvs(dir);
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(t.contains("clearLines = {"), "clearLines present");
        r.ck(t.contains(QString::fromUtf8("\"G1/2 上海-无锡\"")), "clearLines G1 full name");
        r.ck(t.contains(QString::fromUtf8("\"1461/2 上海-苏州\"")), "clearLines 1461 full name");
    });

    runCase("T7_tpf3_clear3", [&](CR& r) {
        QString dir = newCaseDir("T7_tpf3_clear3");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        t1Csvs(dir);
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.clear_if = 3;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(t.contains("clearAll = true"), "clearAll=true");
    });

    runCase("T8_tpf3_xlsx_input", [&](CR& r) {
        QString dir = newCaseDir("T8_tpf3_xlsx_input");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeTimeXlsx(dir + "/G1.xlsx", "Sheet1", {
            {QStringLiteral("上海"), QTime(1, 5, 0), QTime(1, 6, 0)},
            {QStringLiteral("苏州"), QTime(1, 15, 0), QTime(1, 16, 0)},
            {QStringLiteral("无锡"), QTime(1, 25, 0), QTime(1, 26, 0)},
        });
        writeTimeXlsx(dir + "/1461.xlsx", "Sheet1", {
            {QStringLiteral("上海"), QTime(1, 0, 0), QTime(1, 1, 0)},
            {QStringLiteral("苏州"), QTime(1, 10, 0), QTime(1, 11, 0)},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.xls_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(t.contains("{ arrMin = 65, arrSec = 0, depMin = 66, depSec = 0 }"), "xlsx 1:05 -> arrMin 65 (TPF3 hour)");
    });

    runCase("T9_tpf3_list_xlsx", [&](CR& r) {
        QString dir = newCaseDir("T9_tpf3_list_xlsx");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeTimeXlsx(dir + "/book.xlsx", "S1", {
            {QStringLiteral("上海"), QTime(1, 5, 0), QTime(1, 6, 0)},
            {QStringLiteral("苏州"), QTime(1, 15, 0), QTime(1, 16, 0)},
            {QStringLiteral("无锡"), QTime(1, 25, 0), QTime(1, 26, 0)},
        });
        writeTimeXlsx(dir + "/book2.xlsx", "S2", {
            {QStringLiteral("上海"), QTime(1, 0, 0), QTime(1, 1, 0)},
            {QStringLiteral("苏州"), QTime(1, 10, 0), QTime(1, 11, 0)},
        });
        writeListXlsx(dir + "/tpf3_timetable_list.xlsx", {
            {QStringLiteral("G1"), QStringLiteral("book.xlsx"), QStringLiteral("S1")},
            {QStringLiteral("1461"), QStringLiteral("book2.xlsx"), QStringLiteral("S2")},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(t.contains("{ arrMin = 65, arrSec = 0, depMin = 66, depSec = 0 }"), "list sheet G1 1:05->65");
        r.ck(t.contains("{ arrMin = 60, arrSec = 0, depMin = 61, depSec = 0 }"), "list sheet 1461 1:00->60");
    });

    runCase("T10_tpf3_pile", [&](CR& r) {
        QString dir = newCaseDir("T10_tpf3_pile");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeNameIdXlsx(dir + "/tpf3_timetable_line.xlsx", {{QStringLiteral("1461"), 2}});
        writeCsv(dir + "/1461.csv", {
            {"1461/2", "上海", "1:00:00", "1:01:00"},
            {"1461/2", "苏州", "1:10:00", "1:11:00"},
            {"1461/2", "上海", "1:30:00", "1:31:00"},
            {"1461/2", "苏州", "1:40:00", "1:41:00"},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.pile_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(t.contains("{ index = 1, slots = {") && t.contains("{ index = 2, slots = {"), "2 stops");
        r.ck(countMatches(t, "arrMin") == 4, QString("4 slots (2x2), got %1").arg(countMatches(t, "arrMin")));
    });

    runCase("T11_tpf3_cycle_boundary_ok", [&](CR& r) {
        QString dir = newCaseDir("T11_tpf3_cycle_boundary_ok");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeNameIdXlsx(dir + "/tpf3_timetable_line.xlsx", {{QStringLiteral("1461"), 2}});
        writeCsv(dir + "/1461.csv", {
            {"1461/2", "上海", "1:59:30", "1:59:40"},
            {"1461/2", "苏州", "1:59:45", "1:59:50"},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表数据已生成")), "success dialog");
        QString t = rdQ(dir + "/data.lua");
        r.ck(t.contains("{ arrMin = 119, arrSec = 30, depMin = 119, depSec = 40 }"), "boundary 1:59:30 ok");
    });

    runCase("T12_tpf3_cycle_edge_exact", [&](CR& r) {
        QString dir = newCaseDir("T12_tpf3_cycle_edge_exact");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        writeNameIdXlsx(dir + "/tpf3_timetable_line.xlsx", {{QStringLiteral("1461"), 2}});
        writeCsv(dir + "/1461.csv", {
            {"1461/2", "上海", "1:59:30", "2:00:00"},   // dep 正好 = 周期(7200s) -> 应报错
            {"1461/2", "苏州", "2:00:10", "2:00:20"},
        });
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表超出所选周期范围")), "exact-cycle dep should error");
    });

    // ---------------- TPF2（合成夹具）----------------
    runCase("T13_tpf2_minimal_cover", [&](CR& r) {
        QString dir = newCaseDir("T13_tpf2_minimal_cover");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
            {"G1/2", "无锡", "0:25:00", "0:26:00"},
        });
        QByteArray before = rd(dir + "/test.sav.lua");
        bool fb;
        std::string e1b = entryOf(before, 1, fb);
        std::string e2b = entryOf(before, 2, fb);
        std::string e3b = entryOf(before, 3, fb);
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool fa;
        std::string e1a = entryOf(after, 1, fa);
        std::string e2a = entryOf(after, 2, fa);
        std::string e3a = entryOf(after, 3, fa);
        r.ck(e2a == e2b, "entry 2 untouched");
        r.ck(e3a == e3b, "entry 3 untouched");
        r.ck(strHas(e1a, "hasTimetable = true"), "entry1 hasTimetable");
        r.ck(strHas(e1a, "custom = \"keep1\""), "entry1 custom kept");
        r.ck(countMatches(QString::fromStdString(e1a), "stationID") == 3, "entry1 3 stations");
        size_t s1, e1, s2, e2;
        r.ck(sectionRange(before, s1, e1) && sectionRange(after, s2, e2), "sections found");
        r.ck(before.left((int)s1) == after.left((int)s2), "prefix unchanged");
        r.ck(before.mid((int)e1) == after.mid((int)e2), "suffix unchanged");
        {
            QStringList bks = QDir(dir).entryList(QStringList() << "*.backup", QDir::Files);
            r.ck(!bks.isEmpty(), "backup created");
        }
    });

    runCase("T14_tpf2_minimal_clear2", [&](CR& r) {
        QString dir = newCaseDir("T14_tpf2_minimal_clear2");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
            {"G1/2", "无锡", "0:25:00", "0:26:00"},
        });
        QByteArray before = rd(dir + "/test.sav.lua");
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f1, f2, f3;
        std::string e1 = entryOf(after, 1, f1);
        std::string e2 = entryOf(after, 2, f2);
        std::string e3 = entryOf(after, 3, f3);
        r.ck(f1 && f2 && f3, "entries exist");
        r.ck(strHas(e1, "hasTimetable = true") && strHas(e1, "custom = \"keep1\""), "entry1 re-imported + custom kept");
        r.ck(!strHas(e2, "frequency") && strHas(e2, "hasTimetable = false") && strHas(e2, "stations = { }"), "entry2 cleared with empty-timetable marker");
        r.ck(strHas(e2, "other = \"keep2\""), "entry2 other kept");
        std::string e3b = entryOf(before, 3, f3);
        r.ck(e3 == e3b, "entry3 untouched");
        r.nt(QString("entry2 after: %1").arg(QString::fromStdString(e2).left(200).replace("\n", " ")));
    });

    runCase("T15_tpf2_minimal_clear3", [&](CR& r) {
        QString dir = newCaseDir("T15_tpf2_minimal_clear3");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
            {"G1/2", "无锡", "0:25:00", "0:26:00"},
        });
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        w.sdata.clear_if = 3;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f1, f2, f3;
        std::string e1 = entryOf(after, 1, f1);
        std::string e2 = entryOf(after, 2, f2);
        std::string e3 = entryOf(after, 3, f3);
        r.ck(f1 && f2 && f3, "entries exist");
        r.ck(strHas(e1, "hasTimetable = true"), "entry1 re-imported");
        r.ck(!strHas(e2, "frequency") && strHas(e2, "hasTimetable = false") && strHas(e2, "stations = { }"), "entry2 cleared with marker");
        r.ck(!strHas(e3, "frequency") && strHas(e3, "hasTimetable = false") && strHas(e3, "stations = { }"), "entry3 cleared with marker");
        r.ck(strHas(e2, "other = \"keep2\""), "entry2 other kept");
    });

    runCase("T16_tpf2_numeric_migrate", [&](CR& r) {
        QString dir = newCaseDir("T16_tpf2_numeric_migrate");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
            {"G1/2", "无锡", "0:25:00", "0:26:00"},
        });
        QByteArray before = rd(dir + "/test.sav.lua");
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 1);   // 数字键模式
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        size_t s, e;
        r.ck(sectionRange(after, s, e), "section found");
        QByteArray sect = after.mid((int)s, (int)(e - s));
        r.ck(!sect.contains("[\""), "all keys migrated to numeric");
        r.ck(entryCount(after) == entryCount(before), "entry count unchanged");
        size_t s1, e1, s2, e2;
        sectionRange(before, s1, e1);
        sectionRange(after, s2, e2);
        r.ck(before.left((int)s1) == after.left((int)s2), "prefix unchanged");
        r.ck(before.mid((int)e1) == after.mid((int)e2), "suffix unchanged");
    });

    runCase("T17_tpf2_xlsx_input", [&](CR& r) {
        QString dir = newCaseDir("T17_tpf2_xlsx_input");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeTimeXlsx(dir + "/G1.xlsx", "Sheet1", {
            {QStringLiteral("上海"), QTime(0, 5, 0), QTime(0, 6, 0)},
            {QStringLiteral("苏州"), QTime(0, 15, 0), QTime(0, 16, 0)},
            {QStringLiteral("无锡"), QTime(0, 25, 0), QTime(0, 26, 0)},
        });
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        w.sdata.xls_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f;
        std::string e1 = entryOf(after, 1, f);
        r.ck(f && countMatches(QString::fromStdString(e1), "stationID") == 3, "3 stations");
        r.ck(strHas(e1, "{5, 0, 6, 0, },"), "slot 0:05 -> 5 (TPF2 minute-only)");
        r.nt(QString("entry1 head: %1").arg(QString::fromStdString(e1).left(300).replace("\n", " ")));
    });

    runCase("T18_tpf2_xlsx_invalid_compare", [&](CR& r) {
        QString dir = newCaseDir("T18_tpf2_xlsx_invalid_compare");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeTimeXlsx(dir + "/G1.xlsx", "Sheet1", {
            {QStringLiteral("上海"), QTime(0, 5, 0), QTime(0, 6, 0)},
            {QStringLiteral("苏州"), QTime(0, 15, 0), QTime(0, 16, 0)},
            {QStringLiteral("无锡"), QTime(0, 25, 0), QTime(0, 26, 0)},
            {QStringLiteral("常州"), QTime(0, 35, 0), QTime(0, 36, 0)},
        });
        int counts[2] = {-1, -1};
        for(int k = 0; k < 2; ++k)
        {
            // 每个子运行使用独立目录（避免 save 被前一次修改）
            QString d2 = dir + (k == 0 ? "/a" : "/b");
            QDir().mkpath(d2);
            cpDir(dir, d2);
            QDir::setCurrent(d2);
            mainui w;
            baseSetup(w, d2, "test.sav", d2 + "/test.sav.lua", 0);
            w.sdata.xls_if = true;
            w.sdata.invalid_if = (k == 1);
            QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
            QByteArray after = rd(d2 + "/test.sav.lua");
            bool f;
            std::string e1 = entryOf(after, 1, f);
            counts[k] = f ? countMatches(QString::fromStdString(e1), "stationID") : -1;
        }
        r.nt(QString("xlsx stations: invalid_off=%1 invalid_on=%2").arg(counts[0]).arg(counts[1]));
        r.ck(counts[0] == 4, "invalid off: 4 stations");
        r.ck(counts[1] == 3, "invalid on: last row dropped (3)");
    });

    runCase("T19_tpf2_csv_invalid_compare", [&](CR& r) {
        QString dir = newCaseDir("T19_tpf2_csv_invalid_compare");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
            {"G1/2", "无锡", "0:25:00", "0:26:00"},
            {"G1/2", "常州", "0:35:00", "0:36:00"},
        });
        int counts[2] = {-1, -1};
        for(int k = 0; k < 2; ++k)
        {
            QString d2 = dir + (k == 0 ? "/a" : "/b");
            QDir().mkpath(d2);
            cpDir(dir, d2);
            QDir::setCurrent(d2);
            mainui w;
            baseSetup(w, d2, "test.sav", d2 + "/test.sav.lua", 0);
            w.sdata.invalid_if = (k == 1);
            QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
            QByteArray after = rd(d2 + "/test.sav.lua");
            bool f;
            std::string e1 = entryOf(after, 1, f);
            counts[k] = f ? countMatches(QString::fromStdString(e1), "stationID") : -1;
        }
        r.nt(QString("csv stations: invalid_off=%1 invalid_on=%2").arg(counts[0]).arg(counts[1]));
        r.ck(counts[0] == 4, "invalid off: 4 stations");
        r.ck(counts[1] == 3, "invalid on: pop_back (3)");
    });

    runCase("T23_tpf2_xlsx_no_col1", [&](CR& r) {
        QString dir = newCaseDir("T23_tpf2_xlsx_no_col1");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeTimeXlsx(dir + "/G1.xlsx", "Sheet1", {
            {QStringLiteral("上海"), QTime(0, 5, 0), QTime(0, 6, 0)},
            {QStringLiteral("苏州"), QTime(0, 15, 0), QTime(0, 16, 0)},
            {QStringLiteral("无锡"), QTime(0, 25, 0), QTime(0, 26, 0)},
            {QStringLiteral("常州"), QTime(0, 35, 0), QTime(0, 36, 0)},
        }, false);   // 不写第 1 列
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        w.sdata.xls_if = true;
        w.sdata.invalid_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f;
        std::string e1 = entryOf(after, 1, f);
        int sts = countMatches(QString::fromStdString(e1), "stationID");
        r.nt(QString("no-col1 xlsx invalid_on: stations=%1 (expect 3)").arg(sts));
        r.ck(sts == 3, "3 stations (last dropped; col1 not required)");
    });

    runCase("T20_tpf2_dup_rounds", [&](CR& r) {
        QString dir = newCaseDir("T20_tpf2_dup_rounds");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
        });
        writeCsv(dir + "/G1_2.csv", {
            {"G1/2", "上海", "0:05:03", "0:06:02"},
            {"G1/2", "苏州", "0:15:04", "0:16:03"},
        });
        writeCsv(dir + "/G1_3.csv", {   // 与第一组完全相同的重复组
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
        });
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("相近或重复数据")), "shrink warning in confirm dialog");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f;
        std::string e1 = entryOf(after, 1, f);
        int slotCnt = countMatches(QString::fromStdString(e1), "\\{ ?\\d+, \\d+, \\d+, \\d+, ?\\},");
        int sts = countMatches(QString::fromStdString(e1), "stationID");
        r.nt(QString("dup case: stations=%1 slots=%2 (3 rounds in, expect 1 kept)").arg(sts).arg(slotCnt));
        r.ck(sts == 2, "2 stations");
        r.ck(slotCnt == 2, QString("only 1 round kept, slots=%1").arg(slotCnt));
        r.ck(strHas(e1, "{5, 0, 6, 0, },"), "kept first round (5:00)");
        r.ck(!strHas(e1, "{5, 3, 6, 2, },"), "near-dup round removed");
        r.ck(!strHas(e1, "{15, 4, 16, 3, },"), "G1_2 round removed");
    });

    runCase("T21_tpf2_multifile_rounds", [&](CR& r) {
        QString dir = newCaseDir("T21_tpf2_multifile_rounds");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
        });
        writeCsv(dir + "/G1_2.csv", {
            {"G1/2", "上海", "0:35:00", "0:36:00"},
            {"G1/2", "苏州", "0:45:00", "0:46:00"},
        });
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f;
        std::string e1 = entryOf(after, 1, f);
        int sts = countMatches(QString::fromStdString(e1), "stationID");
        int slotCnt = countMatches(QString::fromStdString(e1), "\\{ ?\\d+, \\d+, \\d+, \\d+, ?\\},");
        r.nt(QString("multifile: stations=%1 slots=%2").arg(sts).arg(slotCnt));
        r.ck(sts == 2 && slotCnt == 4, "2 stations x 2 rounds");
    });

    runCase("T22_tpf2_list_mode_csv", [&](CR& r) {
        QString dir = newCaseDir("T22_tpf2_list_mode_csv");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        writeCsv(dir + "/G1.csv", {
            {"G1/2", "上海", "0:05:00", "0:06:00"},
            {"G1/2", "苏州", "0:15:00", "0:16:00"},
        });
        writeListXlsx(dir + "/test.sav_list.xlsx", {
            {QStringLiteral("G1"), QStringLiteral("G1.csv"), QString()},
        });
        mainui w;
        baseSetup(w, dir, "test.sav", dir + "/test.sav.lua", 0);
        w.sdata.easy_if = false;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/test.sav.lua");
        bool f;
        std::string e1 = entryOf(after, 1, f);
        r.ck(f && countMatches(QString::fromStdString(e1), "stationID") == 2, "2 stations via list");
    });

    // ---------------- 真实数据（只读副本）----------------
    runCase("R1_real_sync", [&](CR& r) {
        QString dir = newCaseDir("R1_real_sync");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        r.nt(QString("lines=%1 stations=%2").arg(w.sdata.line.size()).arg(w.sdata.station.size()));
        // 选一条"有 csv 的线路"用于检查
        QString pickName;
        int pickId = -1, pickRows = -1;
        for(const auto& p : w.sdata.line)
        {
            QString n = QString::fromStdString(p.first);
            if(QFile::exists(dir + "/" + n + ".csv")) { pickName = n; pickId = p.second; break; }
        }
        r.nt(QString("pick line: %1 id=%2").arg(pickName).arg(pickId));
        if(pickId > 0) pickRows = csvRowCount(dir + "/" + pickName + ".csv");
        QByteArray before = rd(dir + "/shanghai1.sav.lua");
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        size_t s1, e1, s2, e2;
        bool okSec = sectionRange(before, s1, e1) && sectionRange(after, s2, e2);
        r.ck(okSec, "sections found");
        if(okSec)
        {
            r.ck(before.left((int)s1) == after.left((int)s2), "prefix unchanged");
            r.ck(before.mid((int)e1) == after.mid((int)e2), "suffix unchanged");
        }
        {
            QStringList bks = QDir(dir).entryList(QStringList() << "*.backup", QDir::Files);
            r.ck(!bks.isEmpty(), "backup created");
        }
        if(pickId > 0)
        {
            bool f;
            std::string e = entryOf(after, pickId, f);
            int sts = countMatches(QString::fromStdString(e), "stationID");
            r.nt(QString("picked line stations in output: %1 (csv rows=%2)").arg(sts).arg(pickRows));
            r.ck(f && sts == pickRows, "station count == csv rows");
        }
    });

    runCase("R2_real_invalid_on", [&](CR& r) {
        QString dir = newCaseDir("R2_real_invalid_on");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        QString pickName;
        int pickId = -1, pickRows = -1;
        for(const auto& p : w.sdata.line)
        {
            QString n = QString::fromStdString(p.first);
            if(QFile::exists(dir + "/" + n + ".csv")) { pickName = n; pickId = p.second; break; }
        }
        if(pickId > 0) pickRows = csvRowCount(dir + "/" + pickName + ".csv");
        w.sdata.invalid_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        if(pickId > 0)
        {
            bool f;
            std::string e = entryOf(after, pickId, f);
            int sts = countMatches(QString::fromStdString(e), "stationID");
            r.nt(QString("invalid_on: %1 stations (csv rows=%2)").arg(sts).arg(pickRows));
            r.ck(f && sts == pickRows - 1, "invalid on drops last row");
        }
    });

    runCase("R3_real_list_mode", [&](CR& r) {
        QString dir = newCaseDir("R3_real_list_mode");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;   // 列表引用的“时刻表.xlsx”是 xlsx 模式
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.nt(QString("success=%1 dialogs=%2").arg(hasDialog(QString::fromUtf8("时刻表已成功导入"))).arg(g_dialogs.size()));
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "list-mode(xlsx) sync success");
    });

    runCase("R4_real_clear2", [&](CR& r) {
        QString dir = newCaseDir("R4_real_clear2");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        int beforeEntries = entryCount(rd(dir + "/shanghai1.sav.lua"));
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.ck(entryCount(after) == beforeEntries, QString("entry count %1 -> %2").arg(beforeEntries).arg(entryCount(after)));
        size_t s1, e1, s2, e2;
        bool okSec = sectionRange(rd(dir + "/shanghai1.sav.lua"), s1, e1) && sectionRange(after, s2, e2);
        r.nt(QString("clear2 done ok=%1").arg(okSec));
    });

    runCase("R5_real_clear3", [&](CR& r) {
        QString dir = newCaseDir("R5_real_clear3");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        int beforeEntries = entryCount(rd(dir + "/shanghai1.sav.lua"));
        w.sdata.clear_if = 3;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.ck(entryCount(after) == beforeEntries, QString("entry count %1 -> %2").arg(beforeEntries).arg(entryCount(after)));
    });

    runCase("R6_real_numeric_migrate", [&](CR& r) {
        QString dir = newCaseDir("R6_real_numeric_migrate");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 1);
        int beforeEntries = entryCount(rd(dir + "/shanghai1.sav.lua"));
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        size_t s, e;
        if(sectionRange(after, s, e))
        {
            QByteArray sect = after.mid((int)s, (int)(e - s));
            r.ck(!sect.contains("[\""), "no quoted keys left in section");
        }
        r.ck(entryCount(after) == beforeEntries, "entry count unchanged");
        size_t s1, e1, s2, e2;
        sectionRange(rd(dir + "/shanghai1.sav.lua"), s1, e1);   // 注意：读的是修改后的文件，仅做段定位用
    });

    // 列表模式 + 清空（用户反馈的场景）
    runCase("R7_real_list_clear2", [&](CR& r) {
        QString dir = newCaseDir("R7_real_list_clear2");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        int beforeEntries = entryCount(rd(dir + "/shanghai1.sav.lua"));
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        w.sdata.clear_if = 2;   // 清空line并导入
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.nt(QString("entry count %1 -> %2").arg(beforeEntries).arg(entryCount(after)));
        r.ck(entryCount(after) == beforeEntries, "entry count unchanged");
        int emptied = countEmptiedEntries(after);
        r.nt(QString("emptied entries: %1 (list covers all save lines)").arg(emptied));
        r.ck(emptied == 0, "no emptied entries");
        // 二次导入：确认生成的文件仍可被正常识别/处理
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "second run success dialog");
        QByteArray again = rd(dir + "/shanghai1.sav.lua");
        r.ck(again == after, "second run byte-identical (idempotent)");
    });

    runCase("R8_real_list_clear3", [&](CR& r) {
        QString dir = newCaseDir("R8_real_list_clear3");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        int beforeEntries = entryCount(rd(dir + "/shanghai1.sav.lua"));
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        w.sdata.clear_if = 3;   // 全部清空后导入
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.nt(QString("entry count %1 -> %2").arg(beforeEntries).arg(entryCount(after)));
        r.ck(entryCount(after) == beforeEntries, "entry count unchanged");
        int emptied = countEmptiedEntries(after);
        r.nt(QString("emptied entries: %1 (list covers all save lines)").arg(emptied));
        r.ck(emptied == 0, "no emptied entries");
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "second run success dialog");
        QByteArray again = rd(dir + "/shanghai1.sav.lua");
        r.ck(again == after, "second run byte-identical (idempotent)");
    });

    // 列表只覆盖部分线路 + 清空line：其余线路会被清成空条目
    runCase("R9_real_list_subset_clear2", [&](CR& r) {
        QString dir = newCaseDir("R9_real_list_subset_clear2");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        writeListXlsx(dir + "/shanghai1.sav_list.xlsx", {
            {QStringLiteral("1461"), QStringLiteral("时刻表.xlsx"), QStringLiteral("1461")},
            {QStringLiteral("1463"), QStringLiteral("时刻表.xlsx"), QStringLiteral("1463")},
            {QStringLiteral("K351"), QStringLiteral("时刻表.xlsx"), QStringLiteral("K351")},
            {QStringLiteral("Z1"), QStringLiteral("时刻表.xlsx"), QStringLiteral("Z1")},
        });
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.nt(QString("entry count %1, emptied %2").arg(entryCount(after)).arg(countEmptiedEntries(after)));
        bool f = false;
        std::string t1 = entryOf(after, 271862, f);   // T1：不在列表 → 预期被清空
        r.nt("T1(cleared) entry: " + QString::fromStdString(t1).left(160).replace("\n", "\\n"));
        r.ck(countEmptiedEntries(after) == 0, "no bare-empty entries (empty-timetable marker written)");
        r.ck(strHas(t1, "hasTimetable = false") && strHas(t1, "stations = { }"), "T1 cleared with empty-timetable marker");
        std::string k = entryOf(after, 348499, f);    // K351：在列表 → 应有数据
        r.ck(f && k.find("stationID") != std::string::npos, "listed line K351 keeps stations");
        std::string z = entryOf(after, 279234, f);    // Z1：在列表 → 应有数据
        r.ck(f && z.find("stationID") != std::string::npos, "listed line Z1 keeps stations");
    });

    // 存档中缺少某条线路的条目 + 列表包含它 → 走“新条目插入”分支
    runCase("R10_real_list_insert_missing", [&](CR& r) {
        QString dir = newCaseDir("R10_real_list_insert_missing");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        r.ck(entryCount(rd(dir + "/shanghai1.sav.lua")) == 24, "pre: 24 entries");
        r.ck(eraseEntry(dir + "/shanghai1.sav.lua", 279234), "erase Z1 entry");
        r.ck(entryCount(rd(dir + "/shanghai1.sav.lua")) == 23, "after erase: 23 entries");
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.nt(QString("entry count after insert: %1").arg(entryCount(after)));
        r.ck(entryCount(after) == 24, "Z1 re-inserted (24 entries)");
        bool f = false;
        std::string z = entryOf(after, 279234, f);
        r.ck(f && z.find("stationID") != std::string::npos, "Z1 entry has stations");
        r.nt("inserted Z1 entry: " + QString::fromStdString(z).left(240).replace("\n", "\\n"));
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "second run success");
    });

    // 数字键（d_version=1）+ 列表模式 + 清空
    runCase("R11_real_list_clear2_numeric", [&](CR& r) {
        QString dir = newCaseDir("R11_real_list_clear2_numeric");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 1);
        int beforeEntries = entryCount(rd(dir + "/shanghai1.sav.lua"));
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        size_t s, e;
        if(sectionRange(after, s, e))
        {
            QByteArray sect = after.mid((int)s, (int)(e - s));
            r.ck(!sect.contains("[\""), "no quoted keys left in section");
        }
        r.nt(QString("entry count %1 -> %2, emptied %3").arg(beforeEntries).arg(entryCount(after)).arg(countEmptiedEntries(after)));
        r.ck(entryCount(after) == beforeEntries, "entry count unchanged");
    });

    // 列表模式 + CSV + 清空
    runCase("R12_real_list_csv_clear2", [&](CR& r) {
        QString dir = newCaseDir("R12_real_list_csv_clear2");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        std::vector<std::array<QString, 3>> rows;
        for(const char* nm : {"1461", "1463", "K351", "Z1"})
            rows.push_back({QString::fromUtf8(nm), QString::fromUtf8(nm) + ".csv", QString()});
        writeListXlsx(dir + "/shanghai1.sav_list.xlsx", rows);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = false;
        w.sdata.xls_if = false;   // CSV
        w.sdata.clear_if = 2;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.ck(hasDialog(QString::fromUtf8("时刻表已成功导入")), "success dialog");
        QByteArray after = rd(dir + "/shanghai1.sav.lua");
        r.nt(QString("entry count %1, emptied %2").arg(entryCount(after)).arg(countEmptiedEntries(after)));
        bool f = false;
        std::string k = entryOf(after, 348499, f);
        r.ck(f && k.find("stationID") != std::string::npos, "listed K351 has stations");
    });

    // ============ 调试：上海-普速2 列表/简单模式（用户反馈复现） ============
    const QString LS2 = QStringLiteral("C:/Users/zm/AppData/Local/Temp/opencode/real_src_ls2_1006");
    runCase("S1_ls2_list_xlsx", [&](CR& r) {
        QString dir = newCaseDir("S1_ls2_list_xlsx");
        if(!QDir(LS2).exists()) { r.nt("LS2 source not present, skipped"); return; }
        if(!cpDir(LS2, dir)) { r.ck(false, "copy ls2 src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.nt(QString("dialogs=%1 success=%2").arg(g_dialogs.size()).arg(hasDialog(QString::fromUtf8("时刻表已成功导入"))));
    });
    runCase("S2_ls2_easy_csv", [&](CR& r) {
        QString dir = newCaseDir("S2_ls2_easy_csv");
        if(!QDir(LS2).exists()) { r.nt("LS2 source not present, skipped"); return; }
        if(!cpDir(LS2, dir)) { r.ck(false, "copy ls2 src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = true;
        w.sdata.xls_if = false;
        QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
        r.nt(QString("dialogs=%1 success=%2").arg(g_dialogs.size()).arg(hasDialog(QString::fromUtf8("时刻表已成功导入"))));
    });

    // ============ 调试：上海-普速2 各组合复现（用户反馈） ============
    runCase("S3_ls2_combos", [&](CR& r) {
        if(!QDir(LS2).exists()) { r.nt("LS2 source not present, skipped"); return; }
        struct Cfg { const char* name; bool easy; bool xls; int clear; bool invalid; bool pile; int dver; };
        std::vector<Cfg> cfgs = {
            {"list_clear1", false, true, 1, false, false, 0},
            {"list_clear2", false, true, 2, false, false, 0},
            {"list_clear3", false, true, 3, false, false, 0},
            {"list_invalid", false, true, 1, true, false, 0},
            {"list_pile", false, true, 1, false, true, 0},
            {"list_dver1", false, true, 1, false, false, 1},
            {"easy_xlsx", true, true, 1, false, false, 0},
        };
        for (const auto& c : cfgs)
        {
            QString dir = newCaseDir(QString("S3_") + QString::fromUtf8(c.name));
            if(!cpDir(LS2, dir)) { r.ck(false, "copy ls2 src"); return; }
            QDir::setCurrent(dir);
            g_dialogs.clear();
            mainui w;
            baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", c.dver);
            w.sdata.easy_if = c.easy;
            w.sdata.xls_if = c.xls;
            w.sdata.clear_if = c.clear;
            w.sdata.invalid_if = c.invalid;
            w.sdata.pile_if = c.pile;
            QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
            bool ok = hasDialog(QString::fromUtf8("时刻表已成功导入"));
            QString msg = QString("%1: success=%2 dialogs=%3").arg(QString::fromUtf8(c.name)).arg(ok).arg(g_dialogs.size());
            for(const auto& d : g_dialogs)
                if(d.contains(QString::fromUtf8("错误")))
                    msg += "\n      [ERR] " + QString(d).left(300).replace("\n", " | ");
            r.nt(msg);
        }
    });

    // 诊断：找出列表对应表单里“B 单元格存在但值为空”的异常行
    runCase("S4_diag_sheets", [&](CR& r) {
        QString dir = newCaseDir("S4_diag_sheets");
        if(!QDir(LS2).exists()) { r.nt("LS2 source not present, skipped"); return; }
        if(!cpDir(LS2, dir)) { r.ck(false, "copy ls2 src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        w.sdata.easy_if = false;
        w.sdata.xls_if = true;
        std::vector<std::pair<QString, QString>> sheetRefs;
        {
            QXlsx::Document lst(dir + "/shanghai1.sav_list.xlsx");
            for(int i = 2;; ++i)
            {
                QVariant ln = lst.read(i, 1);
                if(ln.isNull() || ln.toString().trimmed().isEmpty())
                    break;
                QVariant fv = lst.read(i, 2);
                QVariant sv = lst.read(i, 3);
                sheetRefs.push_back({stq(dir.toStdString()) + "/" + fv.toString().trimmed(),
                                     sv.toString().trimmed()});
            }
        }
        int groups = 0, found = 0;
        for(auto& fs2 : sheetRefs)
        {
            if(fs2.first.isEmpty() || fs2.second.isEmpty())
                continue;
            ++groups;
            QXlsx::Document doc(fs2.first);
            QString actual = fs2.second;
            for(const auto& p : doc.sheetNames())
                if(p.trimmed() == fs2.second)
                {
                    actual = p;
                    break;
                }
            doc.selectSheet(actual);
            QString prevStation;
            for(int i = 1; i < 500; ++i)
            {
                auto st = doc.cellAt(i, 2);
                auto arrt = doc.cellAt(i, 3);
                auto dept = doc.cellAt(i, 4);
                if(!st || !arrt || !dept)
                    break;
                QVariant sta = st->value();
                QVariant n2 = doc.read(i + 1, 2);
                QVariant n3 = doc.read(i + 1, 3);
                bool brk = (n2.isNull() || n2.toString().trimmed().isEmpty()) &&
                           (n3.isNull() || n3.toString().trimmed().isEmpty());
                if(brk)
                    break;
                if(sta.toString().trimmed().isEmpty())
                {
                    ++found;
                    r.nt(QString("FAIL sheet=%1 row=%2 prevRowStation=[%3]")
                         .arg(fs2.second).arg(i).arg(prevStation.left(20)));
                    r.nt(QString("     n2.isNull=%1 n2=[%2] n2type=%3 | n3.isNull=%4 n3=[%5] n3type=%6")
                         .arg(n2.isNull()).arg(n2.toString().left(30))
                         .arg(QString::fromUtf8(n2.typeName() ? n2.typeName() : "?"))
                         .arg(n3.isNull()).arg(n3.toString().left(30))
                         .arg(QString::fromUtf8(n3.typeName() ? n3.typeName() : "?")));
                    // 再看停在这一行时，正常模式（无 invalid）会怎么走
                    break;
                }
                prevStation = sta.toString();
                if(found >= 25)
                    break;
            }
            if(found >= 25)
                break;
        }
        r.nt(QString("list groups=%1 failRows=%2").arg(groups).arg(found));
    });

    // 用户反馈复现：表单末尾有“站点列为空、时间列残留 0:00:00”的单元格行 + 勾选忽略最后一行
    runCase("T24_tpf2_xlsx_empty_tail_row", [&](CR& r) {
        QString dir = newCaseDir("T24_tpf2_xlsx_empty_tail_row");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);
        {
            QXlsx::Document x;
            const QString st[4] = {QStringLiteral("上海"), QStringLiteral("苏州"),
                                   QStringLiteral("无锡"), QStringLiteral("常州")};
            const int am[4] = {5, 15, 25, 35};
            for(int i = 0; i < 4; ++i)
            {
                x.write(i + 1, 1, i + 1);
                x.write(i + 1, 2, st[i]);
                x.write(i + 1, 3, QDateTime(QDate(1980, 1, 1), QTime(0, am[i], 0)));
                x.write(i + 1, 4, QDateTime(QDate(1980, 1, 1), QTime(0, am[i] + 1, 0)));
            }
            // 清空残留行：站点为空字符串，时间列还有 0 值
            x.write(5, 2, QString());
            x.write(5, 3, QDateTime(QDate(1980, 1, 1), QTime(0, 0, 0)));
            x.write(5, 4, QDateTime(QDate(1980, 1, 1), QTime(0, 0, 0)));
            x.saveAs(dir + "/G1.xlsx");
        }
        int counts[2] = {-1, -1};
        for(int k = 0; k < 2; ++k)
        {
            QString d2 = dir + (k == 0 ? "/a" : "/b");
            QDir().mkpath(d2);
            cpDir(dir, d2);
            QDir::setCurrent(d2);
            g_dialogs.clear();
            mainui w;
            baseSetup(w, d2, "test.sav", d2 + "/test.sav.lua", 0);
            w.sdata.xls_if = true;
            w.sdata.invalid_if = (k == 1);
            QMetaObject::invokeMethod(&w, "on_sync_all_data_clicked");
            bool ok = hasDialog(QString::fromUtf8("时刻表已成功导入"));
            bool missing = hasDialog(QString::fromUtf8("站点不存在"));
            r.ck(ok, QString("invalid_%1: success").arg(k == 0 ? "off" : "on"));
            r.ck(!missing, QString("invalid_%1: no empty-name station dialog").arg(k == 0 ? "off" : "on"));
            QByteArray after = rd(d2 + "/test.sav.lua");
            bool f = false;
            std::string e1 = entryOf(after, 1, f);
            counts[k] = f ? countMatches(QString::fromStdString(e1), "stationID") : -1;
        }
        r.nt(QString("empty tail row: invalid_off=%1 invalid_on=%2 (expect 4 / 3)").arg(counts[0]).arg(counts[1]));
        r.ck(counts[0] == 4, "invalid off keeps 4");
        r.ck(counts[1] == 3, "invalid on drops last (3)");
    });

    // 换根目录后自动找回二代存档
    runCase("T25_tpf2_save_autopick", [&](CR& r) {
        QString dir = newCaseDir("T25_tpf2_save_autopick");
        QDir::setCurrent(dir);
        mkTpf2Synth(dir);   // test.sav.lua + test.sav_station/_line/_list.xlsx
        fs::path pick;
        // 1) 按原存档名找回
        r.ck(find_tpf2_save(fs::u8path(qs2s(dir)), "test.sav", pick), "find by same name");
        r.ck(pick.filename() == fs::u8path("test.sav.lua"), "picked test.sav.lua");
        // 2) 名字对不上时，唯一带配套文件的 lua
        fs::path pick2;
        r.ck(find_tpf2_save(fs::u8path(qs2s(dir)), "other.sav", pick2), "find by companion");
        r.ck(pick2 == pick, "same pick");
        // 3) 空目录找不到
        QString empty = dir + "/empty";
        QDir().mkpath(empty);
        fs::path pick3;
        r.ck(!find_tpf2_save(fs::u8path(qs2s(empty)), "", pick3), "none in empty dir");
    });

    // ---------------- data_add ----------------
    runCase("D1_dataadd_tpf3_station", [&](CR& r) {
        QString dir = newCaseDir("D1_dataadd_tpf3_station");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        data_add d(w.sdata);
        QMetaObject::invokeMethod(&d, "on_station_input_clicked");
        r.ck(hasDialog(QString::fromUtf8("站点数据导入成功")), "station import dialog");
        std::vector<std::pair<std::string, int>> rows;
        readXlsx(fs::u8path(qs2s(dir + "/tpf3_timetable_station.xlsx")), rows);
        r.ck(rows.size() == 3, QString("xlsx rows=%1").arg(rows.size()));
        r.ck(w.sdata.station.size() == 3, "sdata.station=3");
    });

    runCase("D2_dataadd_tpf3_line_trunc", [&](CR& r) {
        QString dir = newCaseDir("D2_dataadd_tpf3_line_trunc");
        QDir::setCurrent(dir);
        mkTpf3(dir);
        mainui w;
        baseSetup(w, dir, "tpf3_timetable", dir, 2);
        w.sdata.trunc_if = true;
        w.sdata.trunc = '/';
        data_add d(w.sdata);
        QMetaObject::invokeMethod(&d, "on_line_input_clicked");
        r.ck(hasDialog(QString::fromUtf8("线路数据导入成功")), "line import dialog");
        std::vector<std::pair<std::string, int>> rows;
        readXlsx(fs::u8path(qs2s(dir + "/tpf3_timetable_line.xlsx")), rows);
        r.nt(QString("line rows=%1").arg(rows.size()));
        bool g1 = false, y1461 = false;
        for(const auto& p : w.sdata.line)
        {
            if(p.first == "G1") g1 = true;
            if(p.first == "1461") y1461 = true;
        }
        r.ck(g1 && y1461, "truncated names G1/1461");
    });

    runCase("D3_dataadd_tpf2_real_station", [&](CR& r) {
        QString dir = newCaseDir("D3_dataadd_tpf2_real_station");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        std::vector<std::pair<std::string, int>> raw;
        read_id_data(fs::u8path(qs2s(dir + "/shanghai1.sav.lua")), raw, IDtype::STATION);
        r.nt(QString("read_id_data stations=%1").arg(raw.size()));
        data_add d(w.sdata);
        QMetaObject::invokeMethod(&d, "on_station_input_clicked");
        r.ck(hasDialog(QString::fromUtf8("站点数据导入成功")), "station import dialog");
        std::vector<std::pair<std::string, int>> rows;
        readXlsx(fs::u8path(qs2s(dir + "/shanghai1.sav_station.xlsx")), rows);
        r.ck(rows.size() == raw.size(), QString("xlsx %1 == idget %2").arg(rows.size()).arg(raw.size()));
    });

    runCase("D4_dataadd_tpf2_real_line", [&](CR& r) {
        QString dir = newCaseDir("D4_dataadd_tpf2_real_line");
        if(!cpDir(g_realSrc, dir)) { r.ck(false, "copy real src"); return; }
        QDir::setCurrent(dir);
        mainui w;
        baseSetup(w, dir, "shanghai1.sav", dir + "/shanghai1.sav.lua", 0);
        std::vector<std::pair<std::string, int>> raw;
        read_id_data(fs::u8path(qs2s(dir + "/shanghai1.sav.lua")), raw, IDtype::LINE);
        r.nt(QString("read_id_data lines=%1").arg(raw.size()));
        if(!raw.empty())
            r.nt(QString("sample line: %1 -> %2").arg(QString::fromStdString(raw.front().first)).arg(raw.front().second));
        w.sdata.trunc_if = true;
        w.sdata.trunc = '/';
        data_add d(w.sdata);
        QMetaObject::invokeMethod(&d, "on_line_input_clicked");
        r.ck(hasDialog(QString::fromUtf8("线路数据导入成功")), "line import dialog");
        std::vector<std::pair<std::string, int>> rows;
        readXlsx(fs::u8path(qs2s(dir + "/shanghai1.sav_line.xlsx")), rows);
        r.ck(rows.size() == raw.size(), QString("xlsx %1 == idget %2").arg(rows.size()).arg(raw.size()));
        if(!w.sdata.line.empty())
            r.nt(QString("sample stored: %1 -> %2").arg(QString::fromStdString(w.sdata.line.front().first)).arg(w.sdata.line.front().second));
    });

    rep("");
    rep(QString("SUMMARY: PASS=%1 FAIL=%2").arg(g_pass).arg(g_fail));
    perr(QString("SUMMARY: PASS=%1 FAIL=%2").arg(g_pass).arg(g_fail));
    g_report.close();
    return g_fail ? 1 : 0;
}
