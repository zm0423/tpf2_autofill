QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# 指定源代码中的语言（默认英文）
CODECFORTR = UTF-8

SOURCELANGUAGE = zh_CN  # 如果你的源代码是中文字符串

TARGET = tpf2_autofill

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/MarkdownLanguageManager.cpp \
    src/data_add.cpp \
    src/main.cpp \
    src/mainui.cpp \
    src/simplemarkdown.cpp \
    src/util.cpp \

RESOURCES += resources.qrc

HEADERS += \
    src/MarkdownLanguageManager.h \
    src/data_add.h \
    src/mainui.h \
    src/simplemarkdown.h \
    src/util.h \

FORMS += \
    ui/data_add.ui \
    ui/mainui.ui

TRANSLATIONS += \
    tpf2_autofill_zh_CN.ts \  # 源语言，方便查看所有可翻译文本
    tpf2_autofill_en_US.ts


# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

