//
// Created by jorge on 8/12/22.
//

#include "favouriteslist.h"
#include <QLayout>

FavouritesList::FavouritesList(QWidget* parent) : QListWidget(parent)
{}

FavouritesList::~FavouritesList()
{
	for (auto favourite_widget: favourite_widgets)
	{
		delete favourite_widget;
	}
}

void FavouritesList::add_favourite(const Task* task)
{
	if (find_widget_from_task(task) != favourite_widgets.end())
	{
		return;
	}

	auto item{ new QListWidgetItem() };
	const FavouriteWidget* favourite_widget{ new FavouriteWidget(task, this) };
	item->setSizeHint(favourite_widget->sizeHint());
	widget_item_map.emplace(favourite_widget, item);
	favourite_widgets.emplace_back(item);


	this->addItem(item);
	this->setItemWidget(item, const_cast<FavouriteWidget*>(favourite_widget));

	connect(favourite_widget, &FavouriteWidget::unfavourited_task, this, &FavouritesList::remove_favourite_widget);
	connect(favourite_widget, &FavouriteWidget::favourite_chosen, this, &FavouritesList::favourite_chosen);
}

std::vector<const QListWidgetItem*>::iterator FavouritesList::find_widget_from_task(const Task* task)
{
	const auto find_predicate{
			[task, this](const QListWidgetItem* widget_item)
			{
				auto widget{ dynamic_cast<FavouriteWidget*>(this->itemWidget(const_cast<QListWidgetItem*>(widget_item))) };
				return task == (*widget).task();
			}
	};

	return std::find_if(favourite_widgets.begin(), favourite_widgets.end(), find_predicate);
}

WidgetAndItemMap::iterator FavouritesList::find_from_widget_ptr(const FavouriteWidget* favourite_widget)
{
	return widget_item_map.find(favourite_widget);
//	const auto find_predicate{
//			[favourite_widget, this](const QListWidgetItem* widget_item)
//			{
//				auto widget{ dynamic_cast<FavouriteWidget*>(this->itemWidget(const_cast<QListWidgetItem*>(widget_item))) };
//				return *favourite_widget == *widget;
//			}
//	};
//
//	return std::find_if(favourite_widgets.begin(), favourite_widgets.end(), find_predicate);
}

void FavouritesList::remove_favourite_widget(const FavouriteWidget* favourite_widget, const Task* task)
{
	auto favourites_map_iter{ find_from_widget_ptr(favourite_widget) };
	if(favourites_map_iter == widget_item_map.end())
	{
		return;
	}

	this->remove_and_update((*favourites_map_iter).second);

	emit task_removed(task);
}

void FavouritesList::remove_favourite_task(const Task* task)
{
	const std::vector<const QListWidgetItem*>::iterator find_result{ find_widget_from_task(task) };
	if (find_result == favourite_widgets.end())
	{
		return;
	}

	this->remove_and_update(*find_result);
}

void FavouritesList::remove_and_update(const QListWidgetItem* list_item)
{
	const QWidget* internal_widget{ const_cast<const QWidget*>(this->itemWidget(const_cast<QListWidgetItem*>(list_item))) };
	widget_item_map.erase(dynamic_cast<const FavouriteWidget*>(internal_widget));

	const std::vector<const QListWidgetItem*>::iterator favourite_widget_it{ std::find(favourite_widgets.begin(), favourite_widgets.end(), list_item) };

	delete list_item;

	if(favourite_widget_it != favourite_widgets.end())
	{
		favourite_widgets.erase(favourite_widget_it);
	}
}

void FavouritesList::favourite_chosen(const Task* task)
{
	emit new_task_selected(task);
}
