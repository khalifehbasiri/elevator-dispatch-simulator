#include "MovingState.h"
#include "Elevator.h"
#include <QDebug>

void MovingState::requestMove(Elevator* elevator, int floor)
{
    Q_UNUSED(elevator);
    Q_UNUSED(floor);
    qDebug() << "MovingState: ignoring new request. Already moving to" << targetFloor;
}
