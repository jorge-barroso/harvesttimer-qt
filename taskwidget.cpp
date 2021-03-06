#include "taskwidget.h"
#include "ui_taskwidget.h"
#include "task.h"
#include <QSizePolicy>

// TODO edit to allow changing all task details, both in widget and on the system
TaskWidget::TaskWidget(Task* task, QWidget* parent) :
		QWidget(parent),
		ui(new Ui::TaskWidget),
		stop_icon(),
		start_icon(),
		task{ task }
{
	ui->setupUi(this);
	ui->organization_label->setText("");
	ui->project_name_label->setText(task->project_name);
	ui->task_name_label->setText(task->task_name);
	ui->time_label->setText(task->time_tracked.toString("hh:mm"));

	stop_icon.addFile(QString::fromUtf8(":/icons/resources/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
	start_icon.addFile(QString::fromUtf8(":/icons/resources/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);

	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

	if (task->time_tracked.hour() + task->time_tracked.minute() != 0)
	{
		// It's not running
		set_stopped();
	}
	else
	{
		// It's running
		set_started();
	}
}

TaskWidget::~TaskWidget()
{
	delete ui;
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
	emit task_deleted(task, this);
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

}


void TaskWidget::on_edit_button_clicked()
{
	emit task_edited(task, this);
}

void TaskWidget::set_started()
{
	ui->stop_resume_button->setIcon(QIcon(stop_icon));
	ui->stop_resume_button->setChecked(true);
	task->started = true;
}

void TaskWidget::set_stopped()
{
	ui->stop_resume_button->setIcon(QIcon(start_icon));
	ui->stop_resume_button->setChecked(false);
	task->started = false;
}
