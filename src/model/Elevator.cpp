#include "Elevator.h"
#include "IdleState.h"
#include "OverloadedState.h"
#include "FireState.h"
#include "StopState.h"
#include <QDebug>

Elevator::Elevator(int id, QObject* parent)
    : QObject(parent),
      elevatorID(id),
      currentFloor(0),
      doorOpen(false),
      capacity(5),
      passengerCountInside(0),
      isOverloaded(false),
      hasActiveRequest(false),
      pickedUpPassenger(false),
      requestStartFloor(-1),
      requestDestFloor(-1),
      state(new IdleState())
{
}


void Elevator::requestMove(int targetFloor)
{
    if(state) {
        state->requestMove(this, targetFloor);
    }
}

void Elevator::openDoor()
{
    if(!doorOpen) {
        doorOpen = true;
        emit doorStateChanged(true);
        qDebug() << "Elevator" << elevatorID << ": doors opened.";
    }
}

void Elevator::closeDoor()
{
    if(doorOpen) {
        doorOpen = false;
        emit doorStateChanged(false);
        qDebug() << "Elevator" << elevatorID << ": doors closed.";
    }
}

void Elevator::update()
{
    if (dynamic_cast<FireState*>(state)) {
        state->requestMove(this, 0);
        return;
    }

    if(!hasActiveRequest && pendingRequests.isEmpty()) {
        qDebug() << "Elevator" << elevatorID << "IDLE at floor=" << currentFloor;
        return;
    }

    // If we don't have an active request but have pending ones, start a new request
    if(!hasActiveRequest && !pendingRequests.isEmpty()) {
        PassengerRequest nextRequest = pendingRequests.head(); // Just peek, don't remove yet
        requestStartFloor = nextRequest.startFloor;
        requestDestFloor = nextRequest.destFloor;
        hasActiveRequest = true;
        pickedUpPassenger = false;
    }

    if(!pickedUpPassenger)
    {
        if(currentFloor == requestStartFloor)
        {
            if(!doorOpen){
                openDoor();
                
                // Count how many passengers are waiting at this floor
                int waitingPassengers = 0;
                QQueue<PassengerRequest> tempQueue = pendingRequests;
                while(!tempQueue.isEmpty() && tempQueue.head().startFloor == currentFloor) {
                    waitingPassengers++;
                    tempQueue.dequeue();
                }

                // If more than capacity passengers are waiting, go into overload mode
                if(waitingPassengers > capacity) {
                    qDebug() << "Elevator" << elevatorID << ": Too many passengers waiting (" 
                            << waitingPassengers << "). Going into overload mode.";
                    handleOverloadAlarm();
                    return;
                }

                // Otherwise, load passengers up to capacity
                while(!pendingRequests.isEmpty() && 
                      pendingRequests.head().startFloor == currentFloor && 
                      passengerCountInside < capacity) {
                    PassengerRequest req = pendingRequests.dequeue();
                    increaseLoad(req.passengerID);
                }

                if(passengerCountInside > 0) {
                    closeDoor();
                    pickedUpPassenger = true;
                    qDebug() << "Elevator" << elevatorID
                            << ": passengers boarded, heading to" << requestDestFloor
                            << "(Current load: " << passengerCountInside << "/" << capacity << ")";
                }
            }
        }
        else
        {
            if(!doorOpen){
                if(currentFloor < requestStartFloor){
                    currentFloor++;
                } else if(currentFloor > requestStartFloor){
                    currentFloor--;
                }
                emit arrivedAtFloor(currentFloor);
                qDebug() << "Elevator" << elevatorID
                         << "moved to floor=" << currentFloor
                         << " en route to pickup passenger.";
            }
        }
    }
    else
    {
        if(currentFloor == requestDestFloor)
        {
            if(!doorOpen){
                openDoor();

                // Create a copy of the passengers to unload since we'll be modifying the set
                QSet<int> passengersToUnload = passengersInside;
                for(int pid : passengersToUnload) {
                    decreaseLoad(pid);
                }

                closeDoor();
                hasActiveRequest = false;
                pickedUpPassenger = false;
                requestStartFloor = -1;
                requestDestFloor = -1;
                
                // If we have more requests, start processing them
                if(!pendingRequests.isEmpty()) {
                    PassengerRequest nextRequest = pendingRequests.head();
                    requestStartFloor = nextRequest.startFloor;
                    requestDestFloor = nextRequest.destFloor;
                    hasActiveRequest = true;
                    pickedUpPassenger = false;
                }

                qDebug() << "Elevator" << elevatorID
                         << ": request completed, now" 
                         << (pendingRequests.isEmpty() ? "idle." : "processing next request.");
            }
        }
        else
        {
            if(!doorOpen){
                if(currentFloor < requestDestFloor){
                    currentFloor++;
                } else if(currentFloor > requestDestFloor){
                    currentFloor--;
                }
                emit arrivedAtFloor(currentFloor);
                qDebug() << "Elevator" << elevatorID
                         << "moved to floor=" << currentFloor
                         << " en route to destination.";
            }
        }
    }
}



void Elevator::increaseLoad(int passengerID)
{
    if (passengersInside.contains(passengerID)) {
        qDebug() << "Elevator" << elevatorID << ": Passenger" << passengerID << "is already in the elevator!";
        return;
    }

    if (passengerCountInside >= capacity) {
        qDebug() << "Elevator" << elevatorID << ": Cannot board more passengers - at capacity.";
        return;
    }

    passengerCountInside++;
    passengersInside.insert(passengerID);
    qDebug() << "Elevator" << elevatorID << ": Passenger" << passengerID << "boarded. Current load:"
             << passengerCountInside << "/" << capacity;
}

void Elevator::decreaseLoad(int passengerID)
{
    if (!passengersInside.contains(passengerID)) {
        qDebug() << "Elevator" << elevatorID << ": Passenger" << passengerID << "is not in the elevator!";
        return;
    }

    if(passengerCountInside > 0) {
        passengerCountInside--;
        passengersInside.remove(passengerID);
        if (isOverloaded && passengerCountInside <= capacity) {
            isOverloaded = false;
            qDebug() << "Elevator" << elevatorID << ": Load reduced to safe level.";
        }
        qDebug() << "Elevator" << elevatorID << ": Passenger" << passengerID << "departed. Current load:"
                 << passengerCountInside << "/" << capacity;
    }
}

void Elevator::switchToFireState()
{
    qDebug() << "Elevator" << elevatorID << ": switching to FireState!";
    changeState(new FireState());
}

void Elevator::switchToStopState()
{
    qDebug() << "Elevator" << elevatorID << ": switching to StopState!";
    changeState(new StopState());
}

void Elevator::clearRequests()
{
    hasActiveRequest = false;
    pickedUpPassenger = false;
    requestStartFloor = -1;
    requestDestFloor = -1;
    pendingRequests.clear();
    passengersInside.clear();
    passengerCountInside = 0;
    isOverloaded = false;
    qDebug() << "Elevator" << elevatorID << ": cleared all requests.";
}

void Elevator::setRequest(int startFloor, int destFloor, int passengerID)
{
    if (passengersInside.contains(passengerID)) {
        qDebug() << "Elevator" << elevatorID << ": Passenger" << passengerID << "is already in the elevator!";
        return;
    }

    PassengerRequest newRequest;
    newRequest.passengerID = passengerID;
    newRequest.startFloor = startFloor;
    newRequest.destFloor = destFloor;
    
    pendingRequests.enqueue(newRequest);

    if (!hasActiveRequest) {
        requestStartFloor = startFloor;
        requestDestFloor = destFloor;
        hasActiveRequest = true;
        pickedUpPassenger = false;
    }

    qDebug() << "Elevator" << elevatorID << ": new request start="
             << startFloor << "dest=" << destFloor << "for passenger" << passengerID;
}


void Elevator::changeState(ElevatorState* newState)
{
    if(state){
        delete state;
        state = nullptr;
    }
    state = newState;
}

void Elevator::moveToFloor(int floor)
{
    currentFloor = floor;
    emit arrivedAtFloor(currentFloor);
    qDebug() << "Elevator" << elevatorID << "moved to floor=" << currentFloor;
}

void Elevator::handleHelpAlarm()
{
    qDebug() << "Elevator" << elevatorID << ": Help button pressed! Stopping at next floor.";
    emit helpButtonPressed();
    if (!doorOpen) {
        openDoor();
    }
}

void Elevator::handleOverloadAlarm()
{
    isOverloaded = true;
    qDebug() << "Elevator" << elevatorID << ": Overload detected! Capacity exceeded.";
    emit overloadStateTriggered();
    openDoor();
    pickedUpPassenger = false;
    changeState(new OverloadedState());
}
