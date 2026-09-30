#include "mainui.h"
#include "ui_mainui.h"
#include "util.h"
#include "data_add.h"

#include <Qtimer>
#include <QDateTime>
#include <QString>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <QProgressDialog>
#include <QTextEdit>
#include <QScreen>
#include <QFontMetrics>
#include <QSignalBlocker>
#include <QCheckBox>
#include <cstdlib>


#include "xlsxdocument.h"

#include "MarkdownLanguageManager.h"
#include "simplemarkdown.h"

std::string file_sufix(int i);

namespace fs = std::filesystem;

// TPF3 工作文件基名：<基名>_station.xlsx / _line.xlsx / _list.xlsx
static const char* const tpf3_work_name = "tpf3_timetable";


mainui::mainui(QWidget *parent)
    : QWidget(parent)
    , sdata()
    , ui(new Ui::mainui)
{
    ui->setupUi(this);

    // 连接语言切换信号
    connect(&MarkdownLanguageManager::instance(),
            &MarkdownLanguageManager::languageChanged,
            this, &mainui::onLanguageChanged);

    // 更新语言按钮文本
    updateLanguageButton();

    m_easyGroup = new QButtonGroup(this);
    m_easyGroup->addButton(ui->easymode, 1);
    m_easyGroup->addButton(ui->listmode, 0);

    m_xlsGroup = new QButtonGroup(this);
    m_xlsGroup->addButton(ui->xlsxmode, 1);
    m_xlsGroup->addButton(ui->csvmode, 0);

    m_clearGroup = new QButtonGroup(this);
    m_clearGroup->addButton(ui->clear_1, 1);
    m_clearGroup->addButton(ui->clear_2, 2);
    m_clearGroup->addButton(ui->clear_3, 3);
    setWindowTitle(tr("狂热运输2/3 时刻表自动输入") + tr(" V2.0"));

    QObject::connect(m_easyGroup, &QButtonGroup::idClicked,
                     this, [&](int p) {
                         sdata.easy_if = p;
                         refresh_file(sdata);
                     });

    QObject::connect(m_xlsGroup, &QButtonGroup::idClicked,
                     this, [&](int p) {
                         sdata.xls_if = p;
                         refresh_file(sdata);
                     });

    QObject::connect(m_clearGroup, &QButtonGroup::idClicked,
                     this, [&](int p) {
                         sdata.clear_if = p;
                         refresh_file(sdata);
                     });

    QObject::connect(ui->lastvalidation, &QCheckBox::toggled,
                     this, [&](bool p) {
                         sdata.invalid_if = p;
                         refresh_file(sdata);
                     });

    QObject::connect(ui->pile_if, &QCheckBox::toggled,
                     this, [&](bool p) {
                         sdata.pile_if = p;
                         refresh_file(sdata);
                     });

    QObject::connect(ui->version_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this, [&](int p) {
                         if(p < 0 || p > 1)   // 越界索引（-1）不进状态
                             return;
                         sdata.d_version = p;
                         sdata.tpf2_version = p;
                         refresh_file(sdata);
                         refresh();
                     });

    QObject::connect(ui->cycle_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this, [&](int p) {
                         sdata.cycle_index = p;
                         refresh_file(sdata);
                     });

    // “当前steam用户”：手动切换桥接数据目录所在的账号
    QObject::connect(ui->steam_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this, [&](int idx) {
                         if(!sdata.tpf3() || idx < 0)
                             return;
                         const QString path = ui->steam_combo->itemData(idx).toString();
                         if(path.isEmpty())
                             return;   // “未检测到”占位项
                         sdata.steam_user = ui->steam_combo->itemText(idx).toStdString();
                         sdata.sg_dir = fs::u8path(path.toStdString());
                         sdata.sg_name = tpf3_work_name;
                         sdata.sys_save_dir = sdata.sg_dir;
                         sdata.probe_ok = false;
                         sdata.probe_save_id.clear();
                         sdata.probe_cycle_sec = 0;
                         read_station_line();
                         refresh_file(sdata);
                         refresh();
                     });

    QTimer::singleShot(0, this, &mainui::init);
}

// 旧版数据目录名不再使用：命中时强制重新检测
static bool is_legacy_bridge_dir(const fs::path& dir)
{
    const std::string n = dir.filename().u8string();
    return n == "autofill_bridge" || n == "tpf3_autofill_bridge";
}

void mainui::init()
{
    std::ifstream sys_file(sys_file_name);
    std::string buf;
    updateLanguageButton();
    if(sys_file)
    {
        if(getline(sys_file,buf,'\n'))
        {
            sdata.folder_dir = fs::u8path(buf);
            sdata.folder_name = sdata.folder_dir.stem().u8string();
        }

        if(getline(sys_file,buf,'\n'))
        {
            sdata.sg_dir = fs::u8path(buf);
            sdata.sg_name = sdata.sg_dir.stem().u8string();
        }

        if(getline(sys_file,buf,'\n'))
            sdata.sys_save_dir = fs::u8path(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.trunc = buf.front();

        if(getline(sys_file,buf,'\n'))
            sdata.trunc_if = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.easy_if = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.xls_if = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.invalid_if = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.clear_if = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.pile_if = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.d_station_add = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.d_line_add = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.d_clear2_warning = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.d_version = qBound(0, std::stoi(buf), 2);

        if(getline(sys_file,buf,'\n'))
            sdata.cycle_index = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.steam_user = buf;

        if(getline(sys_file,buf,'\n'))
            sdata.d_tpf3_notice = std::stoi(buf);

        if(getline(sys_file,buf,'\n'))
            sdata.tpf2_version = qBound(0, std::stoi(buf), 1);
        if(!sdata.tpf3())
            sdata.tpf2_version = sdata.d_version;

        if(getline(sys_file,buf,'\n'))
            sdata.tpf2_sg_dir = fs::u8path(buf);

        // 三代模式先修正工作文件基名，再读站点/线路表（否则会用桥接目录名去读，读不到）
        if(sdata.tpf3())
            sdata.sg_name = tpf3_work_name;

        read_station_line();

        sys_file.close();

        ui->easymode->setChecked(sdata.easy_if);
        ui->listmode->setChecked(!sdata.easy_if);
        ui->xlsxmode->setChecked(sdata.xls_if);
        ui->csvmode->setChecked(!sdata.xls_if);

        ui->lastvalidation->setChecked(sdata.invalid_if);

        ui->clear_1->setChecked(sdata.clear_if == 1 ? 1 : 0);
        ui->clear_2->setChecked(sdata.clear_if == 2 ? 1 : 0);
        ui->clear_3->setChecked(sdata.clear_if == 3 ? 1 : 0);

        ui->pile_if->setChecked(sdata.pile_if);

        {
            // 注意：三代时 d_version(=2) 对版本下拉是越界值，必须加信号锁、
            // 并只把“二代子版本”填进下拉，否则下拉被置空(-1)的信号会污染状态
            QSignalBlocker block(ui->version_combo);
            ui->version_combo->setCurrentIndex(sdata.tpf3() ? qBound(0, sdata.tpf2_version, 1)
                                                            : qBound(0, sdata.d_version, 1));
        }

        if(sdata.tpf3() && (sdata.sg_dir.empty() || !fs::is_directory(sdata.sg_dir)
                            || is_legacy_bridge_dir(sdata.sg_dir)))
            ensure_tpf3_dir();

        refresh();
        if(!fs::exists(sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")))
        {
            QXlsx::Document doc;
            QXlsx::Format songTi20;
            songTi20.setFontName(tr("宋体"));
            songTi20.setFontSize(20);

            doc.currentWorksheet()->setColumnFormat(1, 6, songTi20);

            doc.write(1, 1, tr("线路"));
            doc.write(1, 2, tr("文件1"));
            doc.write(1, 3, tr("表单名称"));
            doc.write(1, 4, tr("文件2"));
            doc.write(1, 5, tr("表单名称"));
            doc.write(1, 6, "...");

            doc.saveAs(stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string()));

            QString q = tr("未检测到列表文件，已自动生成") + stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string());
            q += tr("，如采用列表模式请编辑该文件\n格式见文档，每行一个线路，如有更多文件请向后加。"
                 "对于文件中的某些表单，请以空格分隔。"
                 "如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。");
            display_info(tr("提示"), std::move(q));

        }
        if(sdata.tpf3())
            start_bridge_probe(true);   // 启动即为三代模式：自动刷新一次（静默）
        return;
    }

    ui->easymode->setChecked(sdata.easy_if);
    ui->listmode->setChecked(!sdata.easy_if);
    ui->xlsxmode->setChecked(sdata.xls_if);
    ui->csvmode->setChecked(!sdata.xls_if);

    ui->lastvalidation->setChecked(sdata.invalid_if);

    ui->clear_1->setChecked(sdata.clear_if == 1 ? 1 : 0);
    ui->clear_2->setChecked(sdata.clear_if == 2 ? 1 : 0);
    ui->clear_3->setChecked(sdata.clear_if == 3 ? 1 : 0);

    ui->pile_if->setChecked(sdata.pile_if);

    ui->version_combo->setCurrentIndex(sdata.d_version);

    refresh();

    if(!get_folder())
        return;
    get_sg();
}

mainui::~mainui()
{
    delete ui;
}


void mainui::on_change_sg_clicked()
{
    if(sdata.tpf3())
    {
        // “刷新”：确保数据目录可用、重读 export.lua（存档名/周期），
        // 并向游戏发送一次实时探测；游戏没响应时保留导出文件里的信息
        ensure_tpf3_dir();
        refresh();
        start_bridge_probe();
        return;
    }
    get_sg();
}


void mainui::on_input_data_clicked()
{
    refresh_file(sdata);
    read_station_line();
    if(sdata.folder_dir.empty())
    {
        display_info(tr("提示"), tr("请先选取工作文件夹"));
        return;
    }
    if(sdata.sg_dir.empty())
    {
        display_info(tr("提示"), sdata.tpf3()
                     ? tr("尚未设置桥接数据目录，请点击右上角“切换到三代”按钮自动检测")
                     : tr("请先选取存档"));
        return;
    }
    read_station_line();
    data_add *adding = new data_add(sdata);
    adding->setAttribute(Qt::WA_DeleteOnClose);
    connect(adding, &data_add::destroyed, this, &mainui::refresh);
    adding->show();


    refresh();

}


void mainui::on_sync_all_data_clicked()
{
    refresh_file(sdata);
    read_station_line();
    refresh();
    std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>> lists;

    try
    {
        if(!get_list(lists))
            return;

        std::vector<std::pair<int, std::vector<stationinfo>>> data;

        if(!get_data(lists, data))
            return;

        if(sdata.tpf3())
        {
            write_data_lua(sdata, data);
            return;
        }


        std::string all_data;

        for(auto &[lineid, stationdata]:data)
            all_data += get_filetext(lineid, stationdata);

        write_to_lua(sdata.sg_dir, all_data, lists, sdata.line, sdata.clear_if, sdata.d_version == 0);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        display_info(tr("错误"), stq(e.what()));
    }
    catch (const std::exception &e)
    {
        display_info(tr("错误"), stq(e.what()));
    }

}



// “当前steam用户”的候选：所有存在桥接数据目录的账号
struct bridge_candidate
{
    std::string account;   // userdata 下的账号文件夹名（Steam 账号 ID）
    fs::path path;         // ...\userdata\<账号>\3493540\local\tpf3_timetable_bridge
};

static bool same_path(const fs::path& a, const fs::path& b)
{
    std::error_code ec;
    return fs::equivalent(a, b, ec);
}

// 自动定位“狂热运输3”桥接数据目录（steam userdata/<账号>/3493540/local/tpf3_timetable_bridge）
static std::vector<bridge_candidate> collect_bridge_candidates()
{
    std::vector<bridge_candidate> out;
    std::vector<fs::path> roots;
#ifdef _WIN32
    const char* pf86 = std::getenv("ProgramFiles(x86)");
    const char* pf = std::getenv("ProgramFiles");
    if(pf86) roots.push_back(fs::path(pf86) / "Steam" / "userdata");
    if(pf) roots.push_back(fs::path(pf) / "Steam" / "userdata");
    roots.push_back(fs::path("C:/Program Files (x86)/Steam/userdata"));
#else
    const char* home = std::getenv("HOME");
    if(home) roots.push_back(fs::path(home) / "Library/Application Support/Steam/userdata");
#endif
    for(const auto& root : roots)
    {
        std::error_code ec;
        if(!fs::is_directory(root, ec))
            continue;
        for(const auto& entry : fs::directory_iterator(root, ec))
        {
            if(ec)
                break;
            fs::path cand = entry.path() / "3493540" / "local" / "tpf3_timetable_bridge";
            std::error_code ec2;
            if(!fs::is_directory(cand, ec2))
                continue;
            bool dup = false;
            for(const auto& o : out)
                if(same_path(o.path, cand))
                {
                    dup = true;
                    break;
                }
            if(!dup)
                out.push_back({entry.path().filename().u8string(), cand});
        }
    }
    return out;
}

// 选择桥接数据目录：优先已保存的账号；否则取 export.lua 最新的（通常=最近玩过的），再否则第一个
static fs::path choose_bridge_dir(const std::string& preferred, std::string& picked)
{
    std::vector<bridge_candidate> cands = collect_bridge_candidates();
    if(cands.empty())
        return {};

    if(!preferred.empty())
        for(const auto& c : cands)
            if(c.account == preferred)
            {
                picked = c.account;
                return c.path;
            }

    size_t best = 0;
    fs::file_time_type bestTime{};
    bool hasTime = false;
    for(size_t i = 0; i < cands.size(); ++i)
    {
        std::error_code ec;
        auto t = fs::last_write_time(cands[i].path / fs::path("export.lua"), ec);
        if(ec)
            continue;
        if(!hasTime || t > bestTime)
        {
            hasTime = true;
            bestTime = t;
            best = i;
        }
    }
    picked = cands[best].account;
    return cands[best].path;
}

void mainui::ensure_tpf3_dir()
{
    // 已有且有效则保留（旧名目录不再使用，强制重新检测）
    std::error_code ec;
    if(!sdata.sg_dir.empty() && fs::is_directory(sdata.sg_dir, ec)
       && !is_legacy_bridge_dir(sdata.sg_dir))
    {
        sdata.sg_name = tpf3_work_name;
        // 补齐“当前steam用户”（按扫描结果匹配当前目录，仅用于记住选择）
        for(const auto& c : collect_bridge_candidates())
            if(c.account != sdata.steam_user && same_path(c.path, sdata.sg_dir))
            {
                sdata.steam_user = c.account;
                refresh_file(sdata);
                break;
            }
        return;
    }

    // 自动检测：优先上次保存的账号；没有保存则自动挑一个（export.lua 最新的）
    std::string picked;
    fs::path detected = choose_bridge_dir(sdata.steam_user, picked);
    if(!detected.empty())
    {
        sdata.sg_dir = detected;
        sdata.steam_user = picked;
        sdata.sg_name = tpf3_work_name;
        sdata.sys_save_dir = detected;
        sdata.probe_ok = false;
        sdata.probe_save_id.clear();
        sdata.probe_cycle_sec = 0;
        refresh_file(sdata);
        read_station_line();
        return;
    }

    // 自动检测失败：提示后进入手动选择
    display_info(tr("未找到数据目录"),
                 tr("未能自动找到桥接数据目录（tpf3_timetable_bridge）。\n如果还没出现过，请先进一次游戏；现在请手动选择。"));
    get_sg();
}

void mainui::on_switch_tpf3_clicked()
{
    if(sdata.tpf3())
    {
        // 切回二代：恢复上次的二代子版本（0/1）和二代存档。
        // 二代工作文件带存档名前缀、三代前缀统一为 tpf3_timetable，
        // 必须先恢复前缀再重新检测站点/线路数据，否则“存在站点/线路数据”是错的
        sdata.d_version = qBound(0, sdata.tpf2_version, 1);

        if(!sdata.tpf2_sg_dir.empty() && fs::exists(sdata.tpf2_sg_dir))
        {
            sdata.sg_dir = sdata.tpf2_sg_dir;
            sdata.sg_name = sdata.sg_dir.stem().u8string();
            sdata.sys_save_dir = sdata.sg_dir.parent_path();
        }
        else
        {
            sdata.sg_dir.clear();
            sdata.sg_name.clear();
        }

        read_station_line();
        refresh_file(sdata);
        refresh();
        return;
    }

    // 首次切到三代：注意事项（可勾选“下次不再提示”）
    if(!sdata.d_tpf3_notice)
    {
        QMessageBox box(this);
        box.setWindowTitle(tr("切换到狂热运输3"));
        box.setTextFormat(Qt::RichText);
        box.setText(tr(
            "<b>注意！请仔细阅读以下内容，三代逻辑与二代差异较大</b>"
            "<ol>"
            "<li>如要使用三代，请先在 mod.io 里安装“Timetable AutoFill”模组；"
            "建议使用方法：先打开游戏进入存档，然后打开本程序——导入是实时的，"
            "不需要像二代一样导入后重新进游戏。</li>"
            "<li>三代无存档文件概念，程序会实时检测当前游戏存档；如更换存档，"
            "请加载完后点“刷新”。请确保当前存档与文件夹内数据一致——"
            "文件夹内数据名均为 tpf3_timetable_xxx；请确保不同存档使用不同的文件夹。</li>"
            "<li>使用流程：打开游戏存档、打开程序、进行线路和站点的导入（还是和二代一样，不用每次都导入）、"
            "数据导入，最后在游戏内 mod 界面点“将导入数据应用至时刻表”即可完成。</li>"
            "<li>如果游戏中途增加了站点或者线路，请按游戏内的“导出存档站点线路信息”按钮，"
            "再在程序内重新导入站点线路数据。</li>"
            "<li>没有存档文件，所以没有备份，请务必确认数据正确后再保存你的存档！</li>"
            "<li>三代有全局周期小时数概念，如有小时数据溢出会报错。</li>"
            "<li>如果你有多个 Steam 账号同时玩《狂热运输》并玩时刻表，请自行选择账号"
            "（在“路径”栏的“当前steam用户”行切换）。如果只有一个账号，"
            "或者只有一个账号玩《狂热运输3》时刻表，那么系统会自动锁定账号，无需手动选择。</li>"
            "<li>如果碰到任何问题，请立刻联系作者。</li>"
            "</ol>"));
        QCheckBox *dontShow = new QCheckBox(tr("下次不再提示"), &box);
        box.setCheckBox(dontShow);
        QPushButton *ok = box.addButton(tr("确定"), QMessageBox::AcceptRole);
        box.setDefaultButton(ok);
        box.exec();
        if(dontShow->isChecked())
        {
            sdata.d_tpf3_notice = true;
            refresh_file(sdata);
        }
    }

    // 记住二代存档（三代工作文件前缀统一为 tpf3_timetable，切回二代时要靠它恢复存档前缀）
    if(!sdata.sg_dir.empty())
        sdata.tpf2_sg_dir = sdata.sg_dir;

    sdata.d_version = 2;
    ensure_tpf3_dir();
    read_station_line();        // 按三代前缀（tpf3_timetable_*）重新检测站点/线路数据
    refresh_file(sdata);
    refresh();
    start_bridge_probe(true);   // 切到三代后自动刷新一次（静默）
}


void mainui::start_bridge_probe(bool silent)
{
    m_probe_silent = silent;
    if(!sdata.tpf3() || sdata.sg_dir.empty())
        return;
    std::error_code ec;
    if(!fs::is_directory(sdata.sg_dir, ec))
        return;

    m_probe_token = QString::number(QDateTime::currentMSecsSinceEpoch());
    if(!write_probe_file(sdata.sg_dir, m_probe_token.toStdString()))
        return;

    if(!m_probe_timer)
    {
        m_probe_timer = new QTimer(this);
        m_probe_timer->setInterval(250);
        connect(m_probe_timer, &QTimer::timeout, this, &mainui::poll_bridge_probe);
    }
    m_probe_left = 20;   // 20 × 250ms = 5 秒
    m_probe_timer->start();

    if(!silent)
    {
        ui->change_sg->setEnabled(false);
        ui->change_sg->setText(tr("检测中…"));
    }
}


void mainui::poll_bridge_probe()
{
    if(!sdata.tpf3())
    {
        m_probe_timer->stop();
        ui->change_sg->setEnabled(true);
        return;
    }

    std::string token;
    std::string save_id;
    int cycle_sec = 0;
    if(read_probe_result(sdata.sg_dir / fs::path("probe_result.lua"), token, save_id, cycle_sec)
       && token == m_probe_token.toStdString())
    {
        m_probe_timer->stop();
        sdata.probe_ok = true;
        sdata.probe_save_id = save_id;
        sdata.probe_cycle_sec = cycle_sec;
        refresh();
        ui->change_sg->setEnabled(true);
        return;
    }

    if(--m_probe_left <= 0)
    {
        m_probe_timer->stop();
        ui->change_sg->setEnabled(true);
        refresh();
        if(!m_probe_silent)
            display_info(tr("未检测到游戏"),
                         tr("已向游戏发送刷新请求，但游戏内没有响应。\n"
                            "请确认游戏正在运行、已进入存档，并且新版桥接模组已启用。\n"
                            "当前显示的是上次导出的信息。"));
    }
}


bool mainui::get_folder()
{
    display_info(tr("选择目录"), tr("请选取所有数据文件的根目录，即保存所有时刻表文件的目录。随后的车站和线路编号信息也都会存放于此"));


    fs::path folder = fs::u8path((QFileDialog::getExistingDirectory(this,
    tr("选择目录"),
    sdata.folder_dir.empty()? "" : QString::fromStdString(sdata.folder_dir.u8string()),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)).toStdString());

    if(folder.empty())
        return 0;

    std::string folder_name = folder.stem().u8string();

    const int keep_version = sdata.d_version;
    const int keep_cycle = sdata.cycle_index;

    sdata = {};

    sdata.d_version = keep_version;
    sdata.cycle_index = keep_cycle;
    sdata.folder_name = folder_name;
    sdata.folder_dir = folder;

    if(sdata.tpf3())
    {
        // 三代没有“选存档”这一步：换完根目录后在这里完成数据目录检测，
        // 并按二代“选完存档”的习惯生成列表文件
        ensure_tpf3_dir();
        if(!sdata.sg_name.empty() &&
           !fs::exists(sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")))
        {
            QXlsx::Document doc;
            QXlsx::Format songTi20;
            songTi20.setFontName(tr("宋体"));
            songTi20.setFontSize(20);

            doc.currentWorksheet()->setColumnFormat(1, 6, songTi20);

            doc.write(1, 1, tr("线路"));
            doc.write(1, 2, tr("文件1"));
            doc.write(1, 3, tr("表单名称"));
            doc.write(1, 4, tr("文件2"));
            doc.write(1, 5, tr("表单名称"));
            doc.write(1, 6, tr("..."));

            doc.saveAs(stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string()));

            QString q = tr("未检测到列表文件，已自动生成") + stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string());
            q += tr("，如采用列表模式请编辑该文件\n格式见文档，每行一个线路，如有更多文件请向后加。"
                 "对于文件中的某些表单，请以空格分隔。"
                 "如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。");
            display_info(tr("提示"), std::move(q));
        }
    }

    refresh();
    refresh_file(sdata);

    return 1;
}


bool mainui::get_sg()
{
    if(sdata.tpf3())
    {
        display_info(tr("选择数据目录"),
                     tr("“狂热运输3”模式下请选取桥接数据目录（tpf3_timetable_bridge），通常在：\n"
                        "C:\\Program Files (x86)\\Steam\\userdata\\<你的ID>\\3493540\\local\\tpf3_timetable_bridge"));

        fs::path dir = fs::u8path((QFileDialog::getExistingDirectory(this,
            tr("选择目录"),
            sdata.sg_dir.empty()? "" : QString::fromStdString(sdata.sg_dir.u8string()),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)).toStdString());

        if(dir.empty())
            return 0;

        sdata.sg_dir = dir;
        sdata.sg_name = tpf3_work_name;
        sdata.sys_save_dir = dir;

        if(!fs::exists(sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")))
        {
            QXlsx::Document doc;
            QXlsx::Format songTi20;
            songTi20.setFontName(tr("宋体"));
            songTi20.setFontSize(20);

            doc.currentWorksheet()->setColumnFormat(1, 6, songTi20);

            doc.write(1, 1, tr("线路"));
            doc.write(1, 2, tr("文件1"));
            doc.write(1, 3, tr("表单名称"));
            doc.write(1, 4, tr("文件2"));
            doc.write(1, 5, tr("表单名称"));
            doc.write(1, 6, tr("..."));

            doc.saveAs(stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string()));

            QString q = tr("未检测到列表文件，已自动生成") + stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string());
            q += tr("，如采用列表模式请编辑该文件\n格式见文档，每行一个线路，如有更多文件请向后加。"
                 "对于文件中的某些表单，请以空格分隔。"
                 "如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。");
            display_info(tr("提示"), std::move(q));
        }

        if(sdata.folder_dir.empty())
            return 1;

        read_station_line();

        refresh();
        refresh_file(sdata);

        return 1;
    }


    display_info(tr("选择存档lua文件"), tr("选取存档，默认应该为“C:\\Program Files (x86)\\Steam\\user"
                                    "data\\XXXX\\1066780\\local\\save\\xxx.lua”，取决于steam安装位置"));

    fs::path sg = fs::u8path((QFileDialog::getOpenFileName(this,
    tr("打开文件"),
    sdata.sys_save_dir.empty()? "C:\\Program Files (x86)\\Steam\\userdata" :
                                                               QString::fromStdString(sdata.sys_save_dir.u8string()),
    tr("lua文件 (*.lua);;所有文件 (*.*)"))).toStdString());

    if(sg.empty())
        return 0;

    std::string sg_name = sg.stem().u8string();

    sdata.sys_save_dir = sg.parent_path();

    sdata.sg_name = sg_name;
    sdata.sg_dir = sg;
    sdata.tpf2_sg_dir = sg;   // 记住二代存档，供从三代切回时恢复

    if(!fs::exists(sdata.folder_dir / fs::u8path(sg_name + "_list.xlsx")))
    {
        QXlsx::Document doc;
        QXlsx::Format songTi20;
        songTi20.setFontName(tr("宋体"));
        songTi20.setFontSize(20);

        doc.currentWorksheet()->setColumnFormat(1, 6, songTi20);

        doc.write(1, 1, tr("线路"));
        doc.write(1, 2, tr("文件1"));
        doc.write(1, 3, tr("表单名称"));
        doc.write(1, 4, tr("文件2"));
        doc.write(1, 5, tr("表单名称"));
        doc.write(1, 6, "...");

        doc.saveAs(stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string()));

        QString q = tr("未检测到列表文件，已自动生成") + stq((sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx")).u8string());
        q += tr("，如采用列表模式请编辑该文件\n格式见文档，每行一个线路，如有更多文件请向后加。"
             "对于文件中的某些表单，请以空格分隔。"
             "如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。");
        display_info(tr("提示"), std::move(q));
    }


    if(sdata.folder_dir.empty())
        return 1;

    read_station_line();

    refresh();
    refresh_file(sdata);



    return 1;
}

void mainui::read_station_line()
{
    readXlsx(sdata.folder_dir / fs::u8path(sdata.sg_name + u8"_station.xlsx"), sdata.station);
    readXlsx(sdata.folder_dir / fs::u8path(sdata.sg_name + u8"_line.xlsx"), sdata.line);
}


void mainui::refresh()
{
    auto setPathText = [this](QLabel *l, const QString &full) {
        l->setText(QFontMetrics(l->font()).elidedText(full, Qt::ElideMiddle, l->maximumWidth()));
        l->setToolTip(full);
    };
    setPathText(ui->dir_name, sdata.folder_name.empty()? tr("无") : QString::fromStdString(sdata.folder_name));
    ui->station_status->setText(sdata.station.empty()? tr("否"):tr("是"));
    ui->line_status->setText(sdata.line.empty()? tr("否"):tr("是"));
    ui->station_status->setStyleSheet(sdata.station.empty()?
        QStringLiteral("color: #c62828; font-weight: bold;") :
        QStringLiteral("color: #2e7d32; font-weight: bold;"));
    ui->line_status->setStyleSheet(sdata.line.empty()?
        QStringLiteral("color: #c62828; font-weight: bold;") :
        QStringLiteral("color: #2e7d32; font-weight: bold;"));

    // TPF3 模式：存档行保留为纯文本（显示引擎返回的存档名），按钮变“刷新”；
    // 同步版本下拉、右列大按钮与标题里的代数
    const bool tpf3 = sdata.tpf3();

    {
        QSignalBlocker block(ui->version_combo);
        ui->version_combo->setCurrentIndex(tpf3 ? qBound(0, sdata.tpf2_version, 1)
                                                : qBound(0, sdata.d_version, 1));
    }
    if(tpf3)
    {
        ui->switch_tpf3->setText(tr("切换回二代"));
        ui->switch_tpf3->setStyleSheet(QStringLiteral(
            "QPushButton { background-color: #546e7a; color: white; border-radius: 5px; padding: 3px 12px; font-weight: bold; }"));
    }
    else
    {
        ui->switch_tpf3->setText(tr("切换到三代"));
        ui->switch_tpf3->setStyleSheet(QStringLiteral(
            "QPushButton { background-color: #2e7d32; color: white; border-radius: 5px; padding: 3px 12px; font-weight: bold; }"));
    }

    ui->label->setText(tr("狂热运输2/3 时刻表mod自动录入"));

    export_info bridgeInfo;
    bool bridgeRead = false;
    if(tpf3 && !sdata.sg_dir.empty())
        bridgeRead = read_export_file(sdata.sg_dir / fs::path("export.lua"), bridgeInfo);

    if(tpf3)
    {
        // 实时探测（“刷新”按钮）优先，其次 export.lua（自动导出/上次导出）
        setPathText(ui->savegame_name,
                    (sdata.probe_ok && !sdata.probe_save_id.empty())
                        ? stq(sdata.probe_save_id)
                        : (bridgeRead && !bridgeInfo.save_id.empty())
                              ? stq(bridgeInfo.save_id)
                              : tr("未读取"));
        ui->text2->setText(tr("当前游戏中存档："));
        if(!(m_probe_timer && m_probe_timer->isActive() && !m_probe_silent))
            ui->change_sg->setText(tr("刷新"));
        const int cycle_sec = (sdata.probe_ok && sdata.probe_cycle_sec > 0)
                                  ? sdata.probe_cycle_sec
                                  : (bridgeRead ? bridgeInfo.cycle_sec : 0);
        ui->cycle_label->setText(cycle_sec > 0
            ? tr("原存档：%1").arg(cycle_label_from_sec(cycle_sec))
            : tr("原存档：未读取"));

        // “当前steam用户”：列出所有存在桥接数据目录的账号（多账号时手动切换）
        QStringList steamItems;
        QStringList steamPaths;
        for(const auto& c : collect_bridge_candidates())
        {
            steamItems << stq(c.account);
            steamPaths << stq(c.path.u8string());
        }
        if(steamItems != m_steam_items || ui->steam_combo->count() == 0)
        {
            m_steam_items = steamItems;
            QSignalBlocker block(ui->steam_combo);
            ui->steam_combo->clear();
            if(steamItems.isEmpty())
                ui->steam_combo->addItem(tr("未检测到"), QString());
            else
                for(int i = 0; i < steamItems.size(); ++i)
                    ui->steam_combo->addItem(steamItems[i], steamPaths[i]);
        }
        {
            QSignalBlocker block(ui->steam_combo);
            int steamIdx = -1;
            const QString curPath = stq(sdata.sg_dir.u8string());
            for(int i = 0; i < ui->steam_combo->count(); ++i)
            {
                const QString p = ui->steam_combo->itemData(i).toString();
                if(!p.isEmpty() && p.compare(curPath, Qt::CaseInsensitive) == 0)
                {
                    steamIdx = i;
                    break;
                }
            }
            if(steamIdx < 0 && steamItems.isEmpty())
                steamIdx = 0;
            ui->steam_combo->setCurrentIndex(steamIdx);
        }
        ui->steam_combo->setEnabled(!steamItems.isEmpty());
    }
    else
    {
        setPathText(ui->savegame_name, sdata.sg_name.empty()? tr("无") : QString::fromStdString(sdata.sg_name));
        ui->text2->setText(tr("存档名称："));
        ui->change_sg->setText(tr("更改存档"));
    }

    // “当前steam用户”行只在 TPF3 下显示
    ui->steam_label->setVisible(tpf3);
    ui->steam_combo->setVisible(tpf3);

    // “兼容版本”栏在 TPF3 下整体变为“周期”（小时数）选项
    ui->group_version->setTitle(tpf3 ? tr("周期") : tr("兼容版本"));
    ui->version_combo->setVisible(!tpf3);
    ui->cycle_label->setVisible(tpf3);
    ui->cycle_combo->setVisible(tpf3);
    {
        QSignalBlocker block(ui->cycle_combo);
        ui->cycle_combo->setCurrentIndex(sdata.cycle_index);
    }
}


bool mainui::get_list(std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& list)
{
    if(sdata.line.empty())
    {
        errortype e{errortype::LINE_EMPTY};
        return false;
    }

    if(sdata.station.empty())
    {
        errortype e{errortype::STATION_EMPTY};
        return false;
    }

    std::string prefix = sdata.xls_if ? ".xlsx" : ".csv";

    std::unordered_multiset<std::string> count;
    for(const auto &[name, id]:sdata.line)
        count.insert(name);

    if(sdata.easy_if)
    {


        for(const auto &[name, id] : sdata.line)
        {
            std::vector<std::pair<QString, QString>> vec;

            for(int i = 0;;++i)
            {
                fs::path temppath = sdata.folder_dir / fs::u8path(name + file_sufix(i) + prefix);

                QString o = stq(temppath.u8string());

                std::error_code ec;
                if(!fs::exists(temppath, ec) || ec)
                    break;

                if(count.count(name) > 1)
                {
                    errortype e{errortype::MULTI_LINE, stq(name)};
                    return false;
                }

                if(!sdata.xls_if)
                {
                    vec.push_back({stq(temppath.u8string()), ""});
                    continue;
                }


                QString qfilename = QString::fromStdString(temppath.u8string());
                QXlsx::Document doc(qfilename);
                QStringList q = doc.sheetNames();
                if(q.isEmpty())
                    continue;
                std::transform(q.begin(), q.end(),
                               std::back_inserter(vec),
                               [&](const auto &a){return std::make_pair(qfilename, a);});
            }
            if(!vec.empty())
                list.push_back({id, vec});
        }
    }
    else
    {
        fs::path file = sdata.folder_dir / fs::u8path(sdata.sg_name + "_list.xlsx");
        if(!fs::exists(file))
        {
            errortype e{errortype::LISTMODE_NOLIST};
            return false;
        }
        QXlsx::Document doc(stq(file.u8string()));
        int i = 2;
        while(1)
        {
            std::vector<std::pair<QString, QString>> vec;
            QVariant linenamev = doc.read(i, 1);
            if(linenamev.isNull() || linenamev.toString().trimmed().isEmpty())
                break;
            QString linename = linenamev.toString().trimmed();
            auto it = std::find_if(sdata.line.begin(), sdata.line.end(),
                                   [&](const auto& p){return p.first == linename;});
            if(it == sdata.line.end())
            {
                errortype e{errortype::LIST_LINE_DONT_EXIST, linename};
                return false;
            }

            if(count.count(it->first) > 1)
            {
                errortype e{errortype::MULTI_LINE, stq(it->first)};
                return false;
            }

            int id = it->second;

            for(int j = 2;;j += 2)
            {
                QVariant filenamev = doc.read(i, j);
                if(filenamev.isNull() || filenamev.toString().trimmed().isEmpty())
                    break;
                QString filename = stq(sdata.folder_dir.u8string()) + '/' + filenamev.toString().trimmed();

                if(!QFile::exists(filename))
                {
                    errortype e{errortype::LIST_FILE_DONT_EXIST, filename};
                    return false;
                }

                QString expectedSuffix = sdata.xls_if ? "xlsx" : "csv";
                if(QFileInfo(filename).suffix().toLower() != expectedSuffix)
                {
                    display_info(tr("错误"),
                                 tr("列表模式下，文件 %1 不是 %2 格式，请检查文件格式选项或修改列表文件")
                                     .arg(filename).arg(expectedSuffix.toUpper()));
                    return false;
                }

                if(!sdata.xls_if)
                {
                    vec.push_back({filename, ""});
                    continue;
                }

                QVariant sheetv = doc.read(i, j + 1);

                if(sheetv.isNull() || sheetv.toString().trimmed().isEmpty())
                    vec.push_back({filename, ""});
                else
                {
                    QStringList sheet = sheetv.toString().trimmed().split(" ", Qt::SkipEmptyParts);
                    for(auto &s:sheet)
                        vec.push_back({filename, s.trimmed()});
                }
            }
            if(!vec.empty())
                list.push_back({id, vec});
            ++i;
        }
    }

    if(list.empty())
    {
        errortype e{errortype::NO_TIMETABLE};
        return false;
    }

    std::unordered_map<int, size_t> firstOccurrence;

    // 第一遍：记录每个first值的首次出现位置
    for (size_t i = 0; i < list.size(); ++i) {
        int key = list[i].first;
        if (firstOccurrence.find(key) == firstOccurrence.end()) {
            firstOccurrence[key] = i;
        }
    }

    // 第二遍：合并到首次出现的位置
    for (size_t i = 0; i < list.size(); ++i) {
        int key = list[i].first;
        size_t targetIndex = firstOccurrence[key];

        if (targetIndex != i) {
            // 合并到目标位置
            auto& targetVec = list[targetIndex].second;
            auto& sourceVec = list[i].second;

            targetVec.insert(
                targetVec.end(),
                std::make_move_iterator(sourceVec.begin()),
                std::make_move_iterator(sourceVec.end())
                );

            // 清空当前元素（可选）
            sourceVec.clear();
        }
    }

    // 第三遍：移除已合并的元素
    std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>> result;
    result.reserve(firstOccurrence.size());

    for (size_t i = 0; i < list.size(); ++i) {
        int key = list[i].first;
        if (firstOccurrence[key] == i) {
            // 这是首次出现的元素，保留
            result.push_back(std::move(list[i]));
        }
    }

    list = std::move(result);

    for(auto &[key, vec]:list)
    {
        if (vec.empty())
            continue;

        std::unordered_set<QString> seen1;
        size_t writeIndex = 0;  // 写入位置

        for (size_t readIndex = 0; readIndex < vec.size(); ++readIndex) {
            // 尝试插入，如果成功（说明首次出现）则保留
            if (seen1.insert(vec[readIndex].first + vec[readIndex].second).second) {
                if (readIndex != writeIndex) {
                    vec[writeIndex] = std::move(vec[readIndex]);
                }
                writeIndex++;
            }
            // 否则是重复的，跳过
        }

        vec.resize(writeIndex);
        vec.shrink_to_fit();
    }
    return 1;

}


std::string file_sufix(int i)
{
    if(i == 0)
        return "";
    std::string s ="_" + std::to_string(i+1);
    return s;
}


bool mainui::get_data(std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& list,
                        std::vector<std::pair<int, std::vector<stationinfo>>> &output)
{
    int shrinkif = 0;
    std::vector<int> shrink_if;
    std::vector<int> shrink_num;
    std::vector<std::pair<int, std::vector<int>>> shrink_index;
    int inde = 0;

    std::unordered_map<QString, fileinfo> files;

    std::unordered_set<QString> leng;

    for(auto& line:list)
        for(auto& sheet:line.second)
            leng.insert(sheet.first);

    int progress = 0;

    QProgressDialog progressDialog;
    progressDialog.setWindowTitle(tr("打开文件"));
    progressDialog.setLabelText(tr("正在打开文件..."));
    progressDialog.setRange(0, leng.size());
    progressDialog.setValue(0);
    progressDialog.setCancelButton(nullptr); // 不显示取消按钮
    progressDialog.setMinimumDuration(0);    // 立即显示
    progressDialog.setModal(true);           // 模态对话框
    progressDialog.show();

    QApplication::processEvents(); // 立即更新UI


    for(auto& line : list)
    {
        std::vector<int> stations;
        std::vector<std::vector<arrdeptime>> times;

        if(sdata.xls_if)
        {
            std::vector<std::pair<QString, QString>> temp;
            for(auto& sheet:line.second)
            {
                if(files.find(sheet.first) == files.end())
                {
                    // 更新进度文本
                    progressDialog.setLabelText(QString(tr("正在打开文件：%1")).arg(QFileInfo(sheet.first).fileName()));
                    progressDialog.setValue(progress);

                    // 强制UI更新
                    QApplication::processEvents();

                    progress++;


                    files.emplace(
                        sheet.first,
                        fileinfo{std::make_unique<QXlsx::Document>(sheet.first), {}});

                    QStringList q = files[sheet.first].doc->sheetNames();
                    for(auto &p: q)
                    {
                        files[sheet.first].sheets.try_emplace(p.trimmed(), p);
                    }
                    if(q.empty())
                    {
                        errortype{errortype::NO_SHEET_IN_FILE, sheet.first};
                        return false;
                    }
                }


                if(sheet.second == "")
                {
                    for(auto &p: files[sheet.first].sheets)
                        temp.emplace_back(sheet.first, p.second);
                }
                else
                {
                    auto it = files[sheet.first].sheets.find(sheet.second);
                    if(it == files[sheet.first].sheets.end())
                    {
                        errortype{errortype::LIST_SHEET_DONT_EXIST, sheet.second};
                        return false;
                    }
                    temp.emplace_back(sheet.first, files[sheet.first].sheets[sheet.second]);
                }
            }

            line.second = std::move(temp);
        }

        for(auto& sheet:line.second)
        {
            std::vector<int> local_stations;
            std::vector<arrdeptime> linetime;

            if(!sdata.xls_if)
            {
                fs::path filename = fs::u8path(sheet.first.toStdString());

                // 更新进度文本
                progressDialog.setLabelText(QString(tr("正在打开文件：%1")).arg(stq(filename.filename().u8string())));
                progressDialog.setValue(progress);

                // 强制UI更新
                QApplication::processEvents();

                progress++;


                std::vector<CSVData> data = readCSV(filename);
                if(data.empty())
                {
                    errortype e{errortype::NO_LINE_STAT, stq(filename.u8string())};
                    return false;
                }

                if(sdata.invalid_if)
                    data.pop_back();

                for(auto &dat:data)
                {
                    if(sdata.station.count(dat.col2) > 1)
                    {
                        errortype e{errortype::MULTI_STATION, stq(dat.col2)};
                        return false;
                    }
                    auto it = sdata.station.find(dat.col2);
                    if(it == sdata.station.end())
                    {
                        errortype e{errortype::STATION_DONT_EXIST, stq(dat.col2)};
                        return false;
                    }
                    local_stations.push_back(it->second);
                }

                for (auto &p : data)
                {
                    TimeComponents t1 = parseCSVTime(p.col3);
                    TimeComponents t2 = parseCSVTime(p.col4);
                    arrdeptime a;
                    if(!t1() || !t2())
                    {
                        errortype e{errortype::TIME_INVALID, stq(filename.u8string())};
                        return false;
                    }
                    a.arrmin = t1.minutes;
                    a.arrsec = t1.seconds;
                    a.depmin = t2.minutes;
                    a.depsec = t2.seconds;
                    if(sdata.tpf3())
                    {
                        // TPF3 模式下时:分:秒的“时”计入总分钟（周期内偏移）
                        a.arrmin = t1.hours * 60 + t1.minutes;
                        a.depmin = t2.hours * 60 + t2.minutes;
                    }
                    linetime.push_back(a);
                }
            }
            else
            {
                auto &doc = files[sheet.first].doc;
                doc->selectSheet(sheet.second);

                for(size_t i = 1;;++i)
                {
                    auto st = doc->cellAt(i, 2);
                    auto arrt = doc->cellAt(i, 3);
                    auto dept = doc->cellAt(i, 4);

                    if(!st || !arrt || !dept)
                        break;



                    QVariant sta = st->value();
                    QVariant arrtime = arrt->dateTime();
                    QVariant deptime = dept->dateTime();

                    if(sdata.invalid_if)
                    {
                        // 末行校验：看下一行的第 2、3 列；都为空说明表格到此结束，
                        // 当前行（最后一行数据，通常是排图回起点的站）不录入
                        QVariant n2 = doc->read(i + 1, 2);
                        QVariant n3 = doc->read(i + 1, 3);
                        if((n2.isNull() || n2.toString().trimmed().isEmpty()) &&
                           (n3.isNull() || n3.toString().trimmed().isEmpty()))
                            break;
                    }
                    else if(sta.isNull() || sta.toString().trimmed().isEmpty() ||
                            arrtime.isNull() || arrtime.toString().trimmed().isEmpty() ||
                            deptime.isNull() || deptime.toString().trimmed().isEmpty() )
                        break;

                    if(sdata.station.count(sta.toString().trimmed().toUtf8().toStdString()) > 1)
                    {
                        errortype e{errortype::MULTI_STATION, sta.toString()};
                        return false;
                    }

                    auto it = sdata.station.find(sta.toString().trimmed().toUtf8().toStdString());
                    if(it == sdata.station.end())
                    {
                        errortype e{errortype::STATION_DONT_EXIST, sta.toString().trimmed()};
                        return false;
                    }

                    local_stations.push_back(it->second);

                    auto arr = read_xlsx_time(arrtime, sdata.tpf3());
                    auto dep = read_xlsx_time(deptime, sdata.tpf3());
                    if(arr.first == -1 || dep.first == -1)
                    {
                        errortype e{errortype::TIME_INVALID, sheet.first};
                        return false;
                    }
                    linetime.push_back({arr.first, arr.second, dep.first, dep.second});
                }
            }

            std::vector<int> station_index(local_stations.size(), 1);
            size_t length = local_stations.size();

            if(sdata.pile_if)
            {
                for(size_t m = 1;m < local_stations.size();++m)
                {
                    int valid = 0;
                    if(local_stations[m] == local_stations[0])
                    {
                        valid = 1;
                        int n = 1;
                        for(;n*m < local_stations.size();++n)
                        {
                            for(int p = 0;p < m ;++p)
                            {
                                if(n * m + p == local_stations.size())
                                {
                                    valid = 0;
                                    break;
                                }
                                if(local_stations[n * m + p] == local_stations[p])
                                    station_index[n * m + p] = n + 1;
                                else
                                {
                                    valid = 0;
                                    break;
                                }
                            }
                            if(valid == 0)
                                break;
                        }
                        if(valid == 0)
                        {
                            station_index.assign(local_stations.size(), 1);
                            m = n * m;
                        }
                        else
                        {
                            length = m;
                            break;
                        }
                    }
                }
            }


            if(stations.empty())
            {
                stations.reserve(length);
                stations.assign(
                    std::make_move_iterator(local_stations.begin()),
                    std::make_move_iterator(local_stations.begin() + length)
                );
            }
            else
            {
                if(stations.size() != length)
                {
                    errortype e{errortype::STATION_MISMATCH, get_linename(sdata.line, line.first)};
                    return false;
                }
                for(size_t i = 0;i < stations.size(); ++i)
                    if(stations[i] != local_stations[i])
                    {
                        errortype e{errortype::STATION_MISMATCH, get_linename(sdata.line, line.first)};
                        return false;
                    }
            }

            int i = 1;
            std::vector<arrdeptime> actual;
            for(int index = 0;index < local_stations.size();++index)
            {
                if(station_index[index] != i)
                {
                    ++i;
                    times.push_back(actual);
                    actual.clear();
                }
                actual.push_back(linetime[index]);
            }
            times.push_back(actual);
        }



        shrink_if.push_back(0);

        std::vector<int> ind(times.size());
        std::iota(ind.begin(), ind.end(), 0);

        for (size_t i = 0; i < times.size(); ++i)
        {
            for (size_t j = i + 1; j < times.size(); ++j)
            {
                int iif = 0;
                for(size_t l = 0; l < stations.size(); ++l)
                {
                    if(!(times[i][l] - times[j][l])())
                    {
                        iif = 1;
                    }
                    else if(iif == 1)
                    {
                        iif = 0;
                        break;
                    }
                }
                if(iif == 1)
                    ind[j] = ind[i];
            }
        }
        std::unordered_set<int> s(ind.begin(),ind.end());

        shrink_num.push_back(s.size());

        if(s.size() < ind.size())
        {
            shrink_if[inde] = 1;
            shrinkif = 1;
        }
        shrink_index.emplace_back(line.first, ind);
        inde++;

        // 相近/重复的组真的只保留一组（保留第一组），与确认框“保留一组并继续”一致
        if(s.size() < ind.size())
        {
            std::vector<std::vector<arrdeptime>> kept;
            kept.reserve(s.size());
            for(size_t i = 0; i < times.size(); ++i)
                if(static_cast<size_t>(ind[i]) == i)
                    kept.push_back(std::move(times[i]));
            times = std::move(kept);
        }

        std::vector<stationinfo> local_station_info;

        for(int i = 0;i < stations.size();++i)
        {
            stationinfo temp;
            temp.stationid = stations[i];
            for(auto &p:times)
                temp.arrdep.push_back(p[i]);
            local_station_info.emplace_back(temp);
        }
        output.emplace_back(line.first, local_station_info);
    }

    progressDialog.setValue(leng.size());
    progressDialog.close();

    QString prefix = tr("时刻表数据如下");

    QString content;

    for (int i = 0;i < output.size();++i)
    {
        content += QString(tr("%1 : %2组数据"))
                 .arg(get_linename(sdata.line, output[i].first))
                 .arg(shrink_num[i]);
        if(shrink_if[i])
            content += tr("     包含相近或重复数据");
        QString last;
        for(auto &p :list[i].second)
        {
            if(last != p.first)
            {
                content += "\n";
                last = p.first;
                content += QFileInfo(p.first).fileName();
                content += ":";
                content += p.second.size() ? p.second : QFileInfo(p.first).baseName();
            }
            else
            {
                content += "  ";
                content += p.second.size() ? p.second : QFileInfo(p.first).baseName();
            }
        }
        content += "\n";
        content += "\n";
    }

    QString suffix;
    switch (sdata.clear_if) {
    case 1:
        suffix += tr("当前为覆盖模式，即仅添加或更新现有时刻表\n");
        break;
    case 2:
        suffix += tr("当前为列表清空，即仅清空_line文件内包含的列表\n");
        break;
    case 3:
        suffix += tr("当前为全部清空模式，即删除所有原时刻表\n");
        break;
    default:
        break;
    }

    suffix += shrinkif ?
                    tr("当前时刻表存在重复或者相近（两组或多组时刻表所有站点时刻<5s），\n"
                    "按确认则随机保留一组并继续，按取消则返回") :
                    tr("请核对时刻表数量以及表单，按确认继续");

    if(!printq(prefix, content, suffix))
        return 0;

    if(sdata.clear_if == 2 && sdata.d_clear2_warning)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("警告"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("该操作会清空 %1_line.xlsx 里的线路对应到存档内的时刻表数据，是否确认？")
                           .arg(QString::fromStdString(sdata.sg_name)));
        msgBox.setStandardButtons(QMessageBox::NoButton);
        QPushButton *okButton = msgBox.addButton(tr("确认"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(okButton);
        QCheckBox *dontShow = new QCheckBox(tr("下次不再提示"), &msgBox);
        msgBox.setCheckBox(dontShow);
        msgBox.exec();
        if(msgBox.clickedButton() != okButton)
            return 0;
        if(dontShow->isChecked())
        {
            sdata.d_clear2_warning = false;
            refresh_file(sdata);
        }
    }


    return 1;
}


/**
 * @brief 生成单个时间槽的Lua配置
 * @param time 时间数据
 * @param indent 缩进级别（制表符数量）
 * @return 时间槽Lua字符串
 */
std::string mainui::generateTimeSlot(const arrdeptime& time, int indent) {
    std::ostringstream oss;
    std::string indentStr(indent, '\t');

    oss << indentStr << "{"
        << time.arrmin << ", "
        << time.arrsec << ", "
        << time.depmin << ", "
        << time.depsec << ", },\n";

    return oss.str();
}

/**
 * @brief 生成单个站点的Lua配置
 * @param station 站点信息
 * @param indent 缩进级别（制表符数量）
 * @return 站点Lua配置字符串
 */
std::string mainui::generateStationLua(const stationinfo& station, int indent) {
    std::ostringstream oss;
    std::string indentStr(indent, '\t');           // 第6层：6个制表符
    std::string indentStrPlus1(indent + 1, '\t');  // 第7层：7个制表符
    std::string indentStrPlus2(indent + 2, '\t');  // 第8层：8个制表符

    oss << indentStr << "{\n";
    oss << indentStrPlus1 << "conditions = {\n";
    oss << indentStrPlus2 << "ArrDep = {\n";

    // 生成所有时间槽（第9层缩进）
    for (const auto& time : station.arrdep) {
        oss << generateTimeSlot(time, indent + 3);
    }

    oss << indentStrPlus2 << "},\n";
    oss << indentStrPlus2 << "type = \"ArrDep\",\n";
    oss << indentStrPlus1 << "},\n";
    oss << indentStrPlus1 << "stationID = " << station.stationid << ",\n";
    oss << indentStr << "},\n";

    return oss.str();
}

/**
 * @brief 生成整个线路的Lua配置（从第3层缩进开始）
 * @param lineId 线路ID
 * @param stations 站点数据向量
 * @return 线路Lua配置字符串
 */
std::string mainui::get_filetext(int lineId, const std::vector<stationinfo>& stations) {
    std::ostringstream oss;

    // 根据文档结构：
    // 第1层：function data() 和 return {
    // 第2层：["timetable_gui.lua"] = {
    // 第3层：timetable = {
    // 第4层：[lineId] = {
    // 第5层：stations = {
    // 第6层：{ 开始每个站点

    oss << "\t\t\t[" << lineId << "] = {\n";           // 第3层：3个制表符
    oss << "\t\t\t\tfrequency = 1,\n";                 // 第4层：4个制表符
    oss << "\t\t\t\thasTimetable = true,\n";           // 第4层：4个制表符
    oss << "\t\t\t\tstations = {\n";                   // 第4层：4个制表符

    // 生成所有站点
    for (const auto& station : stations) {
        oss << generateStationLua(station);  // 从第6层开始
    }

    oss << "\t\t\t\t},\n";                   // 第4层：4个制表符
    oss << "\t\t\t},\n";                     // 第3层：3个制表符

    return oss.str();
}

void mainui::on_settinginfo_clicked()
{

    QString markdown = loadMarkdownContent("mainui");
    QString title = tr("说明");
    SimpleWebMarkdownDialog::showDialog(title, markdown, this);

}

void mainui::on_settinginfo_2_clicked()
{
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("Version 2.0<br/>" +
        QString(QObject::tr("作者：今天学高代了吗<br/>"
                                       "b站视频教程：<a href=\"https://www.bilibili.com/video/BV1xxaZ6WE2E\">"
                                       "https://www.bilibili.com/video/BV1xxaZ6WE2E</a> <br/>"
                                       "github：<a href=\"https://github.com/zm0423/tpf2_autofill\"> "
                                       "https://github.com/zm0423/tpf2_autofill</a> <br/>"
                                       "邮箱：15800733391@163.com <br/>"
                                       "2025.12.14")));

    msgBox.setWindowFlags(Qt::Dialog);
    msgBox.setWindowTitle(tr("关于"));
    msgBox.setIcon(QMessageBox::NoIcon);

    // 设置中文按钮
    msgBox.setStandardButtons(QMessageBox::NoButton);
    QPushButton *yesButton = msgBox.addButton(tr("确定"), QMessageBox::AcceptRole);
    msgBox.setDefaultButton(yesButton);
    msgBox.exec();
    return;
}


void mainui::on_language_button_clicked()
{
    MarkdownLanguageManager::instance().toggleLanguage();

    // 更新按钮文本
    updateLanguageButton();
}

void mainui::updateLanguageButton()
{
    ui->language_button->setText(
        MarkdownLanguageManager::instance().languageButtonText()
        );
}

void mainui::onLanguageChanged()
{

    // 更新按钮文本
    updateLanguageButton();
    ui->retranslateUi(this);
    // retranslateUi 会用翻译文件里的旧窗口标题覆盖构造函数里设置的标题，这里补回
    setWindowTitle(tr("狂热运输2/3 时刻表自动输入") + tr(" V2.0"));
    refresh();
}

QString mainui::loadMarkdownContent(const QString &docName)
{
    return MarkdownLanguageManager::instance().loadMarkdown(docName);
}


