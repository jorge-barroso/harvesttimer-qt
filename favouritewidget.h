#ifndef FAVOURITEWIDGET_H
#define FAVOURITEWIDGET_H

#include <QDialog>
#include "favourite.h"

namespace Ui {
class FavouriteWidget;
}

class FavouriteWidget : public QDialog
{
    Q_OBJECT

public:
    explicit FavouriteWidget(const Favourite &favourite, QWidget *parent = nullptr);
    ~FavouriteWidget();

private:
    Ui::FavouriteWidget *ui;
};

#endif // FAVOURITEWIDGET_H
