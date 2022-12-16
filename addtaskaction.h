//
// Created by jorge on 16/12/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_ADDTASKACTION_H
#define HARVESTTIMER_QT_CMAKE_ADDTASKACTION_H


#include <QAction>
#include <QSystemTrayIcon>

class AddTaskAction : public QAction
{
public:
    explicit AddTaskAction(QObject *parent);
};


#endif //HARVESTTIMER_QT_CMAKE_ADDTASKACTION_H
