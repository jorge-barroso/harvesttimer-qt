//
// Created by jorge on 16/12/22.
//

#include "showorhideaction.h"
#include <QApplication>

ShowOrHideAction::ShowOrHideAction(QObject *parent)
        : QAction(QIcon::fromTheme("view-visible"), QApplication::translate("Menu", "Show/Hide"), parent)
{}
