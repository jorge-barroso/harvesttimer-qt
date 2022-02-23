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

private:
		Ui::TaskWidget* ui;
};

#endif // TASKWIDGET_H
