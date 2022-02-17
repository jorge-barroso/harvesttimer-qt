//
// Created by jorge on 17/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_PROJECT_H
#define HARVESTTIMER_QT_CMAKE_PROJECT_H


#include <QString>
#include "harvesttask.h"

struct Project
{
	QString company_name;
	QString project_name;
	std::vector<HarvestTask> task;
};


#endif //HARVESTTIMER_QT_CMAKE_PROJECT_H
