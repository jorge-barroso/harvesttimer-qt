//
// Created by jorge on 8/12/22.
//

#include <QLayoutItem>
#include <QLayout>
#include "customscrollarea.h"

CustomScrollArea::CustomScrollArea(QWidget* parent) : QScrollArea(parent){}

void CustomScrollArea::clear_task_widgets() const
{
	QLayoutItem* child;
	QLayout* layout = widget()->layout();
	while ((child = layout->takeAt(0)) != nullptr)
	{
		remove_task_widget(child);
	}
}

void CustomScrollArea::remove_task_widget(QLayoutItem* child) const
{
	child->widget()->hide();
	widget()->layout()->removeItem(child);
}

void CustomScrollArea::add_task_widget(QWidget* task_widget) const
{
	widget()->layout()->addWidget(task_widget);
	task_widget->show();
}