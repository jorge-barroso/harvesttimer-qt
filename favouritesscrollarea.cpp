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

	const std::vector<const FavouriteWidget*>::iterator find_result{ find_from_widget_ptr(favourite_widget) };

	if (find_result != favourite_widgets.end())
	{
		return;
	}

	favourite_widgets.emplace_back(favourite_widget);
	this->widget()->layout()->addWidget(const_cast<FavouriteWidget*>(favourite_widget));
	connect(favourite_widget, &FavouriteWidget::unfavourited_task, this, &FavouritesScrollArea::remove_favourite_widget);
	connect(favourite_widget, &FavouriteWidget::favourite_chosen, this, &FavouritesScrollArea::favourite_chosen);
}

std::vector<const FavouriteWidget*>::iterator FavouritesScrollArea::find_widget_from_task(const Task* task)
{
	const FavouriteWidget* favourite_widget{ new FavouriteWidget(task, this) };

	return find_from_widget_ptr(favourite_widget);

}

std::vector<const FavouriteWidget*>::iterator FavouritesScrollArea::find_from_widget_ptr(const FavouriteWidget* favourite_widget)
{
	const auto find_predicate{
			[favourite_widget](const FavouriteWidget* favourite_widget2)
			{
				return *favourite_widget == *favourite_widget2;
			}
	};

	return std::find_if(favourite_widgets.begin(), favourite_widgets.end(), find_predicate);
}

void FavouritesScrollArea::remove_favourite_widget(const FavouriteWidget* favourite_widget)
{
	auto favourite_widgets_iter{ std::remove(favourite_widgets.begin(), favourite_widgets.end(), favourite_widget) };

	this->remove_and_update(favourite_widgets_iter);
}

void FavouritesScrollArea::remove_favourite_task(const Task* task)
{
	const std::vector<const FavouriteWidget*>::iterator find_result{ find_widget_from_task(task) };

	this->remove_and_update(find_result);
}

void FavouritesScrollArea::remove_and_update(const std::vector<const FavouriteWidget*>::iterator& favourite_widgets_iter)
{
	if (favourite_widgets_iter == favourite_widgets.end())
	{
		return;
	}
	favourite_widgets.erase(favourite_widgets_iter);

	update_favourite_widgets();

	const FavouriteWidget* deleted_widget{ *favourite_widgets_iter };
	delete deleted_widget;
}

void FavouritesScrollArea::update_favourite_widgets()
{
	clear_task_widgets();

	for (auto& favourite_widget: favourite_widgets)
	{
		add_task_widget(const_cast<FavouriteWidget*>(favourite_widget));
	}
}

void FavouritesScrollArea::favourite_chosen(const Task* task)
{
	emit new_task_selected(task);
}
