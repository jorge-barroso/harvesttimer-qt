#ifndef FAVOURITEWIDGET_H
#define FAVOURITEWIDGET_H

#include <QDialog>
#include "task.h"

namespace Ui
{
	class FavouriteWidget;
}

class FavouriteWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit FavouriteWidget(const Task* favourite, QWidget* parent = nullptr);

		~FavouriteWidget() override;

		bool operator==(const FavouriteWidget& rhs) const;

		bool operator!=(const FavouriteWidget& rhs) const;

	private:
		Ui::FavouriteWidget* ui;

		const Task* task;
};

#endif // FAVOURITEWIDGET_H
