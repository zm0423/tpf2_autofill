// SimpleMarkdownDialog.cpp
#include "simplemarkdown.h"
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QRegularExpression>
#include <QFile>

SimpleWebMarkdownDialog::SimpleWebMarkdownDialog(const QString &title,
                                                 const QString &markdown,
                                                 QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);

    // 只保留关闭按钮，去掉最小化/最大化
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    initUI();
    setMarkdown(markdown);

    // 自适应大小
    QTimer::singleShot(0, this, [this]() {
        QScreen *screen = QApplication::primaryScreen();
        if (screen) {
            QRect geometry = screen->availableGeometry();
            resize(qMin(800, geometry.width() - 100),
                   qMin(600, geometry.height() - 100));
        }
    });
}

void SimpleWebMarkdownDialog::initUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    webView = new QWebEngineView(this);
    webView->setContextMenuPolicy(Qt::NoContextMenu); // 禁用右键菜单

    layout->addWidget(webView);

    // 设置对话框最小大小
    setMinimumSize(400, 300);
}

QString SimpleWebMarkdownDialog::generateHtml(const QString &markdown)
{
    // 读取本地CSS
    QFile cssFile(":/web/github-markdown.css");
    QString cssContent;
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cssContent = QString::fromUtf8(cssFile.readAll());
        cssFile.close();
    }

    // 构建HTML
    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Markdown Viewer</title>

    <style>
        %1

/* 1. 修复标题分隔线 */
.markdown-body h1,
.markdown-body h2 {
    padding-bottom: 0.3em;
    border-bottom: 1px solid #eaecef !important; /* 确保分割线显示 */
}

/* 2. 修复表格边框（关键） */
.markdown-body table {
    border-collapse: collapse !important;
    border-spacing: 0 !important;
    display: block;
    width: 100%;
    overflow: auto;
    word-break: keep-all; /* 防止表格内容换行 */
}

.markdown-body table th,
.markdown-body table td {
    padding: 6px 13px;
    border: 1px solid #dfe2e5 !important; /* 确保每个单元格都有边框 */
}

.markdown-body table tr {
    background-color: #fff;
    border-top: 1px solid #c6cbd1;
}

.markdown-body table tr:nth-child(2n) {
    background-color: #f6f8fa;
}

/* 3. 修复任务列表等GitHub特有元素 */
.markdown-body .task-list-item {
    list-style-type: none;
}

.markdown-body .task-list-item-checkbox {
    margin: 0 0.2em 0.25em -1.6em;
    vertical-align: middle;
}

/* 4. 修复代码块的额外样式 */
.markdown-body pre {
    border-radius: 6px;
    padding: 16px;
    overflow: auto;
    font-size: 85%;
    line-height: 1.45;
    background-color: #f6f8fa;
}

/* 5. 确保Markdown正文的基础间距 */
.markdown-body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
    font-size: 16px;
    line-height: 1.5;
    word-wrap: break-word;
}

.markdown-body > *:first-child {
    margin-top: 0 !important;
}

.markdown-body > *:last-child {
    margin-bottom: 0 !important;
}
        /* 自定义样式 */
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
            background-color: white;
            margin: 0;
            padding: 0;
        }

        .markdown-body {
            box-sizing: border-box;
            min-width: 200px;
            max-width: 980px;
            margin: 0 auto;
            padding: 45px;
        }

        @media (max-width: 767px) {
            .markdown-body {
                padding: 15px;
            }
        }

        /* 自定义滚动条 */
        ::-webkit-scrollbar {
            width: 12px;
        }
        ::-webkit-scrollbar-track {
            background: #f1f1f1;
            border-radius: 6px;
        }
        ::-webkit-scrollbar-thumb {
            background: #888;
            border-radius: 6px;
        }
        ::-webkit-scrollbar-thumb:hover {
            background: #555;
        }
    </style>
</head>
<body>
    <article class="markdown-body" id="content"></article>

    <!-- 加载本地JS库 -->
    <script src="qrc:/web/markdown-it.min.js"></script>
    <script src="qrc:/web/highlight.min.js"></script>

    <!-- 加载语言包 -->
    <script src="qrc:/web/languages/cpp.min.js"></script>
    <script src="qrc:/web/languages/markdown.min.js"></script>
    <!-- 可选添加其他语言 -->

    <script>
        // 初始化 markdown-it
        const md = window.markdownit({
            html: true,           // 允许HTML标签
            linkify: true,        // 自动链接URL
            typographer: true,    // 排版优化
            highlight: function (str, lang) {
                if (lang && hljs.getLanguage(lang)) {
                    try {
                        return hljs.highlight(str, {
                            language: lang,
                            ignoreIllegals: true
                        }).value;
                    } catch (__) {}
                }
                return ''; // 使用默认转义
            }
        });

        // Markdown内容
        const markdownContent = `)");

                           // 转义Markdown内容
                           QString escaped = markdown;
                           escaped.replace("\\", "\\\\")
                               .replace("`", "\\`")
                               .replace("$", "\\$");

                           html += escaped;

                           html += QString(R"(`;

        // 渲染Markdown
        const result = md.render(markdownContent);
        document.getElementById('content').innerHTML = result;

        // 高亮所有代码块
        document.querySelectorAll('pre code').forEach((block) => {
            hljs.highlightElement(block);
        });

        // 为所有链接添加 target="_blank"
        document.querySelectorAll('a').forEach((link) => {
            link.setAttribute('target', '_blank');
            link.setAttribute('rel', 'noopener noreferrer');
        });

        // 调整图片大小
        document.querySelectorAll('img').forEach((img) => {
            img.style.maxWidth = '100%';
            img.style.height = 'auto';
        });
    </script>
</body>
</html>
)").arg(cssContent);

                           return html;
}



void SimpleWebMarkdownDialog::setMarkdown(const QString &markdown)
{
    QString html = generateHtml(markdown);
    webView->setHtml(html);
}

void SimpleWebMarkdownDialog::showDialog(const QString &title,
                                         const QString &markdown,
                                         QWidget *parent)
{
    SimpleWebMarkdownDialog *dialog = new SimpleWebMarkdownDialog(title, markdown, parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}
