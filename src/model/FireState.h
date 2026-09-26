#ifndef FIRESTATE_H
#define FIRESTATE_H

#include "ElevatorState.h"

class FireState : public ElevatorState
{
public:
    ~FireState() override = default;
    void requestMove(Elevator* elevator, int floor) override;
};

#endif // FIRESTATE_H
