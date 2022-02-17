#include "favourites.h"
#include <iostream>
#include "ui_favourites.h"
#include <cstdio>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTextStream>
#include "favouritewidget.h"

Favourites::Favourites(const QDir& config_dir, QWidget* parent)
    : QDialog(parent), ui(new Ui::Favourites)
    , favourites_file(config_dir.absolutePath() + "/" + this->favourites_file_name)
{
	ui->setupUi(this);

	load_favourites();

	favourite_widgets.reserve(favourites.size());

	for (uint8_t i = 0; i < favourites.size(); ++i)
	{
		const Favourite favourite = favourites[i];
		const FavouriteWidget* widget = new FavouriteWidget(favourite, ui->scrollAreaWidgetContents);
		favourite_widgets[i] = widget;
	}
}

Favourites::~Favourites()
{
	delete ui;

	save_favourites();

	if (favourites_file.isOpen())
	{
		favourites_file.close();
	}

	for (const FavouriteWidget* favourite_widget: favourite_widgets)
	{
		delete favourite_widget;
	}
}

void Favourites::load_favourites()
{
	if (!favourites_file.open(QIODevice::ReadWrite))
	{
		QMessageBox::information(this, "Could not load favourites", favourites_file.errorString());
	}

	QTextStream in(&favourites_file);
	while (!in.atEnd())
	{
		const QString line = in.readLine();
		const QStringList fields = line.split(",");
		const Favourite new_favourite = { fields[0], fields[1], fields[2] };
		favourites.emplace_back(new_favourite);
	}
	favourites_file.close();
}

void Favourites::save_favourites()
{
	favourites_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	for (const Favourite& favourite: favourites)
	{
		QTextStream stream(&favourites_file);
		stream << favourite.company << "," << favourite.project_name << "," << favourite.task_name << "\n";
	}

	favourites_file.close();
}
