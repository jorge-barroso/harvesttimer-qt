#include "favouritewidget.h"
#include "ui_favouritewidget.h"

FavouriteWidget::FavouriteWidget(const Task* task, QWidget* parent) :
		QWidget(parent),
		ui(new Ui::FavouriteWidget),
		task{ task }
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
	return *task == *rhs.task;
}

bool FavouriteWidget::operator!=(const FavouriteWidget& rhs) const
{
	return !(rhs == *this);
}

void FavouriteWidget::on_unfavourite_button_clicked()
{
	emit unfavourited_task(this);
}

