#include "Floor.h"
#include <QDebug>

Floor::Floor(int number, QObject* parent)
    : QObject(parent),
      floorNumber(number),
      upButtonLit(false),
      downButtonLit(false)
{
}

void Floor::pressUp()
{
    upButtonLit = true;
    emit floorButtonPressed(floorNumber, true);
    qDebug() << "Floor" << floorNumber << ": Up pressed.";
}

void Floor::pressDown()
{
    downButtonLit = true;
    emit floorButtonPressed(floorNumber, false);
    qDebug() << "Floor" << floorNumber << ": Down pressed.";
}

void Floor::resetButtons()
{
    upButtonLit = false;
    downButtonLit = false;
}
