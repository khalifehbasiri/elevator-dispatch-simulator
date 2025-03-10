#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static MainWindow* activeWindow;

private:
    Ui::MainWindow *ui;
    QString lastLogMessage;
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

    QList<PassengerScript> passengerScripts;
    QList<SafetyEvent> safetyEvents;

private slots:
    void on_applyConfigButton_clicked();

    // Scripts
    void on_addPassengerScriptButton_clicked();
    void on_addSafetyEventButton_clicked();

    // Simulation Control
    void onStartClicked();
    void onPauseClicked();
    void onResumeClicked();
    void onStopClicked();

    // Called each time simulation updates
    void onSimulationUpdated();

    void refreshFloorGrid();

public slots:
    void appendToLog(const QString& message);
};

#endif // MAINWINDOW_H
