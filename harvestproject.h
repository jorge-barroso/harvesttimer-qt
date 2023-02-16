//
// Created by jorge on 17/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_HARVESTPROJECT_H
#define HARVESTTIMER_QT_CMAKE_HARVESTPROJECT_H


#include <QString>
#include "harvesttask.h"

struct HarvestProject
{
	QString client_name;
	QString project_name;
	qint64 project_id;
	std::vector<HarvestTask> task;

    [[nodiscard]] QString get_project_label() const {
        if(client_name.isNull() || client_name.isEmpty()) {
            return project_name;
        }

        return QString("%1 (%2)").arg(project_name, client_name);
    }
};


#endif //HARVESTTIMER_QT_CMAKE_HARVESTPROJECT_H
