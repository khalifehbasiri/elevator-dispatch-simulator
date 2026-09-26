#ifndef NEARESTCARSTRATEGY_H
#define NEARESTCARSTRATEGY_H

#include "IElevatorDispatchStrategy.h"

class NearestCarStrategy : public IElevatorDispatchStrategy
{
public:
    ~NearestCarStrategy() override = default;
    Elevator* selectElevator(const std::vector<Elevator*>& elevators,
                             int requestedFloor,
                             bool goingUp) override;
};

#endif // NEARESTCARSTRATEGY_H
