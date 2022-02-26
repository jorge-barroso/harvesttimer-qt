#include "taskwidget.h"
#include "ui_taskwidget.h"
#include "task.h"
#include <QSizePolicy>

// TODO edit to allow changing all task details, both in widget and on the system
TaskWidget::TaskWidget(const Task* task, QWidget* parent) :
		QWidget(parent),
		ui(new Ui::TaskWidget),
		stop_icon(),
		start_icon()
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
		// TODO change icon to "play" icon
		ui->stop_resume_button->setIcon(QIcon());
	}
}

TaskWidget::~TaskWidget()
{
	delete ui;
}

void TaskWidget::stop()
{
	// TODO change icon
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

}


void TaskWidget::on_stop_resume_button_clicked()
{
	if (ui->stop_resume_button->isChecked())
	{
		ui->stop_resume_button->setIcon(stop_icon);
		// TODO start
		// TODO emit signal for tasksscrollarea to stop currently running task (if needed), start this one, and update variables
	}
	else
	{
		ui->stop_resume_button->setIcon(start_icon);
		emit task_stopped();
	}
}


void TaskWidget::on_add_favourite_button_clicked()
{

}


void TaskWidget::on_edit_button_clicked()
{

}

