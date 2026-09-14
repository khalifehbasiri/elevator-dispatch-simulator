#include "mainwindow.h"
#include "SimulationController.h"
#include "Elevator.h"
#include <QApplication>
#include <QPushButton>
#include <QSpinBox>
#include <QTest>
#include <iostream>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setFont(QFont("Segoe UI", 10));
    MainWindow window;
    window.show();
    auto* controller = SimulationController::getInstance();
    window.findChild<QSpinBox*>("floorsSpinBox")->setValue(5);
    window.findChild<QSpinBox*>("elevatorsSpinBox")->setValue(2);
    window.findChild<QSpinBox*>("passengersSpinBox")->setValue(3);
    window.findChild<QPushButton*>("applyConfigButton")->click();
    if (controller->getElevators().size() != 2 || controller->getFloors().size() != 5) return 1;
    window.findChild<QPushButton*>("startButton")->click();
    window.findChild<QPushButton*>("pauseButton")->click();
    int paused = controller->getCurrentTime();
    QTest::qWait(1100);
    if (controller->getCurrentTime() != paused) return 2;
    controller->callElevator(0, 0, 3);
    for (int i = 0; i < 5; ++i)
        QMetaObject::invokeMethod(controller, "advanceSimulation", Qt::DirectConnection);
    auto* elevator = controller->getElevators().front();
    if (elevator->getCurrentFloor() != 3 || elevator->getPassengerCount() != 0) return 3;
    QCoreApplication::processEvents();
    if (argc > 1 && !window.grab().save(argv[1])) return 4;
    controller->triggerFireAlarm();
    for (int i = 0; i < 5; ++i) elevator->update();
    if (elevator->getCurrentFloor() != 0) return 5;
    window.findChild<QPushButton*>("resumeButton")->click();
    int resumed = controller->getCurrentTime();
    QTest::qWait(1100);
    if (controller->getCurrentTime() <= resumed) return 6;
    window.findChild<QPushButton*>("stopButton")->click();
    if (elevator->getHasActiveRequest()) return 7;
    std::cout << "PASS: configuration, pause, journey, fire recall, resume and stop\n";
    return 0;
}
