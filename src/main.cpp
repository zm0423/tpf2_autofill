#include "mainui.h"

#include <QApplication>
#include <QGuiApplication>
#include "MarkdownLanguageManager.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);


    a.setOrganizationName("TPF2AUTOFILL");
    a.setApplicationName("TPF2autofill");

    // 修复深色主题下提示框黑底的问题
    a.setStyleSheet("QToolTip { color: #000000; background-color: #ffffff; border: 1px solid #8f8f91; }");

    MarkdownLanguageManager& langManager = MarkdownLanguageManager::instance();
    mainui w;
    w.show();
    return a.exec();
}
