//
// Created by jorge on 16/12/22.
//

#include "quitaction.h"
#include<QApplication>

QuitAction::QuitAction(QObject *parent)
        : QAction(QIcon::fromTheme("application-exit"), QApplication::translate("Tray Icon", "Quit"), parent)
{}
