QT += core gui widgets testlib
CONFIG += console c++11
CONFIG -= app_bundle
TEMPLATE = app
TARGET = elevator-smoke

INCLUDEPATH += $$PWD/../src/model $$PWD/../src/simulation $$PWD/../src/ui
SOURCES += \
    $$files($$PWD/../src/model/*.cpp) \
    $$files($$PWD/../src/simulation/*.cpp) \
    $$PWD/../src/ui/mainwindow.cpp \
    $$PWD/smoke.cpp
HEADERS += \
    $$files($$PWD/../src/model/*.h) \
    $$files($$PWD/../src/simulation/*.h) \
    $$PWD/../src/ui/mainwindow.h
FORMS += $$PWD/../src/ui/mainwindow.ui
