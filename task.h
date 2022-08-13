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

	friend QDataStream& operator<<(QDataStream& stream, const Task& task)
	{
		stream << task.project_id << " " << task.task_id << " " << task.time_entry_id << " " << task.project_name << " "
			   << task.task_name << " " << task.time_tracked << " " << task.note << " " << task.started << " "
			   << task.date;

		return stream;
	}

	friend QDataStream& operator>>(QDataStream& stream, Task& task)
	{
		stream >> task.project_id >> task.task_id >> task.time_entry_id >> task.project_name
			   >> task.task_name >> task.time_tracked >> task.note >> task.started >> task.date;

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
