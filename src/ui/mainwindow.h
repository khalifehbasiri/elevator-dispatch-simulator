#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QString>

class QComboBox;
class QFrame;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    enum class RunState { Ready, Running, Paused, Stopped };
    struct PassengerScript {
        int time;
        int passengerID;
        int startFloor;
        int destFloor;
    };
    struct SafetyEvent {
        int time;
        QString type;
    };

    void buildUi();
    void applyConfiguration();
    void addPassengerScript();
    void addSafetyEvent();
    void startSimulation();
    void pauseSimulation();
    void resumeSimulation();
    void stopSimulation();
    void onSimulationUpdated();
    void refreshFloorGrid();
    void updateControls();
    void appendToLog(const QString& message);

    RunState runState = RunState::Ready;
    QList<PassengerScript> passengerScripts;
    QList<SafetyEvent> safetyEvents;

    QLabel* statusLabel = nullptr;
    QLabel* timeLabel = nullptr;
    QFrame* floorGridFrame = nullptr;
    QPlainTextEdit* logOutput = nullptr;
    QPlainTextEdit* scriptListOutput = nullptr;
    QPlainTextEdit* eventListOutput = nullptr;
    QSpinBox* floorsSpinBox = nullptr;
    QSpinBox* elevatorsSpinBox = nullptr;
    QSpinBox* passengersSpinBox = nullptr;
    QSpinBox* scriptTimeSpinBox = nullptr;
    QSpinBox* passengerIdSpinBox = nullptr;
    QSpinBox* startFloorSpinBox = nullptr;
    QSpinBox* destFloorSpinBox = nullptr;
    QSpinBox* eventTimeSpinBox = nullptr;
    QComboBox* eventTypeComboBox = nullptr;
    QPushButton* startButton = nullptr;
    QPushButton* pauseButton = nullptr;
    QPushButton* resumeButton = nullptr;
    QPushButton* stopButton = nullptr;
};

#endif
