//
// Created by jorge on 23/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_TASKSSCROLLAREA_H
#define HARVESTTIMER_QT_CMAKE_TASKSSCROLLAREA_H

#include <QObject>
#include <QScrollArea>
#include "taskwidget.h"
#include "harvesthandler.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class TasksScrollArea;
}
QT_END_NAMESPACE

class TasksScrollArea : public QScrollArea
{
	Q_OBJECT

	public:
		explicit TasksScrollArea(QWidget* widget);

		~TasksScrollArea() override;

		void addTask(Task* task, const QDate& app_date);

		void updateTaskWidgets(QDate date);

		void updateTaskTimer();

		void stopCurrentTask();

		void setHarvestHandler(HarvestHandler* handler);

	private:
		std::map<QDate, std::vector<TaskWidget*>> task_widgets;

		TaskWidget* runningTaskWidget;
		const Task* runningTask;

		QTimer timer;
		const int timer_seconds { 60 };

		HarvestHandler* harvest_handler;
};


#endif //HARVESTTIMER_QT_CMAKE_TASKSSCROLLAREA_H