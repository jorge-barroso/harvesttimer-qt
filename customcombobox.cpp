//
// Created by jorge on 07/02/23.
//

#include "customcombobox.h"
#include <QAbstractItemView>

CustomComboBox::CustomComboBox(QWidget *parent) : QComboBox(parent) {
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->view()->setMaximumHeight(parent->height());
    this->view()->window()->setMaximumHeight(parent->height());
}
