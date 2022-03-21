//
// Created by jorge on 23/02/2022.
//

#include "maputils.h"
#include "ui_mainwindow.h"
#include "tasksscrollarea.h"
#include <QLayout>

TasksScrollArea::TasksScrollArea(QWidget* widget)
		: QScrollArea(widget)
		, timer{ QTimer(this) }
		, runningTaskWidget{ nullptr }
{
	connect(&timer, &QTimer::timeout, this, &TasksScrollArea::update_task_timer);
	timer.setInterval(timer_seconds * 1000);
}

void TasksScrollArea::add_task(Task* task, const QDate& app_date)
{
	TaskWidget* task_widget{ new TaskWidget(task, this->widget()) };
	connect(task_widget, &TaskWidget::task_started, this, &TasksScrollArea::start_task);
	connect(task_widget, &TaskWidget::edit_task, this, &TasksScrollArea::editTask);
	this->widget()->layout()->addWidget(task_widget);
	MapUtils::map_insert_or_create_vector(task_widgets, app_date, task_widget);

	if (!task->started)
		return;

	start_task(task, task_widget);
}

void TasksScrollArea::update_task_widgets(QDate date)
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

void TasksScrollArea::update_task_timer()
{
	if (runningTaskWidget == nullptr)
		return;

	runningTaskWidget->addMinute();
}

void TasksScrollArea::start_task(const Task* task, TaskWidget* task_widget)
{
	stop_current_task();
	// no need to change start/stop button because that's its default state
	runningTaskWidget = task_widget;
	runningTask = task;
	connect(runningTaskWidget, &TaskWidget::task_stopped, this, &TasksScrollArea::stop_current_task);
	timer.start();
}

void TasksScrollArea::stop_current_task()
{
	if (runningTaskWidget == nullptr)
		return;

	disconnect(runningTaskWidget, &TaskWidget::task_stopped, nullptr, nullptr);
	runningTaskWidget = nullptr;

	timer.stop();

	harvest_handler->stop_task(*runningTask);
}

void TasksScrollArea::set_harvest_handler(HarvestHandler* handler)
{
	this->harvest_handler = handler;
}

TasksScrollArea::~TasksScrollArea()
{
	delete runningTaskWidget;
	delete runningTask;
	for (const auto& widgets : task_widgets)
	{
		for (auto* task_widget : widgets.second)
		{
			delete task_widget;
		}
	}
}
