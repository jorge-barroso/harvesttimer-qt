#ifndef FAVOURITE_H
#define FAVOURITE_H

#include <QString>

struct Favourite {
	long long int project_id;
	QString project_name;
	long long int task_id;
	QString task_name;

	bool operator==(const Favourite& rhs) const
	{
		return project_id == rhs.project_id &&
			   task_id == rhs.task_id;
	}

	bool operator!=(const Favourite& rhs) const
	{
		return !(rhs == *this);
	}

};

#endif // FAVOURITE_H
