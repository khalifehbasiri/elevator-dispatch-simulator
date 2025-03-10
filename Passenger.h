#ifndef PASSENGER_H
#define PASSENGER_H

#include <QObject>
#include <string>

class Passenger : public QObject
{
    Q_OBJECT

private:
    int passengerID;
    int destinationFloor;
    std::string currentState;
    bool hasHelped;

public:
    explicit Passenger(int id, int dest, QObject* parent=nullptr);

    void doNextAction(int timeStep);
    void boardElevator();
    void exitElevator();
    void pressHelp();

    int getPassengerID() const { return passengerID; }
    const std::string& getCurrentState() const { return currentState; }
};

#endif // PASSENGER_H
