include(tpf2_autofill.pro)

TARGET = autofill_test
CONFIG += console
INCLUDEPATH += src
SOURCES -= src/main.cpp
SOURCES += tests/test_main.cpp
