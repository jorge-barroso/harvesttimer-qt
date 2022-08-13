//
// Created by jorge on 8/12/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_FAVOURITESSCROLLAREA_H
#define HARVESTTIMER_QT_CMAKE_FAVOURITESSCROLLAREA_H


#include <QScrollArea>
#include "favouritewidget.h"
#include "customscrollarea.h"
#include "task.h"

class FavouritesScrollArea : public CustomScrollArea
{
	Q_OBJECT

	public:
		explicit FavouritesScrollArea(QWidget* parent);

		~FavouritesScrollArea() override;

		void add_favourite(const Task* task);

		void remove_favourite_task(const Task* task);

	signals:

		void new_task_selected(const Task* pTask);

		void task_removed(const Task* task);

	private slots:

		void remove_favourite_widget(const FavouriteWidget* favourite_widget, const Task* task);

		void favourite_chosen(const Task* task);


	private:
		std::vector<const FavouriteWidget*> favourite_widgets;

		void update_favourite_widgets();

		void remove_and_update(const std::vector<const FavouriteWidget*>::iterator& favourite_widgets_iter);

		std::vector<const FavouriteWidget*>::iterator find_widget_from_task(const Task* task);

		std::vector<const FavouriteWidget*>::iterator find_from_widget_ptr(const FavouriteWidget* favourite_widget);
};


#endif //HARVESTTIMER_QT_CMAKE_FAVOURITESSCROLLAREA_H
