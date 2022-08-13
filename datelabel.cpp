#include "datelabel.h"

DateLabel::DateLabel(QWidget *parent)
    : QLabel{parent},
      daysStep{1},
	  locale()
{
    this->appDate = QDate::currentDate();
}


void DateLabel::resetDate() {
    this->appDate = QDate::currentDate();
    this->updateDisplay();
}

void DateLabel::moveForward() {
    this->appDate = this->appDate.addDays(this->daysStep);
    this->updateDisplay();
}

void DateLabel::moveBackwards() {
    this->appDate = this->appDate.addDays(-this->daysStep);
    this->updateDisplay();
}

QDate DateLabel::getAppDate() {
    return this->appDate;
}

void DateLabel::updateDisplay() {
	QString localisedDate{locale.toString(this->appDate)};
    this->setText(localisedDate);
}
