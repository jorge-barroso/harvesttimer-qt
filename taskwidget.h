#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include "task.h"

namespace Ui
{
	class TaskWidget;
}

class TaskWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit TaskWidget(const Task& task, QWidget* parent);

		~TaskWidget() override;

		void stop();

		void addMinute();

private slots:

        void on_delete_button_clicked();

        void on_stop_resume_button_clicked();

        void on_add_favourite_button_clicked();

        void on_edit_button_clicked();

private:
		Ui::TaskWidget* ui;
};

#endif // TASKWIDGET_H
