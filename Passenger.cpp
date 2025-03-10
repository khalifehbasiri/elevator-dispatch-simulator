#include "Passenger.h"
#include <QDebug>

Passenger::Passenger(int id, int dest, QObject* parent)
    : QObject(parent),
      passengerID(id),
      destinationFloor(dest),
      currentState("WAITING"),
      hasHelped(false)
{
}

void Passenger::doNextAction(int timeStep)
{
    qDebug() << "Passenger" << passengerID << ": timeStep=" << timeStep << "  no real action";
}

void Passenger::boardElevator()
{
    currentState = "RIDING";
    qDebug() << "Passenger" << passengerID << "boarded elevator, dest=" << destinationFloor;
}

void Passenger::exitElevator()
{
    currentState = "EXITED";
    qDebug() << "Passenger" << passengerID << "exited elevator.";
}

void Passenger::pressHelp()
{
    hasHelped = true;
    qDebug() << "Passenger" << passengerID << "pressed HELP button (not fully implemented).";
}
