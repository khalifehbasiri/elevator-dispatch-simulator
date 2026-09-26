#include "mainwindow.h"

#include "Elevator.h"
#include "Floor.h"
#include "SimulationController.h"

#include <QApplication>
#include <QComboBox>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStyle>
#include <QTextDocument>
#include <QVBoxLayout>
#include <algorithm>

namespace {
QLabel* label(const QString& text, const char* role = nullptr)
{
    auto* result = new QLabel(text);
    if (role) result->setProperty("role", role);
    return result;
}

QFrame* card(const QString& title, const QString& description, QVBoxLayout*& content)
{
    auto* frame = new QFrame;
    frame->setObjectName("card");
    content = new QVBoxLayout(frame);
    content->setContentsMargins(22, 20, 22, 20);
    content->setSpacing(12);
    content->addWidget(label(title, "sectionTitle"));
    if (!description.isEmpty()) content->addWidget(label(description, "muted"));
    return frame;
}

QSpinBox* spin(const char* name, int minimum, int maximum, int value)
{
    auto* result = new QSpinBox;
    result->setObjectName(name);
    result->setRange(minimum, maximum);
    result->setValue(value);
    return result;
}

void addField(QGridLayout* grid, int row, int column, const QString& title, QWidget* control)
{
    auto* field = new QVBoxLayout;
    field->setSpacing(6);
    field->addWidget(label(title, "fieldLabel"));
    field->addWidget(control);
    grid->addLayout(field, row, column);
}
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    QFile stylesheet(":/styles.qss");
    if (stylesheet.open(QIODevice::ReadOnly))
        qApp->setStyleSheet(QString::fromUtf8(stylesheet.readAll()));

    buildUi();
    connect(SimulationController::getInstance(), &SimulationController::simulationUpdated,
            this, &MainWindow::onSimulationUpdated);
    applyConfiguration();
}

void MainWindow::buildUi()
{
    setWindowTitle("Elevator Dispatch Studio");
    resize(1280, 960);
    setMinimumSize(920, 650);

    auto* root = new QWidget;
    root->setObjectName("root");
    setCentralWidget(root);
    auto* page = new QVBoxLayout(root);
    page->setContentsMargins(28, 24, 28, 24);
    page->setSpacing(18);

    auto* header = new QHBoxLayout;
    header->setSpacing(16);
    auto* mark = label("E", "brand");
    mark->setAlignment(Qt::AlignCenter);
    mark->setFixedSize(48, 48);
    header->addWidget(mark);
    auto* titleStack = new QVBoxLayout;
    titleStack->setSpacing(2);
    titleStack->addWidget(label("Elevator Dispatch Studio", "pageTitle"));
    titleStack->addWidget(label("Explore requests, movement and safety scenarios", "muted"));
    header->addLayout(titleStack);
    header->addStretch();
    statusLabel = label("READY", "status");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    header->addWidget(statusLabel);
    timeLabel = label("00:00", "clock");
    timeLabel->setObjectName("timeLabel");
    timeLabel->setAlignment(Qt::AlignCenter);
    header->addWidget(timeLabel);
    page->addLayout(header);

    QVBoxLayout* controlContent;
    auto* controlCard = card("Simulation", "Run the clock to process scheduled requests.", controlContent);
    auto* controlRow = new QHBoxLayout;
    controlRow->setSpacing(10);
    startButton = new QPushButton("Start");
    startButton->setObjectName("startButton");
    startButton->setProperty("variant", "primary");
    pauseButton = new QPushButton("Pause");
    pauseButton->setObjectName("pauseButton");
    resumeButton = new QPushButton("Resume");
    resumeButton->setObjectName("resumeButton");
    stopButton = new QPushButton("Stop");
    stopButton->setObjectName("stopButton");
    stopButton->setProperty("variant", "danger");
    for (auto* button : {startButton, pauseButton, resumeButton, stopButton})
        controlRow->addWidget(button);
    controlRow->addStretch();
    controlContent->addLayout(controlRow);
    page->addWidget(controlCard);

    auto* workspace = new QHBoxLayout;
    workspace->setSpacing(18);
    page->addLayout(workspace, 1);

    QVBoxLayout* buildingContent;
    auto* buildingCard = card("Building overview", "Cars move one floor per simulation tick.", buildingContent);
    auto* floorScroll = new QScrollArea;
    floorScroll->setWidgetResizable(true);
    floorScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    floorScroll->setFrameShape(QFrame::NoFrame);
    floorScroll->setObjectName("floorScroll");
    floorGridFrame = new QFrame;
    floorGridFrame->setObjectName("floorGridFrame");
    auto* floorGrid = new QGridLayout(floorGridFrame);
    floorGrid->setContentsMargins(2, 2, 2, 2);
    floorGrid->setHorizontalSpacing(10);
    floorGrid->setVerticalSpacing(8);
    floorScroll->setWidget(floorGridFrame);
    buildingContent->addWidget(floorScroll, 1);
    buildingContent->addWidget(label("●  Active car position     ·  Empty shaft", "legend"));
    workspace->addWidget(buildingCard, 3);

    auto* sideScroll = new QScrollArea;
    sideScroll->setObjectName("sideScroll");
    sideScroll->setWidgetResizable(true);
    sideScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sideScroll->setFrameShape(QFrame::NoFrame);
    auto* side = new QWidget;
    auto* sideLayout = new QVBoxLayout(side);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(14);
    sideScroll->setWidget(side);
    workspace->addWidget(sideScroll, 2);

    QVBoxLayout* configContent;
    auto* configCard = card("Building setup", "Changing the setup resets the current run.", configContent);
    auto* configGrid = new QGridLayout;
    configGrid->setHorizontalSpacing(10);
    floorsSpinBox = spin("floorsSpinBox", 2, 20, 5);
    elevatorsSpinBox = spin("elevatorsSpinBox", 1, 6, 2);
    passengersSpinBox = spin("passengersSpinBox", 1, 50, 3);
    addField(configGrid, 0, 0, "Floors", floorsSpinBox);
    addField(configGrid, 0, 1, "Cars", elevatorsSpinBox);
    addField(configGrid, 0, 2, "Passengers", passengersSpinBox);
    configContent->addLayout(configGrid);
    auto* applyConfigButton = new QPushButton("Apply setup");
    applyConfigButton->setObjectName("applyConfigButton");
    configContent->addWidget(applyConfigButton);
    sideLayout->addWidget(configCard);

    QVBoxLayout* passengerContent;
    auto* passengerCard = card("Passenger request", "Schedule a pickup and destination.", passengerContent);
    auto* passengerGrid = new QGridLayout;
    passengerGrid->setHorizontalSpacing(10);
    passengerGrid->setVerticalSpacing(10);
    scriptTimeSpinBox = spin("scriptTimeSpinBox", 1, 3600, 2);
    passengerIdSpinBox = spin("passengerIdSpinBox", 0, 2, 0);
    startFloorSpinBox = spin("startFloorSpinBox", 0, 4, 0);
    destFloorSpinBox = spin("destFloorSpinBox", 0, 4, 3);
    addField(passengerGrid, 0, 0, "At second", scriptTimeSpinBox);
    addField(passengerGrid, 0, 1, "Passenger ID", passengerIdSpinBox);
    addField(passengerGrid, 1, 0, "From floor", startFloorSpinBox);
    addField(passengerGrid, 1, 1, "To floor", destFloorSpinBox);
    passengerContent->addLayout(passengerGrid);
    auto* addPassengerScriptButton = new QPushButton("Schedule request");
    addPassengerScriptButton->setObjectName("addPassengerScriptButton");
    passengerContent->addWidget(addPassengerScriptButton);
    scriptListOutput = new QPlainTextEdit;
    scriptListOutput->setObjectName("scriptListOutput");
    scriptListOutput->setReadOnly(true);
    scriptListOutput->setMaximumHeight(82);
    scriptListOutput->setPlaceholderText("No requests scheduled");
    passengerContent->addWidget(scriptListOutput);
    sideLayout->addWidget(passengerCard);

    QVBoxLayout* eventContent;
    auto* eventCard = card("Safety event", "Schedule an alarm or emergency stop.", eventContent);
    auto* eventGrid = new QGridLayout;
    eventGrid->setHorizontalSpacing(10);
    eventTimeSpinBox = spin("eventTimeSpinBox", 1, 3600, 4);
    eventTypeComboBox = new QComboBox;
    eventTypeComboBox->setObjectName("eventTypeComboBox");
    eventTypeComboBox->addItems({"Fire recall", "Help alarm", "Overload", "Emergency stop"});
    addField(eventGrid, 0, 0, "At second", eventTimeSpinBox);
    addField(eventGrid, 0, 1, "Scenario", eventTypeComboBox);
    eventContent->addLayout(eventGrid);
    auto* addSafetyEventButton = new QPushButton("Schedule event");
    addSafetyEventButton->setObjectName("addSafetyEventButton");
    eventContent->addWidget(addSafetyEventButton);
    eventListOutput = new QPlainTextEdit;
    eventListOutput->setObjectName("eventListOutput");
    eventListOutput->setReadOnly(true);
    eventListOutput->setMaximumHeight(82);
    eventListOutput->setPlaceholderText("No events scheduled");
    eventContent->addWidget(eventListOutput);
    sideLayout->addWidget(eventCard);
    sideLayout->addStretch();

    QVBoxLayout* logContent;
    auto* logCard = card("Activity", QString(), logContent);
    logCard->setMaximumHeight(140);
    logOutput = new QPlainTextEdit;
    logOutput->setObjectName("logOutput");
    logOutput->setReadOnly(true);
    logOutput->document()->setMaximumBlockCount(200);
    logOutput->setMinimumHeight(65);
    logContent->addWidget(logOutput);
    page->addWidget(logCard);

    connect(applyConfigButton, &QPushButton::clicked, this, &MainWindow::applyConfiguration);
    connect(addPassengerScriptButton, &QPushButton::clicked, this, &MainWindow::addPassengerScript);
    connect(addSafetyEventButton, &QPushButton::clicked, this, &MainWindow::addSafetyEvent);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pauseSimulation);
    connect(resumeButton, &QPushButton::clicked, this, &MainWindow::resumeSimulation);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSimulation);
}

void MainWindow::applyConfiguration()
{
    auto* controller = SimulationController::getInstance();
    controller->configure(floorsSpinBox->value(), elevatorsSpinBox->value(), passengersSpinBox->value());
    passengerScripts.clear();
    safetyEvents.clear();
    scriptListOutput->clear();
    eventListOutput->clear();
    passengerIdSpinBox->setMaximum(passengersSpinBox->value() - 1);
    startFloorSpinBox->setMaximum(floorsSpinBox->value() - 1);
    destFloorSpinBox->setMaximum(floorsSpinBox->value() - 1);
    destFloorSpinBox->setValue(std::min(3, floorsSpinBox->value() - 1));
    scriptTimeSpinBox->setMinimum(1);
    eventTimeSpinBox->setMinimum(1);
    timeLabel->setText("00:00");
    runState = RunState::Ready;
    updateControls();
    refreshFloorGrid();
    appendToLog(QString("Setup applied: %1 floors, %2 cars, %3 passengers.")
                    .arg(floorsSpinBox->value()).arg(elevatorsSpinBox->value()).arg(passengersSpinBox->value()));
}

void MainWindow::addPassengerScript()
{
    if (startFloorSpinBox->value() == destFloorSpinBox->value()) {
        appendToLog("Choose different pickup and destination floors.");
        return;
    }
    const int nextTick = SimulationController::getInstance()->getCurrentTime() + 1;
    if (scriptTimeSpinBox->value() < nextTick) {
        appendToLog("Choose a future second for the request.");
        return;
    }
    PassengerScript request{scriptTimeSpinBox->value(), passengerIdSpinBox->value(),
                            startFloorSpinBox->value(), destFloorSpinBox->value()};
    passengerScripts.append(request);
    const QString summary = QString("t=%1  ·  Passenger %2  ·  F%3 → F%4")
                                .arg(request.time).arg(request.passengerID)
                                .arg(request.startFloor).arg(request.destFloor);
    scriptListOutput->appendPlainText(summary);
    appendToLog("Scheduled " + summary);
}

void MainWindow::addSafetyEvent()
{
    const int nextTick = SimulationController::getInstance()->getCurrentTime() + 1;
    if (eventTimeSpinBox->value() < nextTick) {
        appendToLog("Choose a future second for the event.");
        return;
    }
    SafetyEvent event{eventTimeSpinBox->value(), eventTypeComboBox->currentText()};
    safetyEvents.append(event);
    const QString summary = QString("t=%1  ·  %2").arg(event.time).arg(event.type);
    eventListOutput->appendPlainText(summary);
    appendToLog("Scheduled " + summary);
}

void MainWindow::startSimulation()
{
    SimulationController::getInstance()->startSimulation();
    runState = RunState::Running;
    updateControls();
    appendToLog("Simulation started.");
}

void MainWindow::pauseSimulation()
{
    SimulationController::getInstance()->pauseSimulation();
    runState = RunState::Paused;
    updateControls();
    appendToLog("Simulation paused.");
}

void MainWindow::resumeSimulation()
{
    SimulationController::getInstance()->resumeSimulation();
    runState = RunState::Running;
    updateControls();
    appendToLog("Simulation resumed.");
}

void MainWindow::stopSimulation()
{
    SimulationController::getInstance()->stopSimulation();
    runState = RunState::Stopped;
    updateControls();
    refreshFloorGrid();
    appendToLog("Simulation stopped. Apply setup to start a new run.");
}

void MainWindow::onSimulationUpdated()
{
    auto* controller = SimulationController::getInstance();
    const int now = controller->getCurrentTime();
    timeLabel->setText(QString("%1:%2").arg(now / 60, 2, 10, QLatin1Char('0'))
                       .arg(now % 60, 2, 10, QLatin1Char('0')));

    for (const auto& request : passengerScripts) {
        if (request.time != now) continue;
        controller->callElevator(request.passengerID, request.startFloor, request.destFloor);
        appendToLog(QString("Passenger %1 called a car: F%2 → F%3.")
                        .arg(request.passengerID).arg(request.startFloor).arg(request.destFloor));
    }
    for (const auto& event : safetyEvents) {
        if (event.time != now) continue;
        appendToLog(event.type + " triggered.");
        if (event.type == "Fire recall") controller->triggerFireAlarm();
        else if (event.type == "Help alarm") controller->triggerHelpAlarm();
        else if (event.type == "Overload") controller->triggerOverloadAlarm();
        else if (event.type == "Emergency stop") {
            stopSimulation();
            break;
        }
    }

    scriptTimeSpinBox->setMinimum(std::min(now + 1, 3600));
    eventTimeSpinBox->setMinimum(std::min(now + 1, 3600));
    refreshFloorGrid();
}

void MainWindow::refreshFloorGrid()
{
    auto* grid = qobject_cast<QGridLayout*>(floorGridFrame->layout());
    while (auto* item = grid->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    auto* controller = SimulationController::getInstance();
    const auto& elevators = controller->getElevators();
    const auto& floors = controller->getFloors();

    grid->addWidget(label("FLOOR", "columnHeader"), 0, 0);
    for (int column = 0; column < static_cast<int>(elevators.size()); ++column)
        grid->addWidget(label(QString("CAR %1").arg(column + 1, 2, 10, QLatin1Char('0')),
                              "columnHeader"), 0, column + 1);

    std::vector<Floor*> sortedFloors(floors.begin(), floors.end());
    std::sort(sortedFloors.begin(), sortedFloors.end(),
              [](Floor* a, Floor* b) { return a->getFloorNumber() > b->getFloorNumber(); });
    for (int row = 0; row < static_cast<int>(sortedFloors.size()); ++row) {
        const int floor = sortedFloors[row]->getFloorNumber();
        auto* floorLabel = label(floor == 0 ? "G" : QString("F%1").arg(floor), "floorLabel");
        floorLabel->setMinimumHeight(42);
        grid->addWidget(floorLabel, row + 1, 0);
        for (int column = 0; column < static_cast<int>(elevators.size()); ++column) {
            const bool present = elevators[column]->getCurrentFloor() == floor;
            auto* cell = label(present ? "●  HERE" : "·", present ? "carHere" : "emptyShaft");
            cell->setAlignment(Qt::AlignCenter);
            cell->setMinimumHeight(42);
            grid->addWidget(cell, row + 1, column + 1);
        }
    }
    grid->setColumnStretch(0, 1);
    for (int column = 1; column <= static_cast<int>(elevators.size()); ++column)
        grid->setColumnStretch(column, 1);
    grid->setRowStretch(static_cast<int>(sortedFloors.size()) + 1, 1);
}

void MainWindow::updateControls()
{
    startButton->setEnabled(runState == RunState::Ready);
    pauseButton->setEnabled(runState == RunState::Running);
    resumeButton->setEnabled(runState == RunState::Paused);
    stopButton->setEnabled(runState == RunState::Running || runState == RunState::Paused);

    const char* state = "ready";
    QString text = "READY";
    if (runState == RunState::Running) { state = "running"; text = "RUNNING"; }
    if (runState == RunState::Paused) { state = "paused"; text = "PAUSED"; }
    if (runState == RunState::Stopped) { state = "stopped"; text = "STOPPED"; }
    statusLabel->setText(text);
    statusLabel->setProperty("state", state);
    statusLabel->style()->unpolish(statusLabel);
    statusLabel->style()->polish(statusLabel);
}

void MainWindow::appendToLog(const QString& message)
{
    logOutput->appendPlainText(message);
}
