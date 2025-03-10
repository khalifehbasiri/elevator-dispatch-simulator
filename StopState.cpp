#include "StopState.h"
#include "Elevator.h"
#include <QDebug>

void StopState::requestMove(Elevator* elevator, int floor)
{
    Q_UNUSED(elevator);
    Q_UNUSED(floor);
    qDebug() << "StopState: Elevator is stopped and will not accept any requests.";
}
