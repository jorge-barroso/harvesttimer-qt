//
// Created by jorge on 23/02/2022.
//

#include "maputils.h"
#include "tasksscrollarea.h"
#include <QApplication>
#include <QLayout>
#include <QMessageBox>

TasksScrollArea::TasksScrollArea(QWidget *widget)
        : CustomScrollArea(widget), timer{QTimer(this)}, runningTask{}, runningTaskWidget{nullptr},
          lookup_date{QDate::currentDate()} {
    timer.setInterval(timer_seconds * 1000);
}

TasksScrollArea::~TasksScrollArea() {
    delete runningTaskWidget;
    delete runningTask;
    for (const auto &widgets: task_widgets) {
        for (auto *task_widget: widgets.second) {
            delete task_widget;
        }
    }
}

void TasksScrollArea::add_task(Task *task) {
    task->date = lookup_date;
    harvest_handler->add_task(task);
}

void TasksScrollArea::update_task_widgets() {
    clear_task_widgets();

    if (!task_widgets.contains(lookup_date) || task_widgets.at(lookup_date).empty())
        return;

    std::set<TaskWidget *, task_widgets_cmp> task_widgets_vector{task_widgets.at(lookup_date)};
    for (auto task_widget: task_widgets_vector) {
        add_task_widget(task_widget);
    }
}

void TasksScrollArea::update_task_timer() {
    if (runningTaskWidget == nullptr)
        return;

    runningTaskWidget->addMinute();
}

void TasksScrollArea::start_task(const Task *task, TaskWidget *task_widget) {
    // send restart request to harvest
    harvest_handler->start_task(*task);
    // now the request is sent we can proceed to change our UI to a "started" status
    start_task_locally(task, task_widget);
}

void TasksScrollArea::start_task_locally(const Task *task, TaskWidget *task_widget) {
    // If there is a currently running task we want to set_stopped it, because we're only allowed to track one task at a time
    stop_task_locally();

    // Restart timer
    timer.start();
    connect(&timer, &QTimer::timeout, this, &TasksScrollArea::update_task_timer);

    // Save our currently running tasks
    runningTaskWidget = task_widget;
    runningTask = task;
    connect(runningTaskWidget, &TaskWidget::task_stopped, this, &TasksScrollArea::stop_current_task);
}

void TasksScrollArea::edit_task(const Task *task, TaskWidget *task_widget) {
    emit task_to_edit(task, task_widget);
}

void TasksScrollArea::delete_task(const Task *task, TaskWidget *task_widget) {
    // request task deletion to harvest
    harvest_handler->delete_task(*task);

    // delete from our local list
    if (task->started) {
        stop_task_locally();
    }

    std::set<TaskWidget *, task_widgets_cmp> *stored_tasks{&task_widgets.at(lookup_date)};
    std::set<TaskWidget *, task_widgets_cmp>::iterator task_to_erase{stored_tasks->find(task_widget)};
    if (task_to_erase != stored_tasks->end()) {
        delete task_widget;
        stored_tasks->erase(task_to_erase);
    }
    this->update_task_widgets();
}

void TasksScrollArea::stop_current_task() {
    // stopping remotely takes longer due to the network overhead, so we kick it first and then stop locally
    // to try and be as accurate as possible
    // TODO maybe stop locally on success response received instead of always stopping locally
    harvest_handler->stop_task(*runningTask);

    stop_task_locally();
}

void TasksScrollArea::stop_task_locally() {
    if (runningTaskWidget == nullptr)
        return;

    disconnect(runningTaskWidget, &TaskWidget::task_stopped, nullptr, nullptr);
    runningTaskWidget->set_stopped();
    runningTaskWidget = nullptr;

    timer.stop();
}

void TasksScrollArea::set_harvest_handler(HarvestHandler *handler) {
    this->harvest_handler = handler;
}

void TasksScrollArea::set_lookup_date(const QDate &date) {
    lookup_date = date;
}

void TasksScrollArea::task_added(const Task *task) {
    TaskWidget *task_widget{new TaskWidget(const_cast<Task *>(task), this->widget())};
    connect(task_widget, &TaskWidget::task_started, this, &TasksScrollArea::start_task);
    connect(task_widget, &TaskWidget::task_edited, this, &TasksScrollArea::edit_task);
    connect(task_widget, &TaskWidget::task_deleted, this, &TasksScrollArea::delete_task);
    connect(task_widget, &TaskWidget::task_favourited, this, &TasksScrollArea::favourite_task);
    connect(task_widget, &TaskWidget::task_unfavourited, this, &TasksScrollArea::unfavourite_task);
    bool inserted{MapUtils::map_insert_or_create_set(task_widgets, task->date, task_widget)};

    if (!inserted) {
        delete task_widget;
        return;
    }

    if (task->date == lookup_date) {
        this->widget()->layout()->addWidget(task_widget);
    }

    if (!task->started) {
        return;
    }

    start_task_locally(task, task_widget);
}

void TasksScrollArea::favourite_task(const Task *task) {
    emit task_to_favourites(task);
}

void TasksScrollArea::unfavourite_task(const Task *task) {
    emit task_out_of_favourites(task);
}

void TasksScrollArea::uncheck_task_favourite(const Task *task) {
    for (const auto &map_entry: task_widgets) {
        for (auto *task_widget: map_entry.second) {
            if (task_widget->is_task(task)) {
                task_widget->set_unfavourited();
            }
        }
    }
}

void TasksScrollArea::check_task_favourite(const Task *task) {
    for (const auto &map_entry: task_widgets) {
        for (auto *task_widget: map_entry.second) {
            if (task_widget->is_task(task)) {
                task_widget->set_favourited();
            }
        }
    }
}

void TasksScrollArea::task_edited(Task *task, TaskWidget *task_widget) {
    harvest_handler->update_task(task);
    task_widget->update_task(task);
}
