//
// Created by jorge on 8/12/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_FAVOURITESLIST_H
#define HARVESTTIMER_QT_CMAKE_FAVOURITESLIST_H


#include <QListWidget>
#include <unordered_map>
#include "favouritewidget.h"
#include "task.h"

using WidgetAndItemMap = std::unordered_map<const FavouriteWidget*, const QListWidgetItem*>;

class FavouritesList : public QListWidget
{
	Q_OBJECT

	public:
		explicit FavouritesList(QWidget* parent);

		~FavouritesList() override;

		void add_favourite(const Task* task);

		void remove_favourite_task(const Task* task);

	signals:

		void new_task_selected(const Task* pTask);

		void task_removed(const Task* task);

	private slots:

		void remove_favourite_widget(const FavouriteWidget* favourite_widget, const Task* task);

		void favourite_chosen(const Task* task);


	private:
		std::vector<const QListWidgetItem*> favourite_widgets;

		WidgetAndItemMap widget_item_map;

		void update_favourite_widgets();

		void remove_and_update(const QListWidgetItem* list_item);

		std::vector<const QListWidgetItem*>::iterator find_widget_from_task(const Task* task);

		WidgetAndItemMap::iterator find_from_widget_ptr(const FavouriteWidget* favourite_widget);
};


#endif //HARVESTTIMER_QT_CMAKE_FAVOURITESLIST_H
