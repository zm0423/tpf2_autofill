#ifndef SIMPLEMARKDOWN_H
#define SIMPLEMARKDOWN_H

#include <QDialog>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QString>

class SimpleWebMarkdownDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimpleWebMarkdownDialog(const QString &title,
                                     const QString &markdown,
                                     QWidget *parent = nullptr);

    static void showDialog(const QString &title,
                           const QString &markdown,
                           QWidget *parent = nullptr);

    void setMarkdown(const QString &markdown);

    inline ~SimpleWebMarkdownDialog()
    {
        // 显式销毁 webView，确保资源释放
        if (webView) {
            webView->stop();
            webView->deleteLater();
        }
    }

private:
    void initUI();
    QString generateHtml(const QString &markdown);

    QWebEngineView *webView;
};


#endif // SIMPLEMARKDOWN_H
