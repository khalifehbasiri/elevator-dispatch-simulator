#ifndef IELEVATORDISPATCHSTRATEGY_H
#define IELEVATORDISPATCHSTRATEGY_H

#include <vector>

class Elevator;

class IElevatorDispatchStrategy
{
public:
    virtual ~IElevatorDispatchStrategy() = default;
    virtual Elevator* selectElevator(const std::vector<Elevator*>& elevators,
                                     int requestedFloor,
                                     bool goingUp) = 0;
};

#endif // IELEVATORDISPATCHSTRATEGY_H
