//
// Created by jorge on 16/12/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_QUITACTION_H
#define HARVESTTIMER_QT_CMAKE_QUITACTION_H


#include <QAction>
#include <QSystemTrayIcon>

class QuitAction : public QAction
{
public:
    explicit QuitAction(QObject *parent);
};


#endif //HARVESTTIMER_QT_CMAKE_QUITACTION_H
