#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "maputils.h"
#include "tasksscrollarea.h"

MainWindow::MainWindow(const QDir& config_dir, QWidget* parent)
		: QMainWindow(parent), ui(new Ui::MainWindow), favouritesForm(config_dir),
		  harvest{ HarvestHandler::getInstance(config_dir) }
{
	ui->setupUi(this);
	setWindowTitle("Harvest Timer");


	this->app_date = QDate::currentDate();
	this->ui->date_label->setText(this->app_date.toString());
	this->ui->scroll_area_widget_layout->setAlignment(Qt::AlignmentFlag::AlignTop); // TODO can be added on the UI file?
	this->ui->scrollArea->setHarvestHandler(harvest);

	// TODO gather previous data and pass to task form

	// Wait for our harvest handler to be ready before we show the main window
	// Harvest will be ready automatically when we have user credentials already present
	// otherwise we'll have to wait for the user to log in and then receive the auth credentials
	if (!harvest->is_ready())
		connect(harvest, &HarvestHandler::ready, this, &MainWindow::harvest_handler_ready);
	else
		harvest_handler_ready();

	// connect to signals from modal forms
	connect(&task_form, &AddTaskForm::task_started, this, &MainWindow::task_started);
	connect(&task_form, &AddTaskForm::task_to_favourites, this, &MainWindow::task_to_favourites);
}

MainWindow::~MainWindow()
{
	delete ui;
}


// Date operations
// TODO update widgets on date change

void MainWindow::on_date_forward_button_clicked()
{
	this->ui->date_label->moveForward();
	app_date = ui->date_label->getAppDate();
	ui->scrollArea->updateTaskWidgets(ui->date_label->getAppDate());
}


void MainWindow::on_date_current_button_clicked()
{
	app_date = ui->date_label->getAppDate();
	// If we are already in the current date, there is no need to do anything else here
	if (app_date == QDate::currentDate())
		return;

	this->ui->date_label->resetDate();

	ui->scrollArea->updateTaskWidgets(app_date);
}


void MainWindow::on_date_back_button_clicked()
{
	this->ui->date_label->moveBackwards();
	app_date = ui->date_label->getAppDate();
	ui->scrollArea->updateTaskWidgets(ui->date_label->getAppDate());
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

void MainWindow::task_started(Task* task)
{
	std::optional<long long> time_entry_id{ harvest->addTask(task) };
	if (!time_entry_id.has_value())
		return;

	task->time_entry_id = time_entry_id.value();

	ui->scrollArea->addTask(task, app_date);
//
//	if (currentTask != nullptr)
//		ui->scrollArea->stopCurrentTask();
//
//	currentTask = &task;
}

void MainWindow::task_to_favourites(Task* task)
{
	std::cout << "Task Favourited: " << task->project_name.toStdString() << " // " << task->task_name.toStdString()
			  << " at "
			  << task->time_tracked.toString().toStdString() << std::endl;
}


// Ready up the application once we have the basic data from harvest

void MainWindow::harvest_handler_ready()
{
	const std::vector<HarvestProject> projects(harvest->update_user_data());
	task_form.add_projects(projects);

	// TODO get tasks history

	show();
}