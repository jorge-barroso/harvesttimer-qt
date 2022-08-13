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
	public:
		explicit FavouritesScrollArea(QWidget* parent);

		~FavouritesScrollArea() override;

		void add_favourite(const Task* task);

		void remove_favourite_task(const Task* task);

	private slots:

		void remove_favourite_widget(const FavouriteWidget* favourite_widget);

	private:
		std::vector<const FavouriteWidget*> favourite_widgets;

		void update_favourite_widgets();

		void remove_and_update(std::vector<const FavouriteWidget*>::iterator& favourite_widgets_iter);
};


#endif //HARVESTTIMER_QT_CMAKE_FAVOURITESSCROLLAREA_H
