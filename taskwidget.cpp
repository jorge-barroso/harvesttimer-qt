#include "taskwidget.h"
#include "ui_taskwidget.h"
#include "task.h"
#include <QSizePolicy>
#include <QtWidgets>
#include <QMessageBox>

TaskWidget::TaskWidget(Task* task, QWidget* parent) :
		QWidget(parent),
		ui(new Ui::TaskWidget),
		task{ task }
{
	ui->setupUi(this);

	this->update_internal_widgets();
}


TaskWidget::~TaskWidget()
{
	delete ui;
}

void TaskWidget::update_internal_widgets()
{
	ui->project_name_label->setText(task->project_name);
	ui->task_name_label->setText(task->task_name);
	ui->time_label->setText(task->time_tracked.toString("hh:mm"));

	if (task->started)
	{
		// It's running
		set_started();
	}
	else
	{
		// It's not running
		set_stopped();
	}

	if(task->favourited)
	{
		set_favourited();
	}
	else
	{
		set_unfavourited();
	}
}

void TaskWidget::update_task(Task* new_task)
{
	task->project_id = new_task->project_id;
	task->project_name = new_task->project_name;
	task->task_id = new_task->task_id;
	task->task_name = new_task->task_name;
	task->note = new_task->note;
	task->time_tracked = new_task->time_tracked;

	delete new_task;

	update_internal_widgets();

}

void TaskWidget::addMinute()
{
	QStringList timeComponents{ ui->time_label->text().split(":") };

	int hours{ timeComponents[0].toInt() };
	int minutes{ timeComponents[1].toInt() };
	if (minutes == 59)
	{
		minutes = 0;
		hours += 1;
	}
	else
	{
		minutes += 1;
	}

	ui->time_label->setText(QTime(hours, minutes).toString("hh:mm"));
}


void TaskWidget::on_delete_button_clicked()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this,
								  QApplication::translate("DeleteTask", "Delete Task"),
								  QApplication::translate("DeleteTask", "Are you sure about deleting this task? This action cannot be undone."),
								  QMessageBox::Yes|QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		emit task_deleted(task, this);
	}
}


void TaskWidget::on_stop_resume_button_clicked()
{
	if (ui->stop_resume_button->isChecked())  // If the button has been checked, it wasn't running
	{
		set_started();
		emit task_started(task, this);
	}
	else
	{
		set_stopped();
		emit task_stopped();
	}
}


void TaskWidget::on_add_favourite_button_clicked()
{
	if (!task->favourited)
	{
		set_favourited();
		emit task_favourited(task);
	}
	else
	{
		set_unfavourited();
		emit task_unfavourited(task);
	}

}


void TaskWidget::on_edit_button_clicked()
{
	emit task_edited(task, this);
}

void TaskWidget::set_favourited()
{
	ui->add_favourite_button->setChecked(true);
	task->favourited = true;
}

void TaskWidget::set_unfavourited()
{
	ui->add_favourite_button->setChecked(false);
	task->favourited = false;
}

void TaskWidget::set_started()
{
	ui->stop_resume_button->setChecked(true);
	task->started = true;
}

void TaskWidget::set_stopped()
{
	ui->stop_resume_button->setChecked(false);
	task->started = false;
}

bool TaskWidget::is_task(const Task* task) const
{
	return task->project_id==this->task->project_id &&
			task->task_id==this->task->task_id;
}
