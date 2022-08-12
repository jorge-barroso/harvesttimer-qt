//
// Created by jorge on 8/12/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_CUSTOMSCROLLAREA_H
#define HARVESTTIMER_QT_CMAKE_CUSTOMSCROLLAREA_H


#include <QScrollArea>
#include <QLayoutItem>

class CustomScrollArea : public QScrollArea
{
	protected:
		explicit CustomScrollArea(QWidget* parent);

		void clear_task_widgets() const;

		void add_task_widget(QWidget* task_widget) const;

	private:
		void remove_task_widget(QLayoutItem* child) const;
};


#endif //HARVESTTIMER_QT_CMAKE_CUSTOMSCROLLAREA_H
