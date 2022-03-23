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
		, lookup_date{ QDate::currentDate() }
{
	connect(&timer, &QTimer::timeout, this, &TasksScrollArea::update_task_timer);
	timer.setInterval(timer_seconds * 1000);
}

void TasksScrollArea::add_task(Task* task)
{
	TaskWidget* task_widget{ new TaskWidget(task, this->widget()) };
	connect(task_widget, &TaskWidget::task_started, this, &TasksScrollArea::start_task);
	connect(task_widget, &TaskWidget::task_edited, this, &TasksScrollArea::edit_task);
	connect(task_widget, &TaskWidget::task_deleted, this, &TasksScrollArea::delete_task);
	this->widget()->layout()->addWidget(task_widget);
	MapUtils::map_insert_or_create_vector(task_widgets, lookup_date, task_widget);

	if (!task->started)
		return;

	start_task_locally(task, task_widget);
}

void TasksScrollArea::update_task_widgets()
{
	clear_task_widgets();

	if (!task_widgets.contains(lookup_date))
		return;

	std::vector<TaskWidget*> task_widgets_vector{ task_widgets[lookup_date] };
	for (auto& task_widget: task_widgets_vector)
	{
		add_task_widget(task_widget);
	}
}

void TasksScrollArea::clear_task_widgets() const
{
	QLayoutItem* child;
	QLayout* layout = widget()->layout();
	while ((child = layout->takeAt(0)) != nullptr)
	{
		remove_task_widget(child);
	}
}

void TasksScrollArea::remove_task_widget(QLayoutItem* child) const
{
	child->widget()->hide();
	widget()->layout()->removeItem(child);
}

void TasksScrollArea::add_task_widget(TaskWidget*& task_widget) const
{
	widget()->layout()->addWidget(task_widget);
	task_widget->show();
}

void TasksScrollArea::update_task_timer()
{
	if (runningTaskWidget == nullptr)
		return;

	runningTaskWidget->addMinute();
}

void TasksScrollArea::start_task(const Task* task, TaskWidget* task_widget)
{
	// send restart request to harvest
	harvest_handler->start_task(*task);
	// now the request is sent we can proceed to change our UI to a "started" status
	start_task_locally(task, task_widget);
}

void TasksScrollArea::start_task_locally(const Task* task, TaskWidget* task_widget)
{
	stop_current_task();
	// no need to change start/stop button because that's its default state
	runningTaskWidget = task_widget;
	runningTask = task;
	connect(runningTaskWidget, &TaskWidget::task_stopped, this, &TasksScrollArea::stop_current_task);
	timer.start();
}

void TasksScrollArea::edit_task(const Task* task, TaskWidget* task_widget)
{
	// TODO gather the task details, show a pop-up window for the user to edit the details, and then on confirmation send an update request
	stop_current_task();
	// no need to change start/stop button because that's its default state
	runningTaskWidget = task_widget;
	runningTask = task;
	connect(runningTaskWidget, &TaskWidget::task_stopped, this, &TasksScrollArea::stop_current_task);
	timer.start();
}

void TasksScrollArea::delete_task(const bool task_started, TaskWidget* task_widget)
{
	// TODO request task deletion to harvest


	// delete from our local list
	if (task_started)
	{
		stop_task_locally();
	}

	std::vector<TaskWidget*>* stored_tasks{ &task_widgets.at(lookup_date) };
	std::vector<TaskWidget*>::iterator task_to_erase{ std::find(stored_tasks->begin(), stored_tasks->end(), task_widget) };
	if (task_to_erase != stored_tasks->end())
	{
		delete task_widget;
		stored_tasks->erase(task_to_erase);
	}
	this->update_task_widgets();
}

void TasksScrollArea::stop_current_task()
{
	if (runningTaskWidget == nullptr)
		return;

	stop_task_locally();

	harvest_handler->stop_task(*runningTask);
}

void TasksScrollArea::stop_task_locally()
{
	disconnect(runningTaskWidget, &TaskWidget::task_stopped, nullptr, nullptr);
	runningTaskWidget = nullptr;

	timer.stop();
}

void TasksScrollArea::set_harvest_handler(HarvestHandler* handler)
{
	this->harvest_handler = handler;
}

TasksScrollArea::~TasksScrollArea()
{
	delete runningTaskWidget;
	delete runningTask;
	for (const auto& widgets: task_widgets)
	{
		for (auto* task_widget: widgets.second)
		{
			delete task_widget;
		}
	}
}

void TasksScrollArea::set_lookup_date(const QDate& date)
{
	lookup_date = date;
}
