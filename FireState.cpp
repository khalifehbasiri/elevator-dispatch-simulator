#include "FireState.h"
#include "Elevator.h"
#include <QDebug>
#include <QtGlobal>

void FireState::requestMove(Elevator* elevator, int floor)
{
    Q_UNUSED(floor);

    if (elevator->getCurrentFloor() != 0) {
        if (elevator->isDoorOpen()) {
            elevator->closeDoor();
        }

        if (elevator->getCurrentFloor() > 0) {
            elevator->moveToFloor(elevator->getCurrentFloor() - 1);
        }
    } else {
        if (!elevator->isDoorOpen()) {
            elevator->openDoor();
        }
    }
    qDebug() << "FireState: Moving to safe floor (0). Current floor:" << elevator->getCurrentFloor();
}
