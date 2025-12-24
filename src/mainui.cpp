#include "mainui.h"
#include "ui_mainui.h"
#include "util.h"
#include "data_add.h"

#include <Qtimer>
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


#include "xlsxdocument.h"

#include "MarkdownLanguageManager.h"
#include "simplemarkdown.h"

std::string file_sufix(int i);

namespace fs = std::filesystem;


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
    setWindowTitle(tr("狂热运输2 时刻表自动输入"));

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


    QTimer::singleShot(0, this, &mainui::init);
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

        refresh();
        if(!fs::exists(sdata.folder_dir / (sdata.sg_name + "_list.xlsx")))
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

            doc.saveAs(stq((sdata.folder_dir / (sdata.sg_name + "_list.xlsx")).u8string()));

            QString q = tr("未检测到列表文件，已自动生成") + stq((sdata.folder_dir / (sdata.sg_name + "_list.xlsx")).u8string());
            q += tr("，如采用列表模式请编辑该文件\n格式见文档，每行一个线路，如有更多文件请向后加。"
                 "对于文件中的某些表单，请以空格分隔。"
                 "如果需要一个文件里的所有表单请空置“表单名称”栏目，第一行仅做说明，可随意更改。");
            display_info(tr("提示"), std::move(q));

        }
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


    if(!get_folder())
        return;
    get_sg();
}

mainui::~mainui()
{
    delete ui;
}


void mainui::on_input_data_clicked()
{
    refresh_file(sdata);
    if(sdata.folder_dir.empty())
    {
        display_info(tr("提示"), tr("请先选取工作文件夹"));
        return;
    }
    if(sdata.sg_dir.empty())
    {
        display_info(tr("提示"), tr("请先选取存档"));
        return;
    }
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

    if(!get_list(lists))
        return;

    std::vector<std::pair<int, std::vector<stationinfo>>> data;

    if(!get_data(lists, data))
        return;


    std::string all_data;

    for(auto &[lineid, stationdata]:data)
        all_data += get_filetext(lineid, stationdata);

    write_to_lua(sdata.sg_dir, all_data, lists, sdata.line, sdata.clear_if);

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

    sdata = {};

    sdata.folder_name = folder_name;
    sdata.folder_dir = folder;

    refresh();
    refresh_file(sdata);

    return 1;
}


bool mainui::get_sg()
{

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

    if(!fs::exists(sdata.folder_dir / (sg_name + "_list.xlsx")))
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

        doc.saveAs(stq((sdata.folder_dir / (sdata.sg_name + "_list.xlsx")).u8string()));

        QString q = tr("未检测到列表文件，已自动生成") + stq((sdata.folder_dir / (sdata.sg_name + "_list.xlsx")).u8string());
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
    readXlsx(sdata.folder_dir / (sdata.sg_name + u8"_station.xlsx"), sdata.station);
    readXlsx(sdata.folder_dir / (sdata.sg_name + u8"_line.xlsx"), sdata.line);
}


void mainui::refresh()
{
    ui->dir_name->setText(sdata.folder_name.empty()? tr("无") : QString::fromStdString(sdata.folder_name));
    ui->savegame_name->setText(sdata.sg_name.empty()? tr("无") : QString::fromStdString(sdata.sg_name));
    ui->station_status->setText(sdata.station.empty()? tr("否"):tr("是"));
    ui->line_status->setText(sdata.line.empty()? tr("否"):tr("是"));
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


    if(sdata.easy_if)
    {
        for(auto &p : sdata.line)
        {
            std::vector<std::pair<QString, QString>> vec;

            for(int i = 0;;++i)
            {
                fs::path temppath = sdata.folder_dir / (p.first + file_sufix(i) + prefix);

                QString o = stq(temppath.u8string());

                if(!fs::exists(temppath))
                    break;

                if(!sdata.xls_if)
                {
                    vec.push_back(std::make_pair(stq(temppath.u8string()), ""));
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
                list.push_back(std::make_pair(p.second, vec));
        }
    }
    else
    {
        fs::path file = sdata.folder_dir / (sdata.sg_name + "_list.xlsx");
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

                if(!sdata.xls_if)
                {
                    vec.push_back(std::make_pair(filename, ""));
                    continue;
                }

                QVariant sheetv = doc.read(i, j + 1);

                if(sheetv.isNull() || sheetv.toString().trimmed().isEmpty())
                    vec.push_back(std::make_pair(filename, ""));
                else
                {
                    QStringList sheet = sheetv.toString().trimmed().split(" ", Qt::SkipEmptyParts);
                    for(auto &s:sheet)
                        vec.push_back(std::make_pair(filename, s.trimmed()));
                }
            }
            if(!vec.empty())
                list.push_back(std::make_pair(id, vec));
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
                        if(doc->read(i+1, 1).isNull() || doc->read(i+1, 1).toString().trimmed().isEmpty() ||
                           doc->read(i+1, 3).isNull() || doc->read(i+1, 3).toString().trimmed().isEmpty() ||
                           doc->read(i+1, 4).isNull() || doc->read(i+1, 4).toString().trimmed().isEmpty())
                            break;
                    }
                    else if(sta.isNull() || sta.toString().trimmed().isEmpty() ||
                            arrtime.isNull() || arrtime.toString().trimmed().isEmpty() ||
                            deptime.isNull() || deptime.toString().trimmed().isEmpty() )
                        break;

                    auto it = sdata.station.find(sta.toString().trimmed().toUtf8().toStdString());
                    if(it == sdata.station.end())
                    {
                        errortype e{errortype::STATION_DONT_EXIST, sta.toString().trimmed()};
                        return false;
                    }

                    local_stations.push_back(it->second);

                    auto arr = read_xlsx_time(arrtime);
                    auto dep = read_xlsx_time(deptime);
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


    QString q;

    for (int i = 0;i < output.size();++i)
    {
        q += QString(tr("%1 : %2组数据"))
                 .arg(get_linename(sdata.line, output[i].first))
                 .arg(shrink_num[i]);
        if(shrink_if[i])
            q += tr("     包含相近或重复数据");
        QString last;
        for(auto &p :list[i].second)
        {
            if(last != p.first)
            {
                q += "\n";
                last = p.first;
                q += QFileInfo(p.first).fileName();
                q += ":";
                q += p.second.size() ? p.second : QFileInfo(p.first).baseName();
            }
            else
            {
                q += "  ";
                q += p.second.size() ? p.second : QFileInfo(p.first).baseName();
            }
        }
        q += "\n";
        q += "\n";
    }

    QString p;
    switch (sdata.clear_if) {
    case 1:
        p += tr("当前为覆盖模式，即仅添加或更新现有时刻表\n");
        break;
    case 2:
        p += tr("当前为列表清空，即仅清空_line文件内包含的列表\n");
        break;
    case 3:
        p += tr("当前为全部清空模式，即删除所有原时刻表\n");
        break;
    default:
        break;
    }

    p += shrinkif ?
                    tr("当前时刻表存在重复或者相近（两组或多组时刻表所有站点时刻<5s），\n"
                    "按确认则随机保留一组并继续，按取消则返回") :
                    tr("请核对时刻表数量以及表单，按确认继续");

    if(!printq(q, p))
        return 0;



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
    msgBox.setText(QString(QObject::tr("作者：今天学高代了吗<br/>"
                                       "b站视频教程：<a href=\"https://www.bilibili.com/video/BV1yj2ABwE9v/"
                                       "?spm_id_from=333.1387.homepage.video_card.click&vd_source=3fd42c24215ba0da48b95a40864f298c\">"
                                       "https://www.bilibili.com/video/BV1yj2ABwE9v</a> <br/>"
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
    refresh();
}

QString mainui::loadMarkdownContent(const QString &docName)
{
    return MarkdownLanguageManager::instance().loadMarkdown(docName);
}


