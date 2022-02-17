#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(const QDir & config_dir, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , favouritesForm(config_dir)
	, harvest(config_dir)
{
    ui->setupUi(this);
    setWindowTitle("Harvest Timer");

    this->appDate = QDate::currentDate();
    this->ui->date_label->setText(this->appDate.toString());

    // TODO gather data and pass to task form

	// Wait for our harvest handler to be ready before we show the main window
	if (!harvest.is_ready())
	{
		connect(&harvest, &HarvestHandler::ready, this, &MainWindow::harvest_handler_ready);
	}
	else
	{
		harvest_handler_ready();
	}

    // connect to signals from modal forms
    connect(&task_form, &AddTaskForm::task_started, this, &MainWindow::task_started);
    connect(&task_form, &AddTaskForm::task_to_favourites, this, &MainWindow::task_to_favourites);
}

MainWindow::~MainWindow()
{
    delete ui;
}


// Date operations

void MainWindow::on_date_forward_button_clicked()
{
    this->ui->date_label->moveForward();
}


void MainWindow::on_date_current_button_clicked()
{
    this->ui->date_label->resetDate();
}


void MainWindow::on_date_back_button_clicked()
{
    this->ui->date_label->moveBackwards();
}


// Bottom buttons operations

void MainWindow::on_new_task_button_clicked()
{
    task_form.exec();
}


void MainWindow::on_favourites_button_clicked()
{
    favouritesForm.exec();
}

// Task widget operations

void MainWindow::task_started(Task &task) {
    std::cout << "Task Added: " << task.project.toStdString() << " // " << task.task_name.toStdString() << " at " << task.time_tracked.toString().toStdString() << std::endl;
}


void MainWindow::task_to_favourites(Task &task) {
    std::cout << "Task Favourited: " << task.project.toStdString() << " // " << task.task_name.toStdString() << " at " << task.time_tracked.toString().toStdString() << std::endl;
}


// Ready up the application once we have the basic data from harvest

void MainWindow::harvest_handler_ready()
{
	const std::vector<Project> projects(harvest.get_user_data());
	task_form.add_projects(projects);
	show();
}
