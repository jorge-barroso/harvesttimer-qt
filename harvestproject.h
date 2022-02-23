//
// Created by jorge on 17/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_HARVESTPROJECT_H
#define HARVESTTIMER_QT_CMAKE_HARVESTPROJECT_H


#include <QString>
#include "harvesttask.h"

struct HarvestProject
{
	QString company_name;
	QString project_name;
	qint64 project_id;
	std::vector<HarvestTask> task;
};


#endif //HARVESTTIMER_QT_CMAKE_HARVESTPROJECT_H
