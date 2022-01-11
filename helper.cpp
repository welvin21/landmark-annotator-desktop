#include "helper.h"

QString Helper::getCurrentDateTimeString() {
    QDateTime dateTime = dateTime.currentDateTime();
    QString currentDateTimeString = dateTime.toString("yyyy-MM-dd_HHmmss");

    return currentDateTimeString;
}