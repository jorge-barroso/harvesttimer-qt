//
// Created by jorge on 16/12/22.
//

#include "quitaction.h"
#include<QApplication>

QuitAction::QuitAction(QObject *parent)
        : QAction(QIcon::fromTheme("application-exit"), QApplication::translate("Menu", "Quit"), parent)
{
    this->setShortcut(QKeySequence::Quit);
    this->setStatusTip(QApplication::translate("Menu", "Closes the application"));
}
