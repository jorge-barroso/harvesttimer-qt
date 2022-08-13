#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTime>
#include <ostream>

struct Task
{
	long long project_id;
	long long task_id;
	long long time_entry_id;
	QString project_name;
	QString task_name;
	QTime time_tracked;
	QString note;
	bool started{ false };
	QDate date{ QDate::currentDate() };

	friend QTextStream& operator<<(QTextStream& stream, const Task& task)
	{
		stream << task.project_id << "," << task.task_id << "," << task.time_entry_id << "," << task.project_name << ","
			   << task.task_name << "," << task.time_tracked.toString() << "," << task.note << "," << task.started << ","
			   << task.date.toString();

		return stream;
	}

	friend QTextStream& operator>>(QTextStream& stream, Task& task)
	{
		QString contents{ stream.readAll() };
		QStringList values{ contents.split(",") };

		task.project_id = values[0].toLongLong();
		task.task_id = values[1].toLongLong();
		task.time_entry_id = values[2].toLongLong();
		task.project_name = values[3];
		task.task_name = values[4];
		task.time_tracked = QTime::fromString(values[5]);
		task.note = values[6];
		task.started = QVariant(values[7]).toBool();
		task.date = QDate::fromString(values[8]);

		return stream;
	}

	bool operator==(const Task& rhs) const
	{
		return project_id == rhs.project_id &&
			   task_id == rhs.task_id &&
			   time_entry_id == rhs.time_entry_id &&
			   project_name == rhs.project_name &&
			   task_name == rhs.task_name;
	}

	bool operator!=(const Task& rhs) const
	{
		return !(rhs == *this);
	}
};

#endif // TASK_H
