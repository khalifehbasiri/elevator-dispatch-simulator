#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Floor.h"
#include "SimulationController.h"
#include "Elevator.h"

#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <algorithm> // for std::sort

MainWindow* MainWindow::activeWindow = nullptr;

void MainWindow::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    if (msg.startsWith('"') && msg.endsWith('"')) {
        return;
    }

    if (msg.startsWith("Configuring simulation with") ||
        msg.startsWith("Added Passenger Script:") ||
        msg.startsWith("PassengerScript triggered:") ||
        msg.startsWith("SafetyEvent triggered:") ||
        msg.startsWith("Simulation STARTING") ||
        msg.startsWith("Simulation PAUSED") ||
        msg.startsWith("Simulation RESUMED") ||
        msg.startsWith("Simulation STOPPED")) {
        return;
    }

    if (activeWindow) {
        switch (type) {
            case QtDebugMsg:
                activeWindow->appendToLog(msg);
                break;
            case QtWarningMsg:
                activeWindow->appendToLog("Warning: " + msg);
                break;
            case QtCriticalMsg:
                activeWindow->appendToLog("Critical: " + msg);
                break;
            case QtFatalMsg:
                activeWindow->appendToLog("Fatal: " + msg);
                break;
            default:
                activeWindow->appendToLog(msg);
                break;
        }
    }
}

void MainWindow::appendToLog(const QString& message)
{
    if (message == lastLogMessage) {
        return; // Skip duplicate messages
    }
    
    lastLogMessage = message;
    
    if (ui && ui->logOutput) {
        ui->logOutput->append(message);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      lastLogMessage("")
{
    ui->setupUi(this);

    activeWindow = this;

    qInstallMessageHandler(MainWindow::messageHandler);

    connect(ui->startButton, &QPushButton::clicked,
            this, &MainWindow::onStartClicked);
    connect(ui->pauseButton, &QPushButton::clicked,
            this, &MainWindow::onPauseClicked);
    connect(ui->resumeButton, &QPushButton::clicked,
            this, &MainWindow::onResumeClicked);
    connect(ui->stopButton, &QPushButton::clicked,
            this, &MainWindow::onStopClicked);

    // // Configuration button
    // connect(ui->applyConfigButton, &QPushButton::clicked,
    //         this, &MainWindow::on_applyConfigButton_clicked);

    // // Script/event buttons
    // connect(ui->addPassengerScriptButton, &QPushButton::clicked,
    //         this, &MainWindow::on_addPassengerScriptButton_clicked);

    // // Safety event button
    // connect(ui->addSafetyEventButton, &QPushButton::clicked,
    //         this, &MainWindow::on_addSafetyEventButton_clicked);

    // Simulation update signal
    connect(SimulationController::getInstance(), &SimulationController::simulationUpdated,
            this, &MainWindow::onSimulationUpdated);
    
    // Temporarily disable the message handler to prevent duplication
    QtMessageHandler oldHandler = qInstallMessageHandler(nullptr);
    
    appendToLog("Configuring simulation with 5 floors, 2 elevators, 3 passengers.");
    SimulationController::getInstance()->configure(5, 2, 3);
    
    qInstallMessageHandler(oldHandler);

    refreshFloorGrid();
}

MainWindow::~MainWindow()
{
    qInstallMessageHandler(nullptr);
    activeWindow = nullptr;
    delete ui;
}

void MainWindow::on_applyConfigButton_clicked()
{
    int floors = ui->floorsSpinBox->value();
    int elevs  = ui->elevatorsSpinBox->value();
    int pass   = ui->passengersSpinBox->value();

    QtMessageHandler oldHandler = qInstallMessageHandler(nullptr);
    
    QString message = QString("Configuring simulation with %1 floors, %2 elevators, %3 passengers.")
                     .arg(floors).arg(elevs).arg(pass);
    appendToLog(message);

    SimulationController::getInstance()->configure(floors, elevs, pass);
    
    qInstallMessageHandler(oldHandler);
    
    refreshFloorGrid();
}

void MainWindow::on_addPassengerScriptButton_clicked()
{
    QString text = ui->scriptTextEdit->toPlainText().trimmed();
    if (text.isEmpty()) {
        appendToLog("No passenger script entered!");
        return;
    }

    QStringList parts = text.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 4) {
        appendToLog("Script format: <time> <passengerIDs> <startFloor> <destFloor>");
        return;
    }

    QStringList passengerIDs = parts[1].split(",", Qt::SkipEmptyParts);
    QList<int> pIDs;
    for(const QString& id : passengerIDs) {
        pIDs.append(id.toInt());
    }

    QtMessageHandler oldHandler = qInstallMessageHandler(nullptr);

    for(int pID : pIDs) {
        PassengerScript ps;
        ps.time       = parts[0].toInt();
        ps.passengerID= pID;
        ps.startFloor = parts[2].toInt();
        ps.destFloor  = parts[3].toInt();

        passengerScripts.append(ps);

        QString line = QString("time=%1 passengerID=%2 start=%3 dest=%4")
                       .arg(ps.time).arg(ps.passengerID).arg(ps.startFloor).arg(ps.destFloor);

        ui->scriptListOutput->appendPlainText(line);
        QString message = QString("Added Passenger Script: time=%1 passengerID=%2 start=%3 dest=%4")
                    .arg(ps.time).arg(ps.passengerID).arg(ps.startFloor).arg(ps.destFloor);
        appendToLog(message);
    }
    
    qInstallMessageHandler(oldHandler);
}

void MainWindow::on_addSafetyEventButton_clicked()
{
    QString text = ui->eventTextEdit->toPlainText().trimmed();
    if(text.isEmpty()){
        appendToLog("No safety event entered!");
        return;
    }

    QStringList parts = text.split(" ", Qt::SkipEmptyParts);
    if(parts.size() < 2){
        appendToLog("Event format: <time> <type>");
        return;
    }

    SafetyEvent se;
    se.time = parts[0].toInt();
    se.type = parts[1];

    safetyEvents.append(se);

    QString line = QString("time=%1 type=%2").arg(se.time).arg(se.type);
    ui->eventListOutput->appendPlainText(line);

    appendToLog(QString("Added Safety Event: %1").arg(line));
}

void MainWindow::onStartClicked()
{
    appendToLog("Simulation STARTING...");
    SimulationController::getInstance()->startSimulation();
}

void MainWindow::onPauseClicked()
{
    appendToLog("Simulation PAUSED.");
    SimulationController::getInstance()->pauseSimulation();
}

void MainWindow::onResumeClicked()
{
    appendToLog("Simulation RESUMED.");
    SimulationController::getInstance()->resumeSimulation();
}

void MainWindow::onStopClicked()
{
    appendToLog("Simulation STOPPED.");
    SimulationController::getInstance()->stopSimulation();
}


void MainWindow::onSimulationUpdated()
{
    int simTime = SimulationController::getInstance()->getCurrentTime();
    ui->timeLabel->setText(QString("Time: %1").arg(simTime));

    for(auto &ps : passengerScripts){
        if(ps.time == simTime) {
            QString message = QString("PassengerScript triggered: t=%1 passengerID=%2 start=%3 dest=%4")
                .arg(ps.time).arg(ps.passengerID).arg(ps.startFloor).arg(ps.destFloor);
            appendToLog(message);
            
            SimulationController::getInstance()->callElevator(
                ps.passengerID,
                ps.startFloor,
                ps.destFloor
            );
        }
    }

    for(auto &se : safetyEvents){
        if(se.time == simTime){
            QString message = QString("SafetyEvent triggered: t=%1, type=%2")
                .arg(se.time).arg(se.type);
            appendToLog(message);
            
            QString eventType = se.type.toLower();
            if(eventType == "fire"){
                SimulationController::getInstance()->triggerFireAlarm();
            }
            else if(eventType == "stop"){
                SimulationController::getInstance()->stopSimulation();
            }
            else if(eventType == "help"){
                SimulationController::getInstance()->triggerHelpAlarm();
            }
            else if(eventType == "overload"){
                SimulationController::getInstance()->triggerOverloadAlarm();
            }
        }
    }

    refreshFloorGrid();
}

void MainWindow::refreshFloorGrid()
{
    QLayout* frameLayout = ui->floorGridFrame->layout();
    if(frameLayout){
        QLayoutItem* child;
        while((child = frameLayout->takeAt(0)) != nullptr){
            if(child->widget()){
                delete child->widget();
            }
            delete child;
        }
    }

    QGridLayout* grid = qobject_cast<QGridLayout*>(ui->floorGridFrame->layout());
    if(!grid){
        grid = new QGridLayout();
        ui->floorGridFrame->setLayout(grid);
    }

    auto ctrl = SimulationController::getInstance();
    auto &floors = ctrl->getFloors();
    auto &elevators = ctrl->getElevators();

    std::vector<Floor*> sortedFloors(floors.begin(), floors.end());
    std::sort(sortedFloors.begin(), sortedFloors.end(), [](Floor* a, Floor* b){
        return a->getFloorNumber() > b->getFloorNumber();
    });

    int row = 0;
    for(auto* f : sortedFloors) {
        QString floorLabelText = QString("Floor %1: ").arg(f->getFloorNumber());

        QStringList eList;
        for(auto* e : elevators){
            if(e->getCurrentFloor() == f->getFloorNumber()){
                int eID = e->property("myID").toInt();
                eList << QString("E%1").arg(eID);
            }
        }
        if(eList.isEmpty()){
            floorLabelText += "(No elevator)";
        } else {
            floorLabelText += eList.join(", ");
        }

        QLabel* lbl = new QLabel(floorLabelText, ui->floorGridFrame);
        grid->addWidget(lbl, row, 0);
        row++;
    }
}
