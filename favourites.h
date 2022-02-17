#ifndef FAVOURITES_H
#define FAVOURITES_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <string>
#include <vector>
#include "favourite.h"
#include "favouritewidget.h"

namespace Ui {
class Favourites;
}

class Favourites : public QDialog
{
    Q_OBJECT

public:
    explicit Favourites(const QDir & config_dir, QWidget *parent = nullptr);
    ~Favourites();

private:
    Ui::Favourites *ui;
    const QString favourites_file_name = "favourites.cfg";
    QFile favourites_file;
    std::vector<Favourite> favourites;
    std::vector<const FavouriteWidget*> favourite_widgets;
    void load_favourites();
    void save_favourites();
};

#endif // FAVOURITES_H
