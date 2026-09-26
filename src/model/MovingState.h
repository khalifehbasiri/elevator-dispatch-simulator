#ifndef MOVINGSTATE_H
#define MOVINGSTATE_H

#include "ElevatorState.h"

class MovingState : public ElevatorState
{
private:
    int targetFloor;
public:
    explicit MovingState(int tFloor) : targetFloor(tFloor) {}
    ~MovingState() override = default;
    void requestMove(Elevator* elevator, int floor) override;
};

#endif // MOVINGSTATE_H
