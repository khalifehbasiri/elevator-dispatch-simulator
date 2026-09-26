#ifndef ELEVATORSTATE_H
#define ELEVATORSTATE_H

class Elevator;

class ElevatorState
{
public:
    virtual ~ElevatorState() = default;
    virtual void requestMove(Elevator* elevator, int floor) = 0;
};

#endif // ELEVATORSTATE_H
