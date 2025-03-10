#ifndef FLOOR_H
#define FLOOR_H

#include <QObject>

class Floor : public QObject
{
    Q_OBJECT

private:
    int floorNumber;
    bool upButtonLit;
    bool downButtonLit;

public:
    explicit Floor(int number, QObject* parent=nullptr);

    void pressUp();
    void pressDown();
    void resetButtons();

    int getFloorNumber() const { return floorNumber; }
    bool isUpButtonLit() const { return upButtonLit; }
    bool isDownButtonLit() const { return downButtonLit; }

signals:
    void floorButtonPressed(int floor, bool up);
};

#endif // FLOOR_H
