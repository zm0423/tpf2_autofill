#include "data_add.h"
#include "ui_data_add.h"


#include <QClipboard>
#include <QString>
#include <string>
#include <QMessageBox>
#include <QInputDialog>
#include <QScreen>
#include <QDialog>




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
    ui->checkBox->setText(QString("截断\"%1\"以后内容").arg(sdata.trunc));
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
    display_info("提示", "站点复制代码已复制至粘贴板！");
}


void data_add::on_line_copy_clicked()
{
    copyToClipboard(linecpy);
    display_info("提示", "线路复制代码已复制至粘贴板！");
}


void data_add::on_station_input_clicked()
{
    std::string dat = ui->textEdit->toPlainText().toStdString();
    if(dat.empty())
        return;
    std::vector<std::pair<std::string, int>> linedat;

    sdata.station.clear();

    text_to_vector(dat, linedat);

    if(linedat.empty())
    {
        display_info("错误","数据过少");
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

    display_info("提示","站点数据导入成功，若需更改请查看 存档名_station.xlsx");

    return;
}


void data_add::on_line_input_clicked()
{
    std::string dat = ui->textEdit->toPlainText().toStdString();
    if(dat.empty())
        return;
    std::vector<std::pair<std::string, int>> linedat;

    sdata.station.clear();
    text_to_vector(dat, linedat);

    if(linedat.empty())
    {
        display_info("错误","数据过少");
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


    display_info("提示","线路数据导入成功，若需更改请查看 存档名_station.xlsx");

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
            "截断",
            "输入截断字符 默认/ \n"
            "例如\"G1/2 上海-北京\"会被截断成\"G1\"，用于时刻表匹配 \n"
            "被截断的线路在列表里会排序在相对上侧的位置\n"
            "\n"
            "请输入一个ASCII字符（允许空格）：\n"
            "• 可见字符: A-Z, a-z, 0-9, !@#$%^&*()等\n"
            "• 允许空格\n"
            "• 不允许其他控制字符（Tab、换行等）",
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
            QMessageBox::warning(nullptr, "错误",
                                 input.isEmpty() ? "输入不能为空！" : "只能输入一个字符！");
            continue;
        }

        // 获取字符
        QChar qchar = input.at(0);
        char ch = qchar.toLatin1();  // 转换为char

        // 检查是否为ASCII字符（0-127）
        // 注意：qchar.unicode() 可能返回大于255的值
        if (qchar.unicode() > 127) {
            QMessageBox::warning(nullptr, "错误",
                                 QString("'%1' 不是ASCII字符！\n请输入0-127范围内的字符。")
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
                msg = "不允许输入制表符(Tab)！";
            } else if (ch == '\n' || ch == '\r') {
                msg = "不允许输入换行符！";
            } else {
                msg = QString("字符 0x%1 是不可见的控制字符！")
                          .arg(static_cast<unsigned char>(ch), 2, 16, QChar('0')).toUpper();
            }
            QMessageBox::warning(nullptr, "错误", msg);
            continue;
        }
    }

    if(trunc == '\0')
        return;
    sdata.trunc = trunc;
    refresh_file(sdata);
    ui->checkBox->setText(QString("截断\"%1\"以后内容").arg(sdata.trunc));
}

namespace {
const QString instruction = "说明：\n"
                            "\n"
                            "游戏底层代码仅识别编号，所以需要进入游戏获取id\n"
                            "\n"
                            "先打开游戏，进入设置-高级，打开调试模式，并重启游戏，然后进入存档\n"
                            "按`打开控制台，也可在设置里自行设置按键\n"
                            "复制代码，在控制台里粘贴并回车，可以看到出来了很多数据\n"
                            "复制所有uil上面一直到你的代码那一行的数据，应该符合一行名字一行空格、一行数字一行空格的格式\n"
                            "名字有乱码是正常的\n"
                            "复制好了文本，粘贴进界面左侧，选择站点导入或者线路导入\n"
                            "线路允许进行截断，具体说明看截断选项\n"
                            "\n"
                            "注意：同步时刻表时所有的数据要和这里导入的相匹配，如果有不匹配请自己修改数据\n"
                            "不管是修改时刻表的数据还是_station、_line的数据，总之软件要正确的识别你的对应id\n";

}


void data_add::on_instruction_clicked()
{

    QDialog dialog;
    dialog.setWindowTitle("说明");

    // 获取屏幕尺寸
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 设置对话框为屏幕的70%
    int width = screenGeometry.width() * 0.7;
    int height = screenGeometry.height() * 0.7;
    dialog.resize(width, height);

    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);

    // 根据屏幕尺寸调整字体
    int fontSize = qMax(10, width / 80);  // 动态计算字体大小
    QFont font("Arial", fontSize);
    textEdit->setFont(font);

    // 加载长文本（可以从文件读取）
    textEdit->setPlainText(instruction);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addWidget(textEdit);


    dialog.exec();
}


