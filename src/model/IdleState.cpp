#include "IdleState.h"
#include "Elevator.h"
#include "MovingState.h"
#include <QDebug>

void IdleState::requestMove(Elevator* elevator, int floor)
{
    elevator->closeDoor();
    elevator->changeState(new MovingState(floor));
    qDebug() << "Elevator switching from IdleState to MovingState, target floor:" << floor;
}
