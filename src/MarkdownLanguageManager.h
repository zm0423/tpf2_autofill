#ifndef MARKDOWNLANGUAGEMANAGER_H
#define MARKDOWNLANGUAGEMANAGER_H

// MarkdownLanguageManager.h - 升级版本
#pragma once
#include <QString>
#include <QSettings>
#include <QTranslator>
#include <QApplication>

class MarkdownLanguageManager : public QObject
{
    Q_OBJECT

public:
    enum Language {
        English = 0,
        Chinese = 1,
        SystemDefault = -1
    };

    static MarkdownLanguageManager& instance();

    // 语言设置
    Language currentLanguage() const;
    bool setLanguage(Language lang);  // 返回是否成功

    // 切换语言
    void toggleLanguage();

    // 按钮文本
    QString languageButtonText() const;

    // 加载Markdown
    QString loadMarkdown(const QString &docName) const;

    // 初始化（在main函数中调用）
    static void initialize();

signals:
    void languageChanged();  // 语言切换信号

private:
    MarkdownLanguageManager();
    bool loadQtTranslations(Language lang);
    QString getDocPath(const QString &docName) const;

    Language m_language = SystemDefault;
    QSettings m_settings;
    QTranslator m_appTranslator;   // 应用翻译
    QTranslator m_qtTranslator;    // Qt库翻译
};


#endif // MARKDOWNLANGUAGEMANAGER_H
