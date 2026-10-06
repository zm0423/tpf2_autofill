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
#include <cstdlib>


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
                     int& lid, size_t& entryStart, size_t& bodyStart, size_t& bodyEnd,
                     bool& quotedKey);
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
                  int clear_if,
                  bool quotedKeys)
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

    // 扫描块内所有线路条目（只读）
    struct Entry { size_t entryStart; size_t bodyStart; size_t bodyEnd; int lid; bool quotedKey; };
    std::vector<Entry> entries;
    {
        size_t scan = blockStart + 1;
        int lid;
        size_t es, bs, be;
        bool quoted;
        while(find_next_entry(text, scan, blockEnd, lid, es, bs, be, quoted))
        {
            entries.push_back({es, bs, be, lid, quoted});
            scan = be + 1;
        }
    }

    // 从后往前处理：键格式迁移 + 删除/替换受管字段，其余内容保留
    for(auto it = entries.rbegin(); it != entries.rend(); ++it)
    {
        size_t bodyStart = it->bodyStart;
        size_t bodyEnd = it->bodyEnd;

        // 键格式与所选版本不一致时，迁移键格式
        if(it->quotedKey != quotedKeys)
        {
            size_t kb = it->entryStart;
            size_t ke = text.find(']', kb);
            if(ke != std::string::npos)
            {
                ++ke; // 指向 ']' 之后
                std::string newKey = quotedKeys ?
                            "[\"" + std::to_string(it->lid) + "\"]" :
                            "[" + std::to_string(it->lid) + "]";
                long long delta = static_cast<long long>(newKey.size()) -
                                  static_cast<long long>(ke - kb);
                text.replace(kb, ke - kb, newKey);
                bodyStart = static_cast<size_t>(static_cast<long long>(bodyStart) + delta);
                bodyEnd = static_cast<size_t>(static_cast<long long>(bodyEnd) + delta);
            }
        }

        bool needRemove = clearAll || toClear.count(it->lid) || toReplace.count(it->lid);
        if(!needRemove)
            continue;

        remove_field(text, bodyStart, bodyEnd, "stations");
        bodyEnd = find_entry_end(text, bodyStart);
        remove_field(text, bodyStart, bodyEnd, "hasTimetable");
        bodyEnd = find_entry_end(text, bodyStart);
        remove_field(text, bodyStart, bodyEnd, "frequency");
        bodyEnd = find_entry_end(text, bodyStart);

        auto innerIt = innerBlocks.find(it->lid);
        if(innerIt != innerBlocks.end())
        {
            std::string inner = innerIt->second;
            // 插到条目末尾（'}' 前一行行首），使受管字段沉底、其余字段在上
            size_t at = bodyEnd;
            while(at > bodyStart && text[at - 1] != '\n')
                --at;
            if(at == bodyStart)
                inner = "\n" + inner;
            text.insert(at, inner);
        }
        else
        {
            // 只清空、没有新数据：写入“空时刻表”标记，而不是把条目删成空表。
            // 1.3-1.5 的代码只要条目存在就会直接索引/迭代表里的 .stations（pairs/#），
            // 缺字段会在游戏内报 Lua 错误、表现为“清空不生效”；
            // hasTimetable = false + stations = { } 与游戏内手动清空（取消勾选）后的结构一致。
            // 其它版本（如“时刻表&运行图”）对空表有容错，不受影响。
            std::string inner = "\t\t\t\thasTimetable = false,\n\t\t\t\tstations = { },\n";
            size_t at = bodyEnd;
            while(at > bodyStart && text[at - 1] != '\n')
                --at;
            if(at == bodyStart)
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
                     int& lid, size_t& entryStart, size_t& bodyStart, size_t& bodyEnd,
                     bool& quotedKey)
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
        quotedKey = quoted;
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

std::pair<int, int> read_xlsx_time(QVariant value, bool total_minutes)
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

    if(total_minutes)
        return {time.hour() * 60 + time.minute(), time.second()};

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

        case TIME_OVER_CYCLE:
            output = QString(QObject::tr("时刻表超出所选周期范围：%1").arg(q));
            break;

        case EXPORT_MISMATCH:
            output = QString(QObject::tr("与游戏存档数据不一致：%1\n"
                                         "可能存档已变动（增删或改动了线路、站点），或站点、线路数据未刷新。\n"
                                         "请重新执行“站点、线路数据导入”后再试，必要时在游戏中核对线路站点").arg(q));
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
    file << sdata.d_clear2_warning << '\n';
    file << sdata.d_version << '\n';
    file << sdata.cycle_index << '\n';
    file << sdata.steam_user << '\n';
    file << sdata.d_tpf3_notice << '\n';
    file << sdata.tpf2_version << '\n';
    file << sdata.tpf2_sg_dir.u8string() << '\n';

    file.close();
}


bool find_tpf2_save(const std::filesystem::path& folder,
                    const std::string& preferName,
                    std::filesystem::path& outPath)
{
    outPath.clear();
    std::error_code ec;

    // 1) 优先按原存档名（同名 .lua）
    if(!preferName.empty())
    {
        fs::path cand = folder / fs::u8path(preferName + ".lua");
        if(fs::exists(cand, ec))
        {
            outPath = cand;
            return true;
        }
    }

    // 2) 目录里唯一一个带 _station/_line 配套文件的 .lua；退而求其次：唯一一个 .lua
    std::vector<fs::path> luaFiles;
    std::vector<fs::path> withCompanion;
    fs::directory_iterator it(folder, ec), end;
    if(ec)
        return false;
    for(; it != end; it.increment(ec))
    {
        if(ec)
            break;
        std::error_code ec2;
        if(!it->is_regular_file(ec2) || ec2)
            continue;
        fs::path p = it->path();
        if(p.extension() != fs::u8path(".lua"))
            continue;
        luaFiles.push_back(p);
        std::string stem = p.stem().u8string();
        if(fs::exists(folder / fs::u8path(stem + "_station.xlsx"), ec2) ||
           fs::exists(folder / fs::u8path(stem + "_line.xlsx"), ec2))
            withCompanion.push_back(p);
    }

    if(withCompanion.size() == 1)
        outPath = withCompanion.front();
    else if(withCompanion.empty() && luaFiles.size() == 1)
        outPath = luaFiles.front();
    return !outPath.empty();
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


// ========== 狂热运输3（TPF3 桥接） ==========

int cycle_sec_from_index(int index)
{
    static const int presets[6] = {3600, 7200, 10800, 21600, 43200, 86400};
    if(index < 0 || index > 5)
        return presets[0];
    return presets[index];
}

QString cycle_label_from_sec(int sec)
{
    if(sec <= 0)
        return QString();
    return QObject::tr("%1 小时").arg(sec / 3600);
}

bool read_export_file(const std::filesystem::path& file, export_info& out)
{
    out = {};

    std::ifstream in(file, std::ios::binary);
    if(!in)
        return false;

    enum class Sec { NONE, STATIONS, LINES, LINE_STOPS };
    Sec sec = Sec::NONE;

    std::string line;
    while(std::getline(in, line))
    {
        if(line.find("lineStops = {") != std::string::npos)
        {
            sec = Sec::LINE_STOPS;
            continue;
        }
        if(line.find("stations = {") != std::string::npos)
        {
            sec = Sec::STATIONS;
            continue;
        }
        if(line.find("lines = {") != std::string::npos)
        {
            sec = Sec::LINES;
            continue;
        }
        if(sec == Sec::NONE && line.find("cycleSec") != std::string::npos)
        {
            size_t eq = line.find('=');
            if(eq != std::string::npos)
                out.cycle_sec = std::atoi(line.substr(eq + 1).c_str());
            continue;
        }
        if(sec == Sec::NONE && line.find("saveId") != std::string::npos)
        {
            size_t q1 = line.find('"');
            if(q1 != std::string::npos)
            {
                std::string value;
                for(size_t p = q1 + 1; p < line.size(); ++p)
                {
                    if(line[p] == '\\' && p + 1 < line.size())
                    {
                        value += line[p + 1];
                        ++p;
                        continue;
                    }
                    if(line[p] == '"')
                        break;
                    value += line[p];
                }
                out.save_id = value;
            }
            continue;
        }
        if(line.find('}') != std::string::npos)
        {
            sec = Sec::NONE;
            continue;
        }
        if(sec == Sec::NONE)
            continue;

        // 条目格式：["12345"] = "名称",（线路站序为 "id1,id2,..."）
        size_t lb = line.find('[');
        if(lb == std::string::npos)
            continue;
        size_t rb = line.find(']', lb);
        if(rb == std::string::npos)
            continue;
        size_t eq = line.find('=', rb);
        if(eq == std::string::npos)
            continue;

        std::string idStr = line.substr(lb + 1, rb - lb - 1);
        idStr.erase(std::remove(idStr.begin(), idStr.end(), '"'), idStr.end());
        idStr.erase(std::remove(idStr.begin(), idStr.end(), ' '), idStr.end());
        if(idStr.empty())
            continue;
        int id = std::atoi(idStr.c_str());

        size_t q1 = line.find('"', eq);
        if(q1 == std::string::npos)
            continue;

        std::string value;
        for(size_t p = q1 + 1; p < line.size(); ++p)
        {
            if(line[p] == '\\' && p + 1 < line.size())
            {
                value += line[p + 1];
                ++p;
                continue;
            }
            if(line[p] == '"')
                break;
            value += line[p];
        }

        if(sec == Sec::STATIONS)
            out.stations.push_back({value, id});
        else if(sec == Sec::LINES)
            out.lines.push_back({value, id});
        else if(sec == Sec::LINE_STOPS)
        {
            std::vector<int> stops;
            std::stringstream ss(value);
            std::string tok;
            while(std::getline(ss, tok, ','))
                if(!tok.empty())
                    stops.push_back(std::atoi(tok.c_str()));
            out.line_stops[id] = std::move(stops);
        }
    }

    return true;
}

namespace
{
    std::string lua_escape(const std::string& s)
    {
        std::string o;
        o.reserve(s.size() + 8);
        for(char c : s)
        {
            switch(c)
            {
            case '\\': o += "\\\\"; break;
            case '"':  o += "\\\""; break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            default:   o += c;      break;
            }
        }
        return o;
    }

    // arrmin/arrsec → "H:MM:SS"（TPF3 下 arrmin 为周期内总分钟）
    QString fmt_cycle_time(int min, int sec)
    {
        return QString("%1:%2:%3")
            .arg(min / 60)
            .arg(min % 60, 2, 10, QChar('0'))
            .arg(sec, 2, 10, QChar('0'));
    }
}

bool write_probe_file(const std::filesystem::path& dir, const std::string& token)
{
    std::ofstream out(dir / fs::path("probe.lua"), std::ios::binary | std::ios::trunc);
    if(!out)
        return false;
    out << "function data()\nreturn {\n\ttoken = \"" << lua_escape(token) << "\",\n}\nend\n";
    out.close();
    return true;
}

bool read_probe_result(const std::filesystem::path& file, std::string& token,
                       std::string& save_id, int& cycle_sec)
{
    token.clear();
    save_id.clear();
    cycle_sec = 0;

    std::ifstream in(file, std::ios::binary);
    if(!in)
        return false;

    auto read_quoted = [](const std::string& line, std::string& value) {
        size_t q1 = line.find('"');
        if(q1 == std::string::npos)
            return;
        for(size_t p = q1 + 1; p < line.size(); ++p)
        {
            if(line[p] == '\\' && p + 1 < line.size())
            {
                value += line[p + 1];
                ++p;
                continue;
            }
            if(line[p] == '"')
                break;
            value += line[p];
        }
    };

    std::string line;
    while(std::getline(in, line))
    {
        size_t eq = line.find('=');
        if(eq == std::string::npos)
            continue;
        if(line.find("token") != std::string::npos)
            read_quoted(line, token);
        else if(line.find("saveId") != std::string::npos)
            read_quoted(line, save_id);
        else if(line.find("cycleSec") != std::string::npos)
            cycle_sec = std::atoi(line.substr(eq + 1).c_str());
    }
    return !token.empty();
}

bool write_data_lua(const my_data& sdata,
                    const std::vector<std::pair<int, std::vector<stationinfo>>>& data)
{
    const fs::path exportFile = sdata.sg_dir / fs::path("export.lua");

    export_info info;
    if(!read_export_file(exportFile, info))
    {
        display_info(QObject::tr("错误"),
                     QObject::tr("未能读取游戏数据文件 %1\n请先启动一次游戏，让桥接 mod 导出数据").arg(stq(exportFile.u8string())));
        return false;
    }

    const int cycleSec = cycle_sec_from_index(sdata.cycle_index);

    // 线路全名：以 export.lua 里的游戏内名称为准（截断等操作只影响 CSV 匹配，
    // 不能影响写入 data.lua 的名字，否则桥接按名匹配会失败）
    std::unordered_map<int, std::string> nameById;
    for(const auto& p : info.lines)
        nameById[p.second] = p.first;
    auto fullLineName = [&](int id) -> QString {
        auto it = nameById.find(id);
        if(it != nameById.end())
            return stq(it->second);
        return get_linename(sdata.line, id);
    };

    // 站序校验 + 周期越界校验
    for(const auto& linePair : data)
    {
        const int lineid = linePair.first;
        const auto& stations = linePair.second;

        auto it = info.line_stops.find(lineid);
        if(it == info.line_stops.end())
        {
            errortype e{errortype::EXPORT_MISMATCH,
                        QObject::tr("线路%1 在存档中不存在").arg(fullLineName(lineid))};
            return false;
        }
        if(it->second.size() != stations.size())
        {
            errortype e{errortype::EXPORT_MISMATCH,
                        QObject::tr("线路%1 的站序与存档不一致").arg(fullLineName(lineid))};
            return false;
        }
        for(size_t i = 0; i < stations.size(); ++i)
            if(stations[i].stationid != it->second[i])
            {
                errortype e{errortype::EXPORT_MISMATCH,
                            QObject::tr("线路%1 的站序与存档不一致").arg(fullLineName(lineid))};
                return false;
            }

        for(const auto& st : stations)
            for(const auto& t : st.arrdep)
            {
                bool badArr = t.arrmin < 0 || t.arrsec < 0 || t.arrmin * 60 + t.arrsec >= cycleSec;
                bool badDep = t.depmin < 0 || t.depsec < 0 || t.depmin * 60 + t.depsec >= cycleSec;

                if(badArr || badDep)
                {
                    std::string station_name;
                    for(const auto& p : sdata.station)
                        if(p.second == st.stationid)
                        {
                            station_name = p.first;
                            break;
                        }

                    QString which = badDep
                        ? QObject::tr("出发 %1").arg(fmt_cycle_time(t.depmin, t.depsec))
                        : QObject::tr("到达 %1").arg(fmt_cycle_time(t.arrmin, t.arrsec));

                    errortype e{errortype::TIME_OVER_CYCLE,
                                QObject::tr("线路%1 站点%2（%3，周期上限 %4）")
                                    .arg(fullLineName(lineid))
                                    .arg(stq(station_name))
                                    .arg(which)
                                    .arg(cycle_label_from_sec(cycleSec))};
                    return false;
                }
            }
    }

    // 生成 data.lua
    const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();

    std::ostringstream oss;
    oss << "function data()\nreturn {\n";
    oss << "\trevision = " << nowMs << ",\n";
    oss << "\tforce = false,\n";
    oss << "\tcycleSec = " << cycleSec << ",\n";
    if(sdata.clear_if == 3)
    {
        oss << "\tclearAll = true,\n";
    }
    else if(sdata.clear_if == 2)
    {
        oss << "\tclearLines = {\n";
        for(const auto& p : sdata.line)
            oss << "\t\t\"" << lua_escape(fullLineName(p.second).toUtf8().toStdString()) << "\",\n";
        oss << "\t},\n";
    }
    oss << "\tlines = {\n";
    for(const auto& linePair : data)
    {
        const int lineid = linePair.first;
        const auto& stations = linePair.second;

        oss << "\t\t{\n";
        oss << "\t\t\tname = \"" << lua_escape(fullLineName(lineid).toUtf8().toStdString()) << "\",\n";
        oss << "\t\t\tstops = {\n";
        for(size_t i = 0; i < stations.size(); ++i)
        {
            oss << "\t\t\t\t{ index = " << (i + 1) << ", slots = {\n";
            for(const auto& t : stations[i].arrdep)
                oss << "\t\t\t\t\t{ arrMin = " << t.arrmin << ", arrSec = " << t.arrsec
                    << ", depMin = " << t.depmin << ", depSec = " << t.depsec << " },\n";
            oss << "\t\t\t\t} },\n";
        }
        oss << "\t\t\t},\n";
        oss << "\t\t},\n";
    }
    oss << "\t},\n}\nend\n";

    const fs::path outFile = sdata.sg_dir / fs::path("data.lua");
    std::ofstream out(outFile, std::ios::binary | std::ios::trunc);
    if(!out)
    {
        errortype{errortype::SAVE_FILE_UNSAVE};
        return false;
    }
    out << oss.str();
    out.close();

    display_info(QObject::tr("成功"),
                 QObject::tr("时刻表数据已生成：\n%1\n\n进入游戏后打开 AutoFill 窗口，点击“将导入数据应用至时刻表”即可生效。")
                     .arg(stq(outFile.u8string())));
    return true;
}

