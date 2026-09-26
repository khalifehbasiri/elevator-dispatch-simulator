QT += core gui widgets
CONFIG += c++11

TEMPLATE = app
TARGET = ElevatorSimulator

INCLUDEPATH += src/model src/simulation src/ui

SOURCES += \
    src/main.cpp \
    $$files(src/model/*.cpp) \
    $$files(src/simulation/*.cpp) \
    src/ui/mainwindow.cpp

HEADERS += \
    $$files(src/model/*.h) \
    $$files(src/simulation/*.h) \
    src/ui/mainwindow.h

RESOURCES += src/ui/resources.qrc
