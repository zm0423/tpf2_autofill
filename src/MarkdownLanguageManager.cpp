// MarkdownLanguageManager.cpp
#include "MarkdownLanguageManager.h"
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDebug>
#include <QMessageBox>

MarkdownLanguageManager& MarkdownLanguageManager::instance()
{
    static MarkdownLanguageManager instance;
    return instance;
}

MarkdownLanguageManager::MarkdownLanguageManager()
{
    // 从设置读取
    int savedLang = m_settings.value("language", SystemDefault).toInt();

    // 如果没保存过，检测系统语言
    if (savedLang == SystemDefault) {
        QLocale locale;
        QString sysLang = locale.name();

        if (sysLang.startsWith("zh")) {
            m_language = Chinese;
        } else {
            m_language = English;
        }
    } else {
        m_language = static_cast<Language>(savedLang);
    }

    // 加载翻译文件
    loadQtTranslations(m_language);
}

bool MarkdownLanguageManager::loadQtTranslations(Language lang)
{
    QString langCode = (lang == Chinese) ? "zh_CN" : "en_US";

    // 移除旧翻译
    QCoreApplication::removeTranslator(&m_appTranslator);
    QCoreApplication::removeTranslator(&m_qtTranslator);

    // 加载应用翻译
    QString appTransFile = QString(":/translations/tpf2_autofill_%1.qm").arg(langCode);
    if (m_appTranslator.load(appTransFile)) {
        QCoreApplication::installTranslator(&m_appTranslator);
    } else if (lang == Chinese) {
        // 中文翻译加载失败时警告
        qWarning() << "Cannot load translation file:" << appTransFile;
    }

    // 加载Qt库翻译（可选）
    QString qtTransFile = QString(":/translations/qt_%1.qm").arg(langCode);
    if (m_qtTranslator.load(qtTransFile)) {
        QCoreApplication::installTranslator(&m_qtTranslator);
    }

    return true;
}

bool MarkdownLanguageManager::setLanguage(Language lang)
{
    if (m_language == lang) {
        return true;
    }

    // 加载翻译
    if (!loadQtTranslations(lang)) {
        return false;
    }

    m_language = lang;
    m_settings.setValue("language", lang);

    emit languageChanged();
    return true;
}

void MarkdownLanguageManager::toggleLanguage()
{
    Language newLang = (m_language == English) ? Chinese : English;
    setLanguage(newLang);
}

QString MarkdownLanguageManager::languageButtonText() const
{
    return (m_language == English) ? "中文" : "English";
}

QString MarkdownLanguageManager::loadMarkdown(const QString &docName) const
{
    QString langCode = (m_language == English) ? "en_US" : "zh_CN";
    QString path = QString(":/docs/%1_%2.md").arg(docName).arg(langCode);

    // 回退机制...
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QString fallback = (m_language == English)
        ? QString(":/docs/%1_zh_CN.md").arg(docName)
        : QString(":/docs/%1_en_US.md").arg(docName);
        file.setFileName(fallback);

        if (!file.open(QIODevice::ReadOnly)) {
            return tr("# Error\nCannot load document.");
        }
    }
    return QString::fromUtf8(file.readAll());
}
