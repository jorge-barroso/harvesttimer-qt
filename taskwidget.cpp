#include "taskwidget.h"
#include "ui_taskwidget.h"
#include "task.h"
#include <QSizePolicy>
#include <QtWidgets>
#include <QMessageBox>

TaskWidget::TaskWidget(Task* task, QWidget* parent) :
        QWidget(parent),
        ui(new Ui::TaskWidget),
        m_task{task }
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
	ui->project_name_label->setText(m_task->project_name + " (" + m_task->client_name + ")");
	ui->task_name_label->setText(m_task->task_name);
	ui->time_label->setText(m_task->time_tracked.toString("hh:mm"));

	if (m_task->started)
	{
		// It's running
		set_started();
	}
	else
	{
		// It's not running
		set_stopped();
	}

	if(m_task->favourited)
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
    m_task->project_id = new_task->project_id;
    m_task->project_name = new_task->project_name;
    m_task->task_id = new_task->task_id;
    m_task->task_name = new_task->task_name;
    m_task->note = new_task->note;
    m_task->time_tracked = new_task->time_tracked;

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
	QMessageBox::StandardButton reply{ QMessageBox::question(this,
								  QApplication::translate("DeleteTask", "Delete Task"),
								  QApplication::translate("DeleteTask", "Are you sure about deleting this task? This action cannot be undone."),
								  QMessageBox::Yes|QMessageBox::No) };
	if (reply == QMessageBox::Yes) {
		emit task_deleted(m_task, this);
	}
}


void TaskWidget::on_stop_resume_button_clicked()
{
	if (ui->stop_resume_button->isChecked())  // If the button has been checked, it wasn't running
	{
		set_started();
		emit task_started(m_task, this);
	}
	else
	{
		set_stopped();
		emit task_stopped();
	}
}


void TaskWidget::on_add_favourite_button_clicked()
{
	if (!m_task->favourited)
	{
		set_favourited();
		emit task_favourited(m_task);
	}
	else
	{
		set_unfavourited();
		emit task_unfavourited(m_task);
	}

}


void TaskWidget::on_edit_button_clicked()
{
	emit task_edited(m_task, this);
}

void TaskWidget::set_favourited()
{
	ui->add_favourite_button->setChecked(true);
    ui->add_favourite_button->setIcon(QIcon::fromTheme("bookmark-remove"));
    m_task->favourited = true;
}

void TaskWidget::set_unfavourited()
{
	ui->add_favourite_button->setChecked(false);
    ui->add_favourite_button->setIcon(QIcon::fromTheme("bookmark-new-symbolic"));
    m_task->favourited = false;
}

void TaskWidget::set_started()
{
	ui->stop_resume_button->setChecked(true);
    ui->stop_resume_button->setIcon(QIcon::fromTheme("media-playback-stop-symbolic"));
    m_task->started = true;
}

void TaskWidget::set_stopped()
{
	ui->stop_resume_button->setChecked(false);
    ui->stop_resume_button->setIcon(QIcon::fromTheme("media-playback-start-symbolic"));
    m_task->started = false;
}

bool TaskWidget::is_task(const Task* task) const
{
	return task->project_id==this->m_task->project_id &&
			task->task_id==this->m_task->task_id;
}

const Task *TaskWidget::task() const {
    return this->m_task;
}