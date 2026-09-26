#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <vector>

class Elevator;
class Passenger;
class Floor;
class IElevatorDispatchStrategy;

class SimulationController : public QObject
{
    Q_OBJECT

private:
    static SimulationController* instance;
    explicit SimulationController(QObject* parent = nullptr);

    std::vector<Elevator*> elevators;
    std::vector<Passenger*> passengers;
    std::vector<Floor*> floors;

    IElevatorDispatchStrategy* dispatchStrategy;
    bool isRunning;
    QTimer* simulationTimer;
    int currentTime;

    Elevator* pickElevator(int startFloor);

public:
    static SimulationController* getInstance();

    void configure(int floorCount, int elevatorCount, int passengerCount);

    void startSimulation();
    void pauseSimulation();
    void resumeSimulation();
    void stopSimulation();

    // Safety scenarios
    void triggerFireAlarm();
    void triggerHelpAlarm();
    void triggerOverloadAlarm();

    void callElevator(int passengerID, int startFloor, int destFloor);

    int getCurrentTime() const { return currentTime; }

    std::vector<Elevator*>& getElevators() { return elevators; }
    std::vector<Floor*>& getFloors() { return floors; }

signals:
    void simulationUpdated();
    void elevatorStateChanged(Elevator* e);

private slots:
    void advanceSimulation();
};

#endif
