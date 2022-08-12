//
// Created by jorge on 8/12/22.
//

#include "favouritesscrollarea.h"
#include <QLayout>

FavouritesScrollArea::FavouritesScrollArea(QWidget* parent) : CustomScrollArea(parent)
{}

FavouritesScrollArea::~FavouritesScrollArea()
{
	for(auto favourite_widget : favourite_widgets)
	{
		delete favourite_widget;
	}
}

void FavouritesScrollArea::add_favourite(const Task* task)
{
	const FavouriteWidget* favourite_widget{ new FavouriteWidget(task, this) };
	favourite_widgets.emplace_back(favourite_widget);
	this->widget()->layout()->addWidget(const_cast<FavouriteWidget*>(favourite_widget));
}

void FavouritesScrollArea::remove_favourite(const Task* task)
{
	const FavouriteWidget* favourite_widget{ new FavouriteWidget(task, this) };
	const auto predicate{
			[favourite_widget](const FavouriteWidget* favourite_widget2){
				return *favourite_widget==*favourite_widget2;
			}
	};
	std::remove_if(favourite_widgets.begin(), favourite_widgets.end(), predicate);

	this->update_favourite_widgets();
}

void FavouritesScrollArea::update_favourite_widgets()
{
	clear_task_widgets();

	for (auto& favourite_widget: favourite_widgets)
	{
		add_task_widget(const_cast<FavouriteWidget*>(favourite_widget));
	}
}