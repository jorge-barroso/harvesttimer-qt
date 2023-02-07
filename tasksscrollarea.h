//
// Created by jorge on 23/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_TASKSSCROLLAREA_H
#define HARVESTTIMER_QT_CMAKE_TASKSSCROLLAREA_H

#include "taskwidget.h"
#include "harvesthandler.h"
#include "customscrollarea.h"
#include <set>
#include <QObject>
#include <QScrollArea>
#include <QLayoutItem>
#include <QTimer>

struct task_widgets_cmp {
    bool operator()(const TaskWidget *const a, const TaskWidget *const b) const {
        bool less{a->task()->time_entry_id < b->task()->time_entry_id};
        return less;
    }
};

class TasksScrollArea : public CustomScrollArea {
Q_OBJECT

public:
    explicit TasksScrollArea(QWidget *widget);

    ~TasksScrollArea() override;

    void update_task_widgets();

    void update_task_timer();

    void stop_current_task();

    void set_harvest_handler(HarvestHandler *handler);

    void set_lookup_date(const QDate &date);

    void task_added(const Task *task);

signals:

    void task_to_favourites(const Task *task);

    void task_out_of_favourites(const Task *task);

    void task_to_edit(const Task *task, TaskWidget *widget);

public slots:

    void add_task(Task *task);

    void uncheck_task_favourite(const Task *task);

    void check_task_favourite(const Task *task);

    void task_edited(Task *task, TaskWidget *task_widget);

private slots:

    void edit_task(const Task *task, TaskWidget *task_widget);

    void start_task(const Task *task, TaskWidget *task_widget);

    void delete_task(const Task *task, TaskWidget *task_widget);

    void favourite_task(const Task *task);

    void unfavourite_task(const Task *task);

private:
    QDate lookup_date;
    std::map<QDate, std::set<TaskWidget *, task_widgets_cmp>> task_widgets;

    TaskWidget *runningTaskWidget;
    const Task *runningTask;

    QTimer timer;
    const int timer_seconds{60};

    HarvestHandler *harvest_handler;

    void start_task_locally(const Task *task, TaskWidget *task_widget);

    void stop_task_locally();
};

#endif //HARVESTTIMER_QT_CMAKE_TASKSSCROLLAREA_H
