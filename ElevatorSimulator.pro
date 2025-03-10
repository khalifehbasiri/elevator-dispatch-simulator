# ElevatorSimulator.pro

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

TEMPLATE = app
TARGET = ElevatorSimulator

SOURCES += \
    StopState.cpp \
    main.cpp \
    mainwindow.cpp \
    SimulationController.cpp \
    Elevator.cpp \
    ElevatorState.cpp \
    IdleState.cpp \
    MovingState.cpp \
    OverloadedState.cpp \
    FireState.cpp \
    Floor.cpp \
    Passenger.cpp \
    NearestCarStrategy.cpp \


HEADERS += \
    StopState.h \
    mainwindow.h \
    SimulationController.h \
    Elevator.h \
    ElevatorState.h \
    IdleState.h \
    MovingState.h \
    OverloadedState.h \
    FireState.h \
    Floor.h \
    Passenger.h \
    IElevatorDispatchStrategy.h \
    NearestCarStrategy.h \

FORMS += mainwindow.ui

# If you have resources:
# RESOURCES += resources.qrc
