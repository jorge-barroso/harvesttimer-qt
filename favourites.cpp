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

	connect(ui->favourites_list, &FavouritesList::new_task_selected, this, &Favourites::new_task_selected);
	connect(ui->favourites_list, &FavouritesList::task_removed, this, &Favourites::erase_task);
}

Favourites::~Favourites()
{
	delete ui;

	save_favourites();

	if (favourites_file.isOpen())
	{
		favourites_file.close();
	}

	for (const Task* task: tasks)
	{
		delete task;
	}
}

void Favourites::load_favourites()
{
	if (!favourites_file.open(QIODevice::ReadWrite))
	{
		QMessageBox::warning(this, QApplication::translate("Favourites", "Could not load favourites"), favourites_file.errorString());
	}

	QTextStream in(&favourites_file);
	while (!in.atEnd())
	{
		Task* task = new Task{ };
		in >> *task;
		add_favourite_task(task);
	}
	favourites_file.close();
}

void Favourites::save_favourites()
{
	favourites_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QTextStream stream(&favourites_file);
	for (const Task* task: tasks)
	{
		stream << *task;
	}

	favourites_file.close();
}

bool Favourites::contains(const Task* task) const
{
	const auto find_predicate{
			[task](const Task* vector_task)
			{
				return task->project_name == vector_task->project_name &&
					   task->task_name == vector_task->task_name;
			}
	};
	return std::find_if(tasks.begin(), tasks.end(), find_predicate) != tasks.end();
}

void Favourites::add_favourite_task(const Task* task)
{
	// if we could already find this task, stop it
	if(this->contains(task))
	{
		return;
	}

	tasks.emplace_back(task);

	ui->favourites_list->add_favourite(task);

	emit task_added_to_favourites(task);
}

void Favourites::remove_favourite_task(const Task* task)
{
	erase_task(task);

	ui->favourites_list->remove_favourite_task(task);
}

void Favourites::erase_task(const Task* task)
{
	auto task_it{ std::find(tasks.begin(), tasks.end(), task) };
	if (task_it != tasks.end())
	{
		// We don't delete the task here, it might still be in use in the scroll area
		tasks.erase(task_it);
	}

	emit task_removed_from_favourites(task);
}

void Favourites::new_task_selected(const Task* task)
{
	emit add_task(const_cast<Task*>(task));
	this->close();
}
