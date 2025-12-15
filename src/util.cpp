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



bool removeStructure(std::string& text, size_t start, size_t& end);
void processRemoveSelected(std::string& text, size_t start, size_t end,
                           const std::unordered_set<int>& numbersToRemove);
void processRemoveAll(std::string& text, size_t start, size_t end);




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
        result.push_back(std::make_pair(a.toString().trimmed().toUtf8().toStdString(), b.toInt(&v)));
        if(!v)
        {
            display_info("错误","站点或线路文件第二列含有非数字，请修改后重新打开软件");
            result.clear();
            return;
        }
    }
    return;
}

void readXlsx(const std::filesystem::path& filename, std::unordered_map<std::string, int> &result)
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
        result.insert(std::make_pair(a.toString().trimmed().toUtf8().toStdString(), b.toInt(&v)));
        if(!v)
        {
            display_info("错误","站点或线路文件第二列含有非数字，请修改后重新打开软件");
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
    songTi20.setFontName("宋体");
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
        display_info("错误","文件存储失败，请检查文件夹权限、是否被其他应用打开、或磁盘空间");
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

bool text_to_vector(const std::string& input, std::vector<std::pair<std::string, int>> &output)
{
    std::stringstream ss(input);
    std::string line;
    bool isnum = false;
    std::pair<std::string, int> temp;
    if(input.empty())
        return 0;
    while(std::getline(ss, line,'\n'))
    {
        if(line.empty())
            continue;
        if(isnum == false)
            temp.first = line;
        else
        {
            try{
            temp.second = std::stoi(line);
            }catch(std::invalid_argument& e)
            {
                display_info("错误","站点或线路输入信息的某一组内的第二行含有非数字");
                output.clear();
                output.shrink_to_fit();
                return 0;
            }

            output.push_back(temp);
        }
        isnum = !isnum;
    }
    return 1;
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
    QPushButton *yesButton = msgBox.addButton("确定", QMessageBox::AcceptRole);
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

    std::unordered_set<int> numbersToRemove;
    if(clear_if == 1)
        for(auto &p:id)
            numbersToRemove.emplace(p.first);
    else if (clear_if == 2)
        for(auto &p:line)
            numbersToRemove.emplace(p.second);



    size_t insertPos = std::string::npos;

    // 查找 timetable 块
    size_t pos = text.find("timetable = {");
    if (pos == std::string::npos) {
        errortype{errortype::NO_TIMETABLE_MOD};
        return false;
    }


    size_t bracePos = pos + 12;  // "timetable = " 长度

    if (bracePos + 1 < text.size() && text[bracePos + 1] == '\n') {
        insertPos = bracePos + 2;
    } else {
        text.insert(bracePos + 1, "\n");
        insertPos = bracePos + 2;
    }

    // 查找 timetable 块的结束
    int braceCount = 1;
    size_t end = pos + 13; // "timetable = {" 长度

    for (size_t i = end; i < text.size() && braceCount > 0; ++i) {
        if (text[i] == '{') braceCount++;
        else if (text[i] == '}') braceCount--;

        if (braceCount == 0) {
            end = i;
            break;
        }
    }

    // 根据模式处理
    if (clear_if == 3) {
        processRemoveAll(text, pos, end);
    } else {
        processRemoveSelected(text, pos, end, numbersToRemove);
    }

    // 重新计算插入位置
    if (insertPos != std::string::npos) {
        size_t newPos = text.find("timetable = {");
        if (newPos != std::string::npos) {
            insertPos = text.find('\n', newPos);
            if (insertPos != std::string::npos) {
                insertPos++;
            }
        }
    }

    text.insert(insertPos, data);


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

    display_info("成功", QString("时刻表已成功导入，已生成备份文件 %1").arg(backupPath.filename().u8string()));
    return true;
}

/**
 * @brief 处理删除所有结构
 */
void processRemoveAll(std::string& text, size_t start, size_t end) {
    size_t i = start;

    while (i < end) {
        // 检查是否匹配 [数字] = { 模式
        if (text[i] == '[') {
            // 提取数字
            size_t numStart = i + 1;
            size_t numEnd = numStart;

            // 跳过空白找数字开始
            while (numEnd < end && std::isspace(text[numEnd])) {
                numEnd++;
            }

            size_t digitStart = numEnd;
            while (numEnd < end && std::isdigit(text[numEnd])) {
                numEnd++;
            }

            // 检查是否有数字
            if (digitStart < numEnd) {
                // 跳过空白找 ]
                while (numEnd < end && std::isspace(text[numEnd])) {
                    numEnd++;
                }

                if (numEnd < end && text[numEnd] == ']') {
                    size_t closeBracket = numEnd;

                    // 跳过空白找 =
                    size_t equalsPos = closeBracket + 1;
                    while (equalsPos < end && std::isspace(text[equalsPos])) {
                        equalsPos++;
                    }

                    if (equalsPos < end && text[equalsPos] == '=') {
                        // 跳过空白找 {
                        size_t bracePos = equalsPos + 1;
                        while (bracePos < end && std::isspace(text[bracePos])) {
                            bracePos++;
                        }

                        if (bracePos < end && text[bracePos] == '{') {
                            // 找到完整结构，删除它
                            if (removeStructure(text, i, end)) {
                                // 删除成功，继续从当前位置
                                continue;
                            }
                        }
                    }
                }
            }
        }
        i++;
    }
}

/**
 * @brief 处理删除选定结构
 */
void processRemoveSelected(std::string& text, size_t start, size_t end,
                           const std::unordered_set<int>& numbersToRemove) {

    // 为快速查找构建字符串集合
    std::unordered_set<std::string> patterns;
    for (int num : numbersToRemove) {
        patterns.insert("[" + std::to_string(num) + "]");
    }

    size_t i = start;

    while (i < end) {
        if (text[i] == '[') {
            // 查找 ] 的位置
            size_t closeBracket = text.find(']', i);
            if (closeBracket == std::string::npos || closeBracket >= end) {
                i++;
                continue;
            }

            // 提取括号内的内容
            std::string bracketContent = text.substr(i, closeBracket - i + 1);

            // 检查是否匹配要删除的模式
            bool shouldRemove = false;
            for (const auto& pattern : patterns) {
                if (bracketContent.find(pattern) != std::string::npos) {
                    shouldRemove = true;
                    break;
                }
            }

            if (shouldRemove) {
                // 检查是否是 [数字] = { 完整结构
                size_t equalsPos = closeBracket + 1;
                while (equalsPos < end && std::isspace(text[equalsPos])) {
                    equalsPos++;
                }

                if (equalsPos < end && text[equalsPos] == '=') {
                    size_t bracePos = equalsPos + 1;
                    while (bracePos < end && std::isspace(text[bracePos])) {
                        bracePos++;
                    }

                    if (bracePos < end && text[bracePos] == '{') {
                        // 找到完整结构，删除它
                        if (removeStructure(text, i, end)) {
                            // 删除成功，继续从当前位置
                            continue;
                        }
                    }
                }
            }
        }
        i++;
    }
}

/**
 * @brief 删除一个完整结构
 * @return 是否成功删除
 */
bool removeStructure(std::string& text, size_t start, size_t& end) {
    // 找到匹配的右大括号

    size_t deleteStart = start;
    while (deleteStart > 0 && text[deleteStart - 1] == '\t') {
        deleteStart--;
    }

    int braceCount = 0;
    size_t pos = start;

    // 先找到第一个 {
    while (pos < text.size() && text[pos] != '{') {
        pos++;
    }

    if (pos >= text.size() || text[pos] != '{') {
        return false;
    }

    braceCount = 1;
    size_t structEnd = pos + 1;

    while (structEnd < text.size() && braceCount > 0) {
        if (text[structEnd] == '{') {
            braceCount++;
        } else if (text[structEnd] == '}') {
            braceCount--;
        }
        structEnd++;
    }

    if (braceCount == 0) {
        // 检查并包含后面的逗号
        size_t afterBrace = structEnd;
        while (afterBrace < text.size() &&
               (text[afterBrace] == ' ' || text[afterBrace] == '\t' ||
                text[afterBrace] == '\n' || text[afterBrace] == '\r')) {
            afterBrace++;
        }

        if (afterBrace < text.size() && text[afterBrace] == ',')
            structEnd = afterBrace + 1;

        if (structEnd < text.size() && text[structEnd] == '\n')
            structEnd += 1;


        // 删除整个结构
        size_t length = structEnd - deleteStart;
        text.erase(deleteStart, length);

        // 更新结束位置
        end -= length;

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



bool printq(const QString& q, const QString& p)
{

    QDialog dialog;
    dialog.setWindowTitle("确认");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // 上面的文本
    mainLayout->addWidget(new QLabel("时刻表表单数据如下："));

    // 中间的长文本区域
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setPlainText(q);
    textEdit->setReadOnly(true);
    mainLayout->addWidget(textEdit);

    // 下面的文本
    mainLayout->addWidget(new QLabel(p));

    // 按钮行
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("确定");
    QPushButton *cancelButton = new QPushButton("取消");

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
            output = QString("未发现线路id数据");
            break;

        case STATION_EMPTY :
            output = QString("未发现站点id数据");
            break;

        case LISTMODE_NOLIST:
            output = QString("列表模式未发现列表");
            break;

        case LIST_LINE_DONT_EXIST :
            output = QString("列表模式下，线路") + q + "不存在";
            break;

        case LIST_FILE_DONT_EXIST :
            output = QString("列表模式下，文件") + q + "不存在";
            break;

        case LIST_SHEET_DONT_EXIST:
            output = QString("列表模式下，") + q + "表单不存在";
            break;

        case NO_TIMETABLE:
            output = QString("列表模式下无时刻表或简单模式未检测到任何时刻表");
            break;

        case STATION_DONT_EXIST:
            output = QString("站点") + q + "对应id不存在";
            break;

        case NO_LINE_STAT:
            output = QString("表单") + q + "内无数据";
            break;

        case STATION_MISMATCH:
            output = QString("线路") + q + "内存在多个时刻表的站点不对应";
            break;

        case TIME_INVALID:
            output = QString("文件") + q + "内含有无效时间数据";
            break;

        case NO_SHEET_IN_FILE:
            output = QString("文件") + q + "内无表单";
            break;

        case SAVE_FILE_UNOPEN:
            output = QString("存档文件无法打开，请检查权限等");
            break;

        case SAVE_FILE_UNSAVE:
            output = QString("存档文件无法保存，请检查权限或者是否在别的应用打开等");
            break;

        case NO_TIMETABLE_MOD:
            output = QString("未安装时刻表mod");
            break;

        default:
            output = "其他";
            break;
    }

        display_info("错误", output);
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

    file.close();
}




