#ifndef OVERLOADEDSTATE_H
#define OVERLOADEDSTATE_H

#include "ElevatorState.h"

class OverloadedState : public ElevatorState
{
public:
    ~OverloadedState() override = default;
    void requestMove(Elevator* elevator, int floor) override;
};

#endif // OVERLOADEDSTATE_H
