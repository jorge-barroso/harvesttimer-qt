#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTime>
#include <ostream>

enum TaskFields
{
	project_id,
	task_id,
	time_entry_id,
	project_name,
	task_name,
	time_tracked,
	note,
	started,
	date
};

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
		stream << TaskFields::project_id << " " << task.project_id << " " << TaskFields::task_id << " " << task.task_id
			   << " " << TaskFields::time_entry_id << " " << task.time_entry_id << " " << TaskFields::project_name
			   << " " << task.project_name
			   << " " << TaskFields::task_name << " " << task.task_name << " " << TaskFields::time_tracked << " "
			   << task.time_tracked
			   << " " << TaskFields::note << " " << task.note << " " << TaskFields::started << " " << task.started
			   << " " << TaskFields::date << " " << task.date;
		return stream;
	}

	friend QDataStream& operator>>(QDataStream& stream, Task& task)
	{
		TaskFields key;
		while (!stream.atEnd())
		{
			stream >> key;
			switch (key)
			{

				case ::project_id:
					stream >> task.project_id;
					break;
				case ::task_id:
					stream >> task.task_id;
					break;
				case ::time_entry_id:
					stream >> task.time_entry_id;
					break;
				case ::project_name:
					stream >> task.project_name;
					break;
				case ::task_name:
					stream >> task.task_name;
					break;
				case ::time_tracked:
					stream >> task.time_tracked;
					break;
				case ::note:
					stream >> task.note;
					break;
				case ::started:
					stream >> task.started;
					break;
				case ::date:
					stream >> task.date;
					break;
			}
		}
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
