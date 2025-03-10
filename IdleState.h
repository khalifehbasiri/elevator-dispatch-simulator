#ifndef IDLESTATE_H
#define IDLESTATE_H

#include "ElevatorState.h"

class IdleState : public ElevatorState
{
public:
    ~IdleState() override = default;
    void requestMove(Elevator* elevator, int floor) override;
};

#endif // IDLESTATE_H
