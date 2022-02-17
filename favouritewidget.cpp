#include "favouritewidget.h"
#include "ui_favouritewidget.h"

FavouriteWidget::FavouriteWidget(const Favourite &favourite, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FavouriteWidget)
{
    ui->setupUi(this);
    ui->organization_label->setText(favourite.company);
    ui->project_label->setText(favourite.project_name);
    ui->task_label->setText(favourite.task_name);
}

FavouriteWidget::~FavouriteWidget()
{
    delete ui;
}
