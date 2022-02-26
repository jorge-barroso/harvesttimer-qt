#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QIcon>
#include "task.h"

namespace Ui
{
	class TaskWidget;
}

class TaskWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit TaskWidget(const Task* task, QWidget* parent);

		~TaskWidget() override;

		void stop();

		void addMinute();

	signals:
		void task_stopped();

		void task_started();

	private slots:

		void on_delete_button_clicked();

		void on_stop_resume_button_clicked();

		void on_add_favourite_button_clicked();

		void on_edit_button_clicked();

	private:
		Ui::TaskWidget* ui;

		QIcon stop_icon;

		QIcon start_icon;
};

#endif // TASKWIDGET_H
