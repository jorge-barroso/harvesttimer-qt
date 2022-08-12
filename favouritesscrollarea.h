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

		void remove_favourite(const Task* task);

	private:
		std::vector<const FavouriteWidget*> favourite_widgets;

		void update_favourite_widgets();
};


#endif //HARVESTTIMER_QT_CMAKE_FAVOURITESSCROLLAREA_H
