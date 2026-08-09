#include "util.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <QDebug>
#include <QMessageBox>
#include <stdexcept>
#include <fstream>
#include <unordered_set>
#include <chrono>
#include <iomanip>


#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <string>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>



bool find_next_entry(const std::string& text, size_t start, size_t end,
                     int& lid, size_t& entryStart, size_t& bodyStart, size_t& bodyEnd);
size_t find_entry_end(const std::string& text, size_t bodyStart);
bool remove_field(std::string& text, size_t start, size_t end, const std::string& name);




#include "xlsxdocument.h"

namespace fs = std::filesystem;


bool get_first_cut(std::string& linename, const char token) {
    size_t firstPos = linename.find_first_of(token);
    if (firstPos == std::string::npos) {
        return 0;
    }
    linename = linename.substr(0, firstPos);
    return 1;
}

void readXlsx(const std::filesystem::path& filename, std::vector<std::pair<std::string, int>> &result)
{
    result.clear();
    if(!std::filesystem::exists(filename))
        return;

    QXlsx::Document xls(stq(filename.u8string()));

    for(int count = 1;;++count)
    {
        QVariant a = xls.read(count, 1);
        QVariant b = xls.read(count, 2);
        if(a.isNull() || a.toString().trimmed().isEmpty() ||
            b.isNull() || b.toString().trimmed().isEmpty())
            break;
        bool v;
        result.push_back({a.toString().trimmed().toUtf8().toStdString(), b.toInt(&v)});
        if(!v)
        {
            display_info(QObject::tr("错误"),QObject::tr("站点或线路文件第二列含有非数字，请修改后重新打开软件"));
            result.clear();
            return;
        }
    }
    return;
}

void readXlsx(const std::filesystem::path& filename, std::unordered_multimap<std::string, int> &result)
{
    result.clear();

    if(!std::filesystem::exists(filename))
        return;

    QXlsx::Document xls(stq(filename.u8string()));


    for(int count = 1;;++count)
    {
        QVariant a = xls.read(count, 1);
        QVariant b = xls.read(count, 2);
        if(a.isNull() || a.toString().trimmed().isEmpty() ||
            b.isNull() || b.toString().trimmed().isEmpty())
            break;
        bool v;
        result.insert({a.toString().trimmed().toUtf8().toStdString(), b.toInt(&v)});
        if(!v)
        {
            display_info(QObject::tr("错误"),QObject::tr("站点或线路文件第二列含有非数字，请修改后重新打开软件"));
            result.clear();
            return;
        }

    }
    return;
}


bool writeVectorToXlsx(const std::vector<std::pair<std::string, int>>& data,
                   const fs::path& filepath, const std::vector<std::pair<int, int>>& sort)
{
    QXlsx::Document xls;
    QXlsx::Format songTi20;
    songTi20.setFontName(QObject::tr("宋体"));
    songTi20.setFontSize(20);

    xls.currentWorksheet()->setColumnFormat(1, 2, songTi20);

    int count = 1;

    if(sort.empty())
        std::for_each(data.cbegin(),
                  data.cend(),
                  [&](const auto& p){
                      xls.write(count, 1, stq(p.first));
                      xls.write(count, 2, p.second);
                      ++count;
                  });
    else
        std::for_each(sort.cbegin(),
                      sort.cend(),
                      [&](const auto& p){
                          xls.write(count, 1, stq(data.at(p.first).first));
                          xls.write(count, 2, data.at(p.first).second);
                          ++count;
                      });

    if(!xls.saveAs(stq(filepath.u8string())))
    {
        display_info(QObject::tr("错误"),QObject::tr("文件存储失败，请检查文件夹权限、是否被其他应用打开、或磁盘空间"));
        return 0;
    }
    return 1;
}



EndingType checkEnding(const std::string& input) {
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

    if (lowerInput.length() >= 3 &&
        lowerInput.compare(lowerInput.length() - 3, 3, u8"路") == 0) {
        return EndingType::ROAD;
    }

    if (lowerInput.length() >= 3 &&
        lowerInput.compare(0, 3, u8"前") == 0) {
        return EndingType::STRANGE_STATION;
    }

    if (lowerInput.length() >= 6 &&
        lowerInput.compare(lowerInput.length() - 6, 6, u8"侧站") == 0) {
        return EndingType::STRANGE_STATION;
    }

    if (lowerInput.length() >= 6 &&
        lowerInput.compare(lowerInput.length() - 6, 6, u8"分站") == 0) {
        return EndingType::STRANGE_STATION;
    }

    if (lowerInput.length() >= 6 &&
        lowerInput.compare(0, 6, u8"降低") == 0) {
        return EndingType::STRANGE_STATION;
    }

    if (lowerInput.length() >= 4 &&
        lowerInput.compare(lowerInput.length() - 4, 4, u8"road") == 0) {
        return EndingType::ROAD;
    }

    return EndingType::NO_MATCH;
}

void display_info(const QString& head, const QString& info)
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::Dialog);
    msgBox.setWindowTitle(head);
    msgBox.setText(info);
    msgBox.setIcon(QMessageBox::Information);

    // 设置中文按钮
    msgBox.setStandardButtons(QMessageBox::NoButton);
    QPushButton *yesButton = msgBox.addButton(QObject::tr("确定"), QMessageBox::AcceptRole);
    msgBox.setDefaultButton(yesButton);
    msgBox.exec();
    return;
}


bool write_to_lua(const std::filesystem::path& filename,
                  const std::string &data,
                  const std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& id,
                  const std::vector<std::pair<std::string, int>> & line,
                  int clear_if)
{
    std::ifstream infile(filename, std::ios::binary);

    if(!infile)
    {
        errortype{errortype::SAVE_FILE_UNOPEN};
        return false;
    }

    std::string text((std::istreambuf_iterator<char>(infile)),
                     std::istreambuf_iterator<char>());
    infile.close();

    std::unordered_set<int> toReplace;
    std::unordered_set<int> toClear;
    bool clearAll = false;

    if(clear_if == 1)
        for(auto &p:id)
            toReplace.emplace(p.first);
    else if(clear_if == 2)
        for(auto &p:line)
            toClear.emplace(p.second);
    else if(clear_if == 3)
        clearAll = true;

    // 解析生成的数据：按线路ID缓存整块文本和内部字段文本
    std::unordered_map<int, std::string> fullBlocks;
    std::unordered_map<int, std::string> innerBlocks;
    {
        size_t i = 0;
        while(i < data.size())
        {
            size_t bs = data.find('[', i);
            if(bs == std::string::npos)
                break;
            size_t p = bs + 1;
            if(p < data.size() && data[p] == '"')
                ++p;
            size_t ds = p;
            while(p < data.size() && std::isdigit(static_cast<unsigned char>(data[p])))
                ++p;
            if(p == ds)
                break;
            int lid = std::stoi(data.substr(ds, p - ds));
            size_t eq = data.find('=', p);
            if(eq == std::string::npos)
                break;
            size_t lb = data.find('{', eq);
            if(lb == std::string::npos)
                break;
            int bc = 1;
            size_t b = lb + 1;
            for(; b < data.size() && bc > 0; ++b)
            {
                if(data[b] == '{')
                    ++bc;
                else if(data[b] == '}')
                    --bc;
            }
            if(bc != 0)
                break;
            size_t rb = b - 1;
            std::string inner = data.substr(lb + 1, rb - lb - 1);
            while(!inner.empty() && (inner.front() == '\n' || inner.front() == '\r'))
                inner.erase(inner.begin());
            while(!inner.empty() && (inner.back() == '\n' || inner.back() == '\r' ||
                                     inner.back() == '\t' || inner.back() == ' '))
                inner.pop_back();
            inner += "\n";
            innerBlocks[lid] = std::move(inner);
            size_t blockEnd = rb + 1;
            if(blockEnd < data.size() && data[blockEnd] == ',')
                ++blockEnd;
            if(blockEnd < data.size() && data[blockEnd] == '\n')
                ++blockEnd;
            fullBlocks[lid] = data.substr(bs, blockEnd - bs);
            i = blockEnd;
        }
    }

    // 查找 timetable 块
    size_t pos = text.find("timetable = {");
    if(pos == std::string::npos)
    {
        errortype{errortype::NO_TIMETABLE_MOD};
        return false;
    }

    size_t blockStart = pos + 12; // "timetable = " 长度
    if(blockStart >= text.size() || text[blockStart] != '{')
    {
        errortype{errortype::NO_TIMETABLE_MOD};
        return false;
    }

    // 匹配 timetable 块的结束位置
    int braceCount = 1;
    size_t blockEnd = blockStart + 1;
    for(; blockEnd < text.size() && braceCount > 0; ++blockEnd)
    {
        if(text[blockEnd] == '{')
            braceCount++;
        else if(text[blockEnd] == '}')
            braceCount--;
    }
    if(braceCount != 0)
    {
        errortype{errortype::NO_TIMETABLE_MOD};
        return false;
    }
    --blockEnd; // 指向 '}'

    // 探测存档使用的键格式：新版为字符串键 ["id"]，旧版为数字键 [id]
    bool quotedKeys = false;
    {
        size_t q = text.find("[\"", blockStart);
        if(q != std::string::npos && q < blockEnd)
            quotedKeys = true;
    }

    // 扫描块内所有线路条目（只读）
    struct Entry { size_t bodyStart; size_t bodyEnd; int lid; };
    std::vector<Entry> entries;
    {
        size_t scan = blockStart + 1;
        int lid;
        size_t es, bs, be;
        while(find_next_entry(text, scan, blockEnd, lid, es, bs, be))
        {
            entries.push_back({bs, be, lid});
            scan = be + 1;
        }
    }

    // 从后往前处理：只删除/替换受管字段，其余内容保留
    for(auto it = entries.rbegin(); it != entries.rend(); ++it)
    {
        bool needRemove = clearAll || toClear.count(it->lid) || toReplace.count(it->lid);
        if(!needRemove)
            continue;

        size_t bodyStart = it->bodyStart;
        size_t bodyEnd = it->bodyEnd;

        remove_field(text, bodyStart, bodyEnd, "stations");
        bodyEnd = find_entry_end(text, bodyStart);
        remove_field(text, bodyStart, bodyEnd, "hasTimetable");
        bodyEnd = find_entry_end(text, bodyStart);
        remove_field(text, bodyStart, bodyEnd, "frequency");

        auto innerIt = innerBlocks.find(it->lid);
        if(innerIt != innerBlocks.end())
        {
            std::string inner = innerIt->second;
            size_t at = bodyStart;
            if(at < text.size() && text[at] == '\n')
                ++at;
            else
                inner = "\n" + inner;
            text.insert(at, inner);
        }
    }

    // 存档中不存在的线路，插入新条目
    {
        std::string newBlocks;
        for(auto &[lid, block] : fullBlocks)
        {
            bool exists = false;
            for(auto &e : entries)
                if(e.lid == lid)
                {
                    exists = true;
                    break;
                }
            if(exists)
                continue;
            std::string b = block;
            if(quotedKeys)
            {
                size_t kb = b.find('[');
                size_t ke = kb == std::string::npos ? std::string::npos : b.find(']', kb);
                if(kb != std::string::npos && ke != std::string::npos && ke > kb)
                    b.replace(kb, ke - kb + 1, "[\"" + b.substr(kb + 1, ke - kb - 1) + "\"]");
            }
            newBlocks += b;
        }
        if(!newBlocks.empty())
        {
            size_t insertPos = blockStart + 1;
            if(insertPos < text.size() && text[insertPos] == '\n')
                ++insertPos;
            else
            {
                text.insert(insertPos, "\n");
                ++insertPos;
            }
            text.insert(insertPos, newBlocks);
        }
    }

    // 3. 生成备份文件名（原文件名_年月日_时分秒.backup）
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = {};

#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream backupName;
    backupName << filename.stem().string()
               << "_" << std::put_time(&tm, "%Y%m%d_%H%M%S")
               << filename.extension().string()
               << ".backup";

    fs::path backupPath = filename.parent_path() / backupName.str();

    // 4. 备份原文件
    fs::copy_file(filename, backupPath, fs::copy_options::overwrite_existing);

    // 5. 保存处理后的内容
    std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
    if (!outFile)
    {
        errortype{errortype::SAVE_FILE_UNSAVE};
        return false;
    }

    outFile << text;
    outFile.close();

    display_info(QObject::tr("成功"), QString(QObject::tr("时刻表已成功导入，已生成备份文件 %1")).arg(backupPath.filename().u8string()));
    return true;
}

/**
 * @brief 在 [start,end) 内查找下一个 "[id] = {" 或 "[\"id\"] = {" 条目
 */
bool find_next_entry(const std::string& text, size_t start, size_t end,
                     int& lid, size_t& entryStart, size_t& bodyStart, size_t& bodyEnd)
{
    size_t i = start;
    while(i < end)
    {
        if(text[i] != '[')
        {
            ++i;
            continue;
        }
        size_t p = i + 1;
        bool quoted = false;
        if(p < end && text[p] == '"')
        {
            quoted = true;
            ++p;
        }
        size_t dStart = p;
        while(p < end && std::isdigit(static_cast<unsigned char>(text[p])))
            ++p;
        if(p == dStart)
        {
            ++i;
            continue;
        }
        if(quoted)
        {
            if(p < end && text[p] == '"')
                ++p;
            else
            {
                ++i;
                continue;
            }
        }
        if(p >= end || text[p] != ']')
        {
            ++i;
            continue;
        }
        size_t e = p + 1;
        while(e < end && std::isspace(static_cast<unsigned char>(text[e])))
            ++e;
        if(e >= end || text[e] != '=')
        {
            ++i;
            continue;
        }
        ++e;
        while(e < end && std::isspace(static_cast<unsigned char>(text[e])))
            ++e;
        if(e >= end || text[e] != '{')
        {
            ++i;
            continue;
        }
        lid = std::stoi(text.substr(dStart, p - dStart - (quoted ? 1 : 0)));
        entryStart = i;
        bodyStart = e + 1;
        int bc = 1;
        size_t b = bodyStart;
        for(; b < end && bc > 0; ++b)
        {
            if(text[b] == '{')
                ++bc;
            else if(text[b] == '}')
                --bc;
        }
        if(bc != 0)
        {
            ++i;
            continue;
        }
        bodyEnd = b - 1;
        return true;
    }
    return false;
}

/**
 * @brief 从 bodyStart（条目 '{' 之后）匹配出条目结束 '}' 的位置
 */
size_t find_entry_end(const std::string& text, size_t bodyStart)
{
    int bc = 1;
    size_t b = bodyStart;
    for(; b < text.size() && bc > 0; ++b)
    {
        if(text[b] == '{')
            ++bc;
        else if(text[b] == '}')
            --bc;
    }
    return (bc == 0) ? b - 1 : std::string::npos;
}

/**
 * @brief 删除 [start,end) 内名为 name 的字段（含整行）
 */
bool remove_field(std::string& text, size_t start, size_t end, const std::string& name)
{
    size_t i = start;
    while(i < end)
    {
        size_t f = text.find(name, i);
        if(f == std::string::npos || f >= end)
            return false;

        char prev = f == 0 ? '\n' : text[f - 1];
        if(prev != '\n' && prev != '{' && prev != ',' && prev != ' ' && prev != '\t')
        {
            i = f + name.size();
            continue;
        }

        size_t e = f + name.size();
        while(e < end && (text[e] == ' ' || text[e] == '\t'))
            ++e;
        if(e >= end || text[e] != '=')
        {
            i = f + name.size();
            continue;
        }
        ++e;
        while(e < end && (text[e] == ' ' || text[e] == '\t'))
            ++e;
        if(e >= end)
            return false;

        size_t fieldEnd;
        if(text[e] == '{')
        {
            int bc = 1;
            size_t b = e + 1;
            for(; b < end && bc > 0; ++b)
            {
                if(text[b] == '{')
                    ++bc;
                else if(text[b] == '}')
                    --bc;
            }
            if(bc != 0)
                return false;
            fieldEnd = b;
        }
        else
        {
            size_t b = e;
            while(b < end && text[b] != ',' && text[b] != '\n')
                ++b;
            fieldEnd = (b < end && text[b] == ',') ? b + 1 : b;
        }
        if(fieldEnd < end && text[fieldEnd] == ',')
            ++fieldEnd;
        while(fieldEnd < end && (text[fieldEnd] == ' ' || text[fieldEnd] == '\t'))
            ++fieldEnd;
        if(fieldEnd < end && text[fieldEnd] == '\n')
            ++fieldEnd;

        size_t lineStart = f;
        while(lineStart > start && text[lineStart - 1] != '\n')
            --lineStart;
        if(lineStart == start)
            lineStart = f;

        text.erase(lineStart, fieldEnd - lineStart);
        return true;
    }
    return false;
}

std::vector<CSVData> readCSV(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    std::vector<CSVData> result;

    if (!file) return result;

    // 检查BOM
    unsigned char bom[3];
    file.read((char*)bom, 3);
    if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)) {
        file.seekg(0);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> cols;
        size_t start = 0, end;

        // 分割逗号
        while ((end = line.find(',', start)) != std::string::npos) {
            cols.push_back(line.substr(start, end - start));
            start = end + 1;
        }
        cols.push_back(line.substr(start));

        // 确保有足够列
        if (cols.size() < 4) continue;

        // 获取第2、3、4列（索引1,2,3）
        std::string& col2 = cols[1];
        std::string& col3 = cols[2];
        std::string& col4 = cols[3];

        // 去除空格
        auto trim = [](std::string s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                            [](unsigned char ch) { return !std::isspace(ch); }));
            s.erase(std::find_if(s.rbegin(), s.rend(),
                                 [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
            return s;
        };

        col2 = trim(col2);
        col3 = trim(col3);
        col4 = trim(col4);

        // 检查是否全为空
        if (col2.empty() && col3.empty() && col4.empty()) {
            continue;
        }

        result.push_back({col2, col3, col4});
    }

    return result;
}

TimeComponents parseCSVTime(const std::string& timeStr) {
    TimeComponents tc;

    // 去除空格
    std::string str = timeStr;
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);

    if (str.empty()) return tc;

    // 替换所有全角冒号为半角
    std::string fullColon = "：";
    std::string semiColon = ":";
    size_t pos = 0;
    while ((pos = str.find(fullColon, pos)) != std::string::npos) {
        str.replace(pos, fullColon.length(), semiColon);
        pos += semiColon.length();
    }


    std::stringstream ss(str);
    char delimiter;

    // 尝试读取 时:分:秒
    if (ss >> tc.hours >> delimiter >> tc.minutes >> delimiter >> tc.seconds) {
        return tc;
    }

    // 如果失败，重置重试 时:分
    ss.clear();
    ss.str(str);
    tc.seconds = 0;
    if (ss >> tc.hours >> delimiter >> tc.minutes) {
        return tc;
    }

    // 解析失败，返回全0
    return {-1, -1, -1};
}

std::pair<int, int> read_xlsx_time(QVariant value)
{
    if (value.isNull()) return {-1, -1};

    QTime time;

    // Qt6 使用 typeId()
    if (value.typeId() == QMetaType::QDateTime) {
        time = value.toDateTime().time();
    }
    else if(value.typeId() == QMetaType::QTime)
    {
        time = value.toTime();
    }
    // Qt6 使用 canConvert<T>()
    else if (value.canConvert<double>()) {
        double excelTime = value.toDouble();
        int totalSeconds = excelTime * 24 * 3600;
        time = QTime(0, 0).addSecs(totalSeconds);
    }

    if (!time.isValid()) return {-1, -1};

    return {time.minute(), time.second()};

}


arrdeptime operator-(arrdeptime a, arrdeptime b)
{
    arrdeptime re;
    if(a.arrmin > b.arrmin)
    {
        if(a.arrsec >= b.arrsec)
        {
            re.arrmin = a.arrmin - b.arrmin;
            re.arrsec = a.arrsec - b.arrsec;
        }
        else
        {
            re.arrmin = a.arrmin - b.arrmin - 1;
            re.arrsec = a.arrsec - b.arrsec + 60;
        }
    }
    else if(a.arrmin < b.arrmin)
    {
        if(a.arrsec <= b.arrsec)
        {
            re.arrmin = b.arrmin - a.arrmin;
            re.arrsec = b.arrsec - a.arrsec;
        }
        else
        {
            re.arrmin = b.arrmin - a.arrmin - 1;
            re.arrsec = b.arrsec - a.arrsec + 60;
        }
    }
    else
    {
        re.arrmin = 0;
        re.arrsec = abs(a.arrsec- b.arrsec);
    }
    
    if(a.depmin > b.depmin)
    {
        if(a.depsec >= b.depsec)
        {
            re.depmin = a.depmin - b.depmin;
            re.depsec = a.depsec - b.depsec;
        }
        else
        {
            re.depmin = a.depmin - b.depmin - 1;
            re.depsec = a.depsec - b.depsec + 60;
        }
    }
    else if(a.depmin < b.depmin)
    {
        if(a.depsec <= b.depsec)
        {
            re.depmin = b.depmin - a.depmin;
            re.depsec = b.depsec - a.depsec;
        }
        else
        {
            re.depmin = b.depmin - a.depmin - 1;
            re.depsec = b.depsec - a.depsec + 60;
        }
    }
    else
    {
        re.depmin = 0;
        re.depsec = abs(a.depsec- b.depsec);
    }
    return re;
}



bool printq(const QString& prefix, const QString& content, const QString& suffix)
{

    QDialog dialog;
    dialog.setWindowTitle(QObject::tr("确认"));

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // 上面的文本
    mainLayout->addWidget(new QLabel(prefix));

    // 中间的长文本区域
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setPlainText(content);
    textEdit->setReadOnly(true);
    mainLayout->addWidget(textEdit);

    // 下面的文本
    mainLayout->addWidget(new QLabel(suffix));

    // 按钮行
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton(QObject::tr("确定"));
    QPushButton *cancelButton = new QPushButton(QObject::tr("取消"));

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    bool confirmed = false;

    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        confirmed = true;
        dialog.accept();
    });

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    return (dialog.exec() == QDialog::Accepted);
}


errortype::errortype(int type, QString q)
{
    QString output;
    switch(type)
    {

        case LINE_EMPTY :
            output = QString(QObject::tr("未发现线路id数据"));
            break;

        case STATION_EMPTY :
            output = QString(QObject::tr("未发现站点id数据"));
            break;

        case LISTMODE_NOLIST:
            output = QString(QObject::tr("列表模式未发现列表"));
            break;

        case LIST_LINE_DONT_EXIST :
            output = QString(QObject::tr("列表模式下，线路%1不存在").arg(q));
            break;

        case LIST_FILE_DONT_EXIST :
            output = QString(QObject::tr("列表模式下，文件%1不存在").arg(q));
            break;

        case LIST_SHEET_DONT_EXIST:
            output = QString(QObject::tr("列表模式下，%1表单不存在").arg(q));
            break;

        case NO_TIMETABLE:
            output = QString(QObject::tr("列表模式下无时刻表或简单模式未检测到任何时刻表"));
            break;

        case STATION_DONT_EXIST:
            output = QString(QObject::tr("站点%1不存在").arg(q));
            break;

        case NO_LINE_STAT:
            output = QString(QObject::tr("表单%1内无数据").arg(q));
            break;

        case STATION_MISMATCH:
            output = QString(QObject::tr("线路%1内存在多个时刻表的站点不对应").arg(q));
            break;

        case TIME_INVALID:
            output = QString(QObject::tr("文件%1内含有无效时间数据").arg(q));
            break;

        case NO_SHEET_IN_FILE:
            output = QString(QObject::tr("文件%1内无表单").arg(q));
            break;

        case SAVE_FILE_UNOPEN:
            output = QString(QObject::tr("存档文件无法打开，请检查权限等"));
            break;

        case SAVE_FILE_UNSAVE:
            output = QString(QObject::tr("存档文件无法保存，请检查权限或者是否在别的应用打开等"));
            break;

        case NO_TIMETABLE_MOD:
            output = QString(QObject::tr("未安装时刻表mod"));
            break;

        case MULTI_STATION:
            output = QString(QObject::tr("时刻表内的%1站点存在重名").arg(q));
            break;

        case MULTI_LINE:
            output = QString(QObject::tr("时刻表内的%1线路存在重名").arg(q));
            break;

        default:
            output = QObject::tr("其他");
            break;
    }

        display_info(QObject::tr("错误"), output);
}


void refresh_file(const my_data &sdata)
{
    std::ofstream file(sys_file_name);

    file << sdata.folder_dir.u8string()<< '\n';
    file << sdata.sg_dir.u8string() << '\n';
    file << sdata.sys_save_dir.u8string() << '\n';
    file << sdata.trunc<< '\n';
    file << sdata.trunc_if << '\n';
    file << sdata.easy_if << '\n';
    file << sdata.xls_if << '\n';
    file << sdata.invalid_if << '\n';
    file << sdata.clear_if << '\n';
    file << sdata.pile_if << '\n';
    file << sdata.d_station_add << '\n';
    file << sdata.d_line_add << '\n';

    file.close();
}


void read_id_data(const std::filesystem::path& filePath,
                  std::vector<std::pair<std::string, int>>& data,
                  IDtype type)
{
    std::ifstream file(filePath);

    std::string match = (type == IDtype::STATION ? "stations = {" : "lines = {");
    std::string line;
    bool inModSection = false;
    bool in_area = false;

    while (std::getline(file, line)) {
        // 1. 查找 ["your_mod.lua"] 行
        if (!inModSection && line.find("[\"timetable_idget.lua\"]") != std::string::npos) {
            inModSection = true;
            continue;
        }

        // 2. 如果不在我们的mod部分，跳过
        if (!inModSection) continue;

        // 3. 检查是否进入stations或lines部分
        if (line.find(match) != std::string::npos) {
            in_area = true;
            continue;
        }

        // 4. 检查是否结束当前部分
        if (line.find("}") != std::string::npos) {
            if (!in_area) {
                continue;
            } else {
                // 结束整个mod部分
                break;
            }
        }

        // 5. 解析数据行（格式固定为: [数字] = "名称",）
        if (in_area) {
            // 格式示例: [1001] = "中央车站",
            size_t bracketStart = line.find('[');
            size_t bracketEnd = line.find(']');
            size_t quoteStart = line.find('"');
            size_t quoteEnd = line.find('"', quoteStart + 1);

            if (bracketStart != std::string::npos && bracketEnd != std::string::npos &&
                quoteStart != std::string::npos && quoteEnd != std::string::npos) {

                // 提取ID
                std::string idStr = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                int id = std::stoi(idStr);

                // 提取名称
                std::string name = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);

                data.push_back({name, id});
            }
        }
    }


    file.close();
}

