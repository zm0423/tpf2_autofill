#include "mainui.h"

#include <QApplication>
#include <QGuiApplication>
#include "MarkdownLanguageManager.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);


    a.setOrganizationName("TPF2AUTOFILL");
    a.setApplicationName("TPF2autofill");

    MarkdownLanguageManager& langManager = MarkdownLanguageManager::instance();
    mainui w;
    w.show();
    return a.exec();
}
