//
// Created by jorge on 8/12/22.
//

#include "favouritesscrollarea.h"
#include <QLayout>

FavouritesScrollArea::FavouritesScrollArea(QWidget* parent) : CustomScrollArea(parent)
{}

FavouritesScrollArea::~FavouritesScrollArea()
{
	for (auto favourite_widget: favourite_widgets)
	{
		delete favourite_widget;
	}
}

void FavouritesScrollArea::add_favourite(const Task* task)
{
	const FavouriteWidget* favourite_widget{ new FavouriteWidget(task, this) };
	favourite_widgets.emplace_back(favourite_widget);
	this->widget()->layout()->addWidget(const_cast<FavouriteWidget*>(favourite_widget));
	connect(favourite_widget, &FavouriteWidget::unfavourited_task, this,
			&FavouritesScrollArea::remove_favourite_widget);
}

void FavouritesScrollArea::remove_favourite_widget(const FavouriteWidget* favourite_widget)
{
	auto favourite_widgets_iter{ std::remove(favourite_widgets.begin(), favourite_widgets.end(), favourite_widget) };

	this->remove_and_update(favourite_widgets_iter);
}

void FavouritesScrollArea::remove_favourite_task(const Task* task)
{
	const FavouriteWidget* favourite_widget{ new FavouriteWidget(task, this) };
	const auto predicate{
			[favourite_widget](const FavouriteWidget* favourite_widget2)
			{
				return *favourite_widget == *favourite_widget2;
			}
	};
	auto favourite_widgets_iter{ std::remove_if(favourite_widgets.begin(), favourite_widgets.end(), predicate) };


	this->remove_and_update(favourite_widgets_iter);
}

void FavouritesScrollArea::remove_and_update(std::vector<const FavouriteWidget*>::iterator& favourite_widgets_iter)
{
	if (favourite_widgets_iter != favourite_widgets.end())
	{
		favourite_widgets.erase(favourite_widgets_iter);
		delete *favourite_widgets_iter;
	}

	update_favourite_widgets();
}

void FavouritesScrollArea::update_favourite_widgets()
{
	clear_task_widgets();

	for (auto& favourite_widget: favourite_widgets)
	{
		add_task_widget(const_cast<FavouriteWidget*>(favourite_widget));
	}
}