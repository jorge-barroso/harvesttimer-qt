#include "favourites.h"
#include <iostream>
#include "ui_favourites.h"
#include <QMessageBox>
#include <QTextStream>

Favourites::Favourites(const QDir& config_dir, QWidget* parent)
		: QDialog(parent), ui(new Ui::Favourites)
		, favourites_file(config_dir.absolutePath() + "/" + this->favourites_file_name)
{
	ui->setupUi(this);

	load_favourites();
}

Favourites::~Favourites()
{
	delete ui;

	save_favourites();

	if (favourites_file.isOpen())
	{
		favourites_file.close();
	}

	for(const Task* task : tasks)
	{
		delete task;
	}
}

void Favourites::load_favourites()
{
	if (!favourites_file.open(QIODevice::ReadWrite))
	{
		QMessageBox::information(this, "Could not load favourites", favourites_file.errorString());
	}

	Task task;
	QTextStream in(&favourites_file);
	while (!in.atEnd())
	{
		in >> task;
		ui->favourites_scroll_area->add_favourite(&task);
	}
	favourites_file.close();
}

void Favourites::save_favourites()
{
	favourites_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QTextStream stream(&favourites_file);
	for (const Task* task : tasks)
	{
		stream << *task;
	}

	favourites_file.close();
}

void Favourites::add_favourite_task(const Task* task)
{
	tasks.emplace_back(task);
	ui->favourites_scroll_area->add_favourite(task);
}

void Favourites::remove_favourite_task(const Task* task)
{
	std::remove(tasks.begin(), tasks.end(), task);
	ui->favourites_scroll_area->remove_favourite_task(task);
}
