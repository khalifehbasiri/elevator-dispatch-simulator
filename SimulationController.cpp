#include "SimulationController.h"
#include "Elevator.h"
#include "Floor.h"
#include "Passenger.h"
#include "IElevatorDispatchStrategy.h"
#include "NearestCarStrategy.h"
#include <cstdlib>
#include <climits>
#include <QDebug>
#include <QVariant>

SimulationController* SimulationController::instance = nullptr;

SimulationController::SimulationController(QObject* parent)
    : QObject(parent),
      dispatchStrategy(new NearestCarStrategy),
      isRunning(false),
      currentTime(0)
{
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &SimulationController::advanceSimulation);
}

SimulationController* SimulationController::getInstance()
{
    if(!instance) {
        instance = new SimulationController();
    }
    return instance;
}

void SimulationController::configure(int floorCount, int elevatorCount, int passengerCount)
{
    for(auto* f : floors) delete f;
    floors.clear();
    for(auto* e : elevators) delete e;
    elevators.clear();
    for(auto* p : passengers) delete p;
    passengers.clear();

    for(int i=0; i<floorCount; i++){
        Floor* f = new Floor(i, this);
        floors.push_back(f);
    }

    for(int i=0; i<elevatorCount; i++){
        Elevator* e = new Elevator(i, this);
        e->setProperty("myID", QVariant(i));
        elevators.push_back(e);
    }

    for(int i=0; i<passengerCount; i++){
        int randomDest = rand() % floorCount;
        Passenger* p = new Passenger(i, randomDest, this);
        passengers.push_back(p);
    }
}

void SimulationController::startSimulation()
{
    if(!isRunning) {
        isRunning = true;
        currentTime = 0;
        simulationTimer->start(1000); // 1 second steps
        qDebug() << "Simulation started.";
    }
}

void SimulationController::pauseSimulation()
{
    if(isRunning) {
        isRunning = false;
        simulationTimer->stop();
        qDebug() << "Simulation paused at time=" << currentTime;
    }
}

void SimulationController::resumeSimulation()
{
    if(!isRunning) {
        isRunning = true;
        simulationTimer->start(1000);
        qDebug() << "Simulation resumed at time=" << currentTime;
    }
}

void SimulationController::stopSimulation()
{
    if(isRunning) {
        simulationTimer->stop();
        isRunning = false;

        for(auto* e : elevators) {
            e->clearRequests();
            e->switchToStopState();
        }
    }
    qDebug() << "Simulation stopped at time=" << currentTime;
}

void SimulationController::triggerFireAlarm()
{
    qDebug() << "Fire alarm triggered!";
    for(auto* e : elevators) {
        e->switchToFireState();
    }
}

void SimulationController::triggerHelpAlarm()
{
    qDebug() << "Help alarm triggered! Contacting building safety...";
    for(auto* e : elevators) {
        e->handleHelpAlarm();
    }
}

void SimulationController::triggerOverloadAlarm()
{
    qDebug() << "Overload alarm triggered! Elevator capacity exceeded.";
    for(auto* e : elevators) {
        e->handleOverloadAlarm();
    }
}

void SimulationController::callElevator(int passengerID, int startFloor, int destFloor)
{
    Elevator* chosen = pickElevator(startFloor);
    if(chosen) {
        chosen->setRequest(startFloor, destFloor, passengerID);
    }
}

Elevator* SimulationController::pickElevator(int startFloor)
{
    if(elevators.empty()) return nullptr;

    Elevator* best = nullptr;
    int bestScore = INT_MAX;

    for(auto* e : elevators) {
        int score = 0;

        if(e->getCurrentFloor() == startFloor) {
            return e;
        }

        if(!e->getHasActiveRequest()) {
            score = std::abs(e->getCurrentFloor() - startFloor);
            if(score < bestScore) {
                bestScore = score;
                best = e;
            }
            continue;
        }

        score = std::abs(e->getCurrentFloor() - startFloor) + 5;
        if(score < bestScore) {
            bestScore = score;
            best = e;
        }
    }

    if(best) return best;

    return elevators.front();
}

void SimulationController::advanceSimulation()
{
    currentTime++;
    qDebug() << "Time step:" << currentTime;

    for(auto* e : elevators) {
        e->update();
    }
    for(auto* p : passengers) {
        p->doNextAction(currentTime);
    }

    emit simulationUpdated();
}
