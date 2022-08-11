#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTime>

struct Task {
	long long project_id;
	long long task_id;
	long long time_entry_id;
    QString project_name;
    QString task_name;
    QTime time_tracked;
	QString note;
	bool started{ false };
	QDate date{ QDate::currentDate() };
};

#endif // TASK_H
