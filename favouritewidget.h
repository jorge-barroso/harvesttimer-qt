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

		[[nodiscard]] const Task* task() const;

		bool operator==(const FavouriteWidget& rhs) const;

		bool operator!=(const FavouriteWidget& rhs) const;

	signals:

		void unfavourited_task(const FavouriteWidget* widget, const Task* task);

		void favourite_chosen(const Task* task);

	private slots:

		void on_unfavourite_button_clicked();

	protected:
		void mouseReleaseEvent(QMouseEvent* event) override;

	private:
		Ui::FavouriteWidget* ui;

		const Task* internal_task;
};

#endif // FAVOURITEWIDGET_H
