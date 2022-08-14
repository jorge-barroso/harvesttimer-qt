#include "favouritewidget.h"
#include "ui_favouritewidget.h"

FavouriteWidget::FavouriteWidget(const Task* task, QWidget* parent) :
		QWidget(parent),
		ui(new Ui::FavouriteWidget),
		internal_task{ task }
{
	ui->setupUi(this);
	ui->project_label->setText(task->project_name);
	ui->task_label->setText(task->task_name);
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
}

FavouriteWidget::~FavouriteWidget()
{
	delete ui;
}

bool FavouriteWidget::operator==(const FavouriteWidget& rhs) const
{
	return *internal_task == *rhs.internal_task;
}

bool FavouriteWidget::operator!=(const FavouriteWidget& rhs) const
{
	return !(rhs == *this);
}

void FavouriteWidget::on_unfavourite_button_clicked()
{
	emit unfavourited_task(this, internal_task);
}

void FavouriteWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	emit favourite_chosen(internal_task);
}

const Task* FavouriteWidget::task() const
{
	return this->internal_task;
}
