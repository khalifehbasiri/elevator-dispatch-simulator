#include "NearestCarStrategy.h"
#include "Elevator.h"
#include <cmath>
#include <QDebug>

Elevator* NearestCarStrategy::selectElevator(const std::vector<Elevator*>& elevators,
                                             int requestedFloor,
                                             bool goingUp)
{
    Q_UNUSED(goingUp);
    if(elevators.empty()) return nullptr;

    Elevator* best = elevators[0];
    int bestDist = std::abs(best->getCurrentFloor() - requestedFloor);

    for(auto* e : elevators) {
        int dist = std::abs(e->getCurrentFloor() - requestedFloor);
        if(dist < bestDist){
            bestDist = dist;
            best = e;
        }
    }
    qDebug() << "NearestCarStrategy: returning elevator with distance" << bestDist;
    return best;
}
