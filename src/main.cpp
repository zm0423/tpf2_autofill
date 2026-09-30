#include "mainui.h"

#include <QApplication>
#include <QGuiApplication>
#include "MarkdownLanguageManager.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);


    a.setOrganizationName("TPF2AUTOFILL");
    a.setApplicationName("TPF2autofill");

    // 修复深色主题下提示框黑底的问题；路径标签的框体样式也必须走全局样式表
    // （widget 自带样式表会连带把它的 tooltip 染成近透明黑底，QToolTip 规则放哪都不生效）
    a.setStyleSheet(
        "QToolTip { color: #000000; background-color: #ffffff; border: 1px solid #8f8f91; }"
        "QLabel[pathBox=\"true\"] { background-color: rgba(0, 0, 0, 20); border: 1px solid rgba(0, 0, 0, 40); border-radius: 3px; padding: 2px 6px; }");

    MarkdownLanguageManager& langManager = MarkdownLanguageManager::instance();
    mainui w;
    w.show();
    return a.exec();
}
