#include "OverloadedState.h"
#include "Elevator.h"
#include <QDebug>

void OverloadedState::requestMove(Elevator* elevator, int floor)
{
    Q_UNUSED(elevator);
    Q_UNUSED(floor);
    qDebug() << "OverloadedState: Elevator won't move until load is reduced!";
}
