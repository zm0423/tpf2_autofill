#include "data_add.h"
#include "ui_data_add.h"


#include <QClipboard>
#include <QString>
#include <string>
#include <QMessageBox>
#include <QInputDialog>
#include <QScreen>
#include <QDialog>

#include "simplemarkdown.h"
#include "MarkdownLanguageManager.h"


namespace fs = std::filesystem;


void copyToClipboard(const std::string& text) {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(text));
}


data_add::data_add(my_data &input_data, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::data_add)
    , sdata(input_data)
{
    ui->setupUi(this);
    ui->checkBox->setChecked(sdata.trunc_if);
    ui->checkBox->setText(QString(tr("截断\"%1\"以后内容")).arg(sdata.trunc));
    setWindowTitle(tr("站点、线路数据导入"));
    QObject::connect(ui->checkBox, &QCheckBox::toggled,
                     this, [&](bool p) {
                         sdata.trunc_if = p;
                         refresh_file(sdata);
                     });
}

data_add::~data_add()
{
    delete ui;
}

void data_add::on_station_copy_clicked()
{
    copyToClipboard(stationcpy);
    display_info(tr("提示"), tr("站点复制代码已复制至粘贴板！"));
}


void data_add::on_line_copy_clicked()
{
    copyToClipboard(linecpy);
    display_info(tr("提示"), tr("线路复制代码已复制至粘贴板！"));
}


void data_add::on_station_input_clicked()
{
    std::string dat = ui->textEdit->toPlainText().toStdString();
    if(dat.empty())
        return;
    std::vector<std::pair<std::string, int>> linedat;



    if(!text_to_vector(dat, linedat))
        return;

    if(linedat.empty())
    {
        display_info(tr("错误"),tr("数据过少"));
        return;
    }


    std::sort(linedat.begin(), linedat.end(),
              [](const auto& a, const auto& b){
                  return checkEnding(a.first) < checkEnding(b.first);
              });



    if(!writeVectorToXlsx(linedat, sdata.folder_dir / (sdata.sg_name + "_station.xlsx")))
        return;

    sdata.station.clear();
    sdata.station.insert(linedat.cbegin(), linedat.cend());


    ui->textEdit->setText("");

    display_info(tr("提示"),tr("站点数据导入成功，若需更改请查看 存档名_station.xlsx"));

    return;
}


void data_add::on_line_input_clicked()
{
    std::string dat = ui->textEdit->toPlainText().toStdString();
    if(dat.empty())
        return;
    std::vector<std::pair<std::string, int>> linedat;

    if(!text_to_vector(dat, linedat))
        return;

    if(linedat.empty())
    {
        display_info(tr("错误"),tr("数据过少"));
        return;
    }

    sdata.trunc_if = ui->checkBox->isChecked();

    std::vector<int> tags(linedat.size());

    std::vector<std::pair<int, int>> temp;
    temp.reserve(linedat.size());
    int i = 0;

    std::transform(linedat.begin(), linedat.end(),
                    std::back_inserter(temp),
                    [&](auto& p){
                            int tag = sdata.trunc_if ? (get_first_cut(p.first, sdata.trunc) ? 1 : 0) : 0;
                            return std::make_pair(i++, tag);
    });

    std::sort(temp.begin(), temp.end(), [&](const auto &p1, const auto &p2){
                                if(p1.second != p2.second)
                                    return p1.second > p2.second;
                                else
                                    return linedat.at(p1.first).first < linedat.at(p2.first).first;
        });




    if(!writeVectorToXlsx(linedat, sdata.folder_dir / (sdata.sg_name + "_line.xlsx"), temp))
        return;


    display_info(tr("提示"),tr("线路数据导入成功，若需更改请查看 存档名_line.xlsx"));

    ui->textEdit->setText("");

    sdata.line = std::move(linedat);


    return;

}




void data_add::on_instruction_2_clicked()
{
    char trunc;
    while (true) {
        bool ok = false;

        // 弹出输入对话框
        QString input = QInputDialog::getText(
            nullptr,
            tr("截断"),
            tr("输入截断字符 默认/ \n"
            "例如\"G1/2 上海-北京\"会被截断成\"G1\"，用于时刻表匹配 \n"
            "被截断的线路在列表里会排序在相对上侧的位置\n"
            "\n"
            "请输入一个ASCII字符（允许空格）：\n"
            "• 可见字符: A-Z, a-z, 0-9, !@#$%^&*()等\n"
            "• 允许空格\n"
            "• 不允许其他控制字符（Tab、换行等）"),
            QLineEdit::Normal,
            "",
            &ok
            );

        // 用户点击取消
        if (!ok) {
            trunc = '\0';
            break;            // 返回空字符表示取消
        }

        // 检查输入长度
        if (input.length() != 1) {
            QMessageBox::warning(nullptr, tr("错误"),
                                 input.isEmpty() ? tr("输入不能为空！") : tr("只能输入一个字符！"));
            continue;
        }

        // 获取字符
        QChar qchar = input.at(0);
        char ch = qchar.toLatin1();  // 转换为char

        // 检查是否为ASCII字符（0-127）
        // 注意：qchar.unicode() 可能返回大于255的值
        if (qchar.unicode() > 127) {
            QMessageBox::warning(nullptr, tr("错误"),
                                 QString(tr("'%1' 不是ASCII字符！\n请输入0-127范围内的字符。"))
                                     .arg(qchar));
            continue;
        }

        // 检查字符是否有效
        // 允许：可见字符（isprint）或空格
        if (ch == ' ') {
            // 空格是允许的
            trunc = ' ';
            break;
        }
        else if (std::isprint(static_cast<unsigned char>(ch))) {
            // 其他可见ASCII字符
            trunc = ch;
            break;
        }
        else {
            // 不可见的控制字符（除空格外）
            QString msg;
            if (ch == '\t') {
                msg = tr("不允许输入制表符(Tab)！");
            } else if (ch == '\n' || ch == '\r') {
                msg = tr("不允许输入换行符！");
            } else {
                msg = QString(tr("字符 0x%1 是不可见的控制字符！"))
                          .arg(static_cast<unsigned char>(ch), 2, 16, QChar('0')).toUpper();
            }
            QMessageBox::warning(nullptr, tr("错误"), msg);
            continue;
        }
    }

    if(trunc == '\0')
        return;
    sdata.trunc = trunc;
    refresh_file(sdata);
    ui->checkBox->setText(QString(tr("截断\"%1\"以后内容")).arg(sdata.trunc));
}

void data_add::on_instruction_clicked()
{

    QString markdown = MarkdownLanguageManager::instance().loadMarkdown("data_add");
    QString title = tr("说明");
    SimpleWebMarkdownDialog::showDialog(title, markdown, this);
}


