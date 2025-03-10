#ifndef STOPSTATE_H
#define STOPSTATE_H

#include "ElevatorState.h"

class StopState : public ElevatorState
{
public:
    ~StopState() override = default;
    void requestMove(Elevator* elevator, int floor) override;
};

#endif // STOPSTATE_H
