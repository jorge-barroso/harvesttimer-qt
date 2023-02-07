//
// Created by jorge on 17/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_HARVESTTASK_H
#define HARVESTTIMER_QT_CMAKE_HARVESTTASK_H


#include <QString>

struct HarvestTask
{
	qint64 task_id;
	QString task_name;
    QString client_name;
};


#endif //HARVESTTIMER_QT_CMAKE_HARVESTTASK_H
