#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <QSet>
#include <QQueue>
#include "ElevatorState.h"

struct PassengerRequest {
    int passengerID;
    int startFloor;
    int destFloor;
};

class Elevator : public QObject
{
    Q_OBJECT

    friend class ElevatorState;

private:
    int elevatorID;
    int currentFloor;
    bool doorOpen;
    int capacity;
    int passengerCountInside;
    bool isOverloaded;
    QSet<int> passengersInside;

    ElevatorState* state;

    // new:
    bool hasActiveRequest;
    bool pickedUpPassenger;
    int requestStartFloor;
    int requestDestFloor;
    QQueue<PassengerRequest> pendingRequests;

public:
    explicit Elevator(int id, QObject* parent=nullptr);

    void requestMove(int targetFloor);
    void openDoor();
    void closeDoor();
    void update();

    void increaseLoad(int passengerID);
    void decreaseLoad(int passengerID);

    // Safety scenarios
    void switchToFireState();
    void switchToStopState();
    void handleHelpAlarm();
    void handleOverloadAlarm();

    void moveToFloor(int floor);

    void setRequest(int startFloor, int destFloor, int passengerID);
    void clearRequests();

    void changeState(ElevatorState* newState);

    int getCurrentFloor() const { return currentFloor; }
    bool isDoorOpen() const { return doorOpen; }
    int getPassengerCount() const { return passengerCountInside; }
    int getCapacity() const { return capacity; }
    bool getIsOverloaded() const { return isOverloaded; }
    bool getHasActiveRequest() const { return hasActiveRequest || !pendingRequests.isEmpty(); }
    bool hasPassenger(int passengerID) const { return passengersInside.contains(passengerID); }

signals:
    void arrivedAtFloor(int floor);
    void doorStateChanged(bool isOpen);
    void overloadStateTriggered();
    void helpButtonPressed();
};

#endif
