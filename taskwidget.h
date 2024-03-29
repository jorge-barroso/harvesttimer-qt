#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QIcon>
#include "task.h"

namespace Ui {
    class TaskWidget;
}

class TaskWidget : public QWidget {
Q_OBJECT

public:
    explicit TaskWidget(Task *task, QWidget *parent);

    ~TaskWidget() override;

    void set_stopped();

    void addMinute();

    bool is_task(const Task *task) const;

    void set_favourited();

    void set_unfavourited();

    void update_task(Task *new_task);

    const Task* task() const;

signals:

    void task_stopped();

    void task_started(const Task *task, TaskWidget *task_widget);

    void task_edited(const Task *task, TaskWidget *task_widget);

    void task_deleted(const Task *task, TaskWidget *task_widget);

    void task_favourited(const Task *task);

    void task_unfavourited(const Task *task);

private slots:

    void on_delete_button_clicked();

    void on_stop_resume_button_clicked();

    void on_add_favourite_button_clicked();

    void on_edit_button_clicked();

private:
    Ui::TaskWidget *ui;

    Task *m_task;

    void set_started();

    void update_internal_widgets();
};

#endif // TASKWIDGET_H
