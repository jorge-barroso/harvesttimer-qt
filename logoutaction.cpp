//
// Created by jorge on 01/02/23.
//

#include "logoutaction.h"
#include<QApplication>

LogoutAction::LogoutAction(QObject *parent)
    : QAction(QIcon::fromTheme("system-log-out"), QApplication::translate("Menu", "Logout"), parent) {
    this->setStatusTip(QApplication::translate("Menu", "Logs you out from Harvest and closes the application"));
}
