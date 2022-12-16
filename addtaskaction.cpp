//
// Created by jorge on 16/12/22.
//

#include "addtaskaction.h"
#include <QApplication>

AddTaskAction::AddTaskAction(QObject *parent)
    : QAction(QIcon::fromTheme("list-add"), QApplication::translate("Tray Icon", "Add Task"), parent){}
