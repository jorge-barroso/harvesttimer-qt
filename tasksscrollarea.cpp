//
// Created by jorge on 23/02/2022.
//

#include "maputils.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "tasksscrollarea.h"
#include <QLayout>

TasksScrollArea::TasksScrollArea(QWidget* widget)
		: QScrollArea(widget)
		, timer{ QTimer(this) }
		, runningTaskWidget{ nullptr }
{
	connect(&timer, &QTimer::timeout, this, &TasksScrollArea::updateTaskTimer);
	timer.setInterval(timer_seconds * 1000);
}

void TasksScrollArea::addTask(const Task& task, const QDate& app_date)
{
	TaskWidget* task_widget{ new TaskWidget(task, this->widget()) };
	this->widget()->layout()->addWidget(task_widget);
	MapUtils::map_insert_or_create_vector(task_widgets, app_date, task_widget);

	if (task.started)
	{
		if(runningTaskWidget != nullptr)
			runningTaskWidget->stop();

		// no need to run start because that's the default state
		runningTaskWidget = task_widget;
	}

	timer.start();
}

void TasksScrollArea::updateTaskWidgets(QDate date)
{
	QLayoutItem* child;
	QLayout* layout = this->widget()->layout();
	while ((child = layout->takeAt(0)) != nullptr)
	{
		child->widget()->hide();
		this->widget()->layout()->removeItem(child);
	}

	if (!task_widgets.contains(date))
		return;

	std::vector<TaskWidget*> task_widgets_vector{ task_widgets[date] };
	for (auto& task_widget: task_widgets_vector)
	{
		this->widget()->layout()->addWidget(task_widget);
		task_widget->show();
	}
}

void TasksScrollArea::updateTaskTimer()
{
	if (runningTaskWidget == nullptr)
		return;

	runningTaskWidget->addMinute();
}

void TasksScrollArea::stopCurrentTask()
{
	if (runningTaskWidget == nullptr)
		return;

	runningTaskWidget = nullptr;
	timer.stop();
}

TasksScrollArea::~TasksScrollArea()
= default;
