#ifndef DATELABEL_H
#define DATELABEL_H

#include <QLabel>
#include <QDate>

class DateLabel : public QLabel
{
	Q_OBJECT
	public:
		explicit DateLabel(QWidget* parent = nullptr);

		void resetDate();

		void moveForward();

		void moveBackwards();

		QDate getAppDate();

	private:
		QDate appDate;
		uint8_t daysStep;

		void updateDisplay();

		QLocale locale;

	signals:

};

#endif // DATELABEL_H
