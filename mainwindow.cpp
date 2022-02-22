#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(const QDir& config_dir, QWidget* parent)
		: QMainWindow(parent), ui(new Ui::MainWindow), favouritesForm(config_dir), harvest(config_dir)
{
	ui->setupUi(this);
	setWindowTitle("Harvest Timer");

	this->appDate = QDate::currentDate();
	this->ui->date_label->setText(this->appDate.toString());
	this->ui->scroll_area_widget_layout->setAlignment(Qt::AlignmentFlag::AlignTop);

	// TODO gather previous data and pass to task form

	// Wait for our harvest handler to be ready before we show the main window
	// Harvest will be ready automatically when we have user credentials already present
	// otherwise we'll have to wait for the user to log in and then receive the auth credentials
	if (!harvest.is_ready())
		connect(&harvest, &HarvestHandler::ready, this, &MainWindow::harvest_handler_ready);
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
	update_task_widgets(ui->date_label->getAppDate());
}


void MainWindow::on_date_current_button_clicked()
{
	QDate appDate{ ui->date_label->getAppDate() };
	// If we are already in the current date, there is no need to do anything else here
	if (appDate == QDate::currentDate())
		return

	this->ui->date_label->resetDate();

	update_task_widgets(appDate);
}


void MainWindow::on_date_back_button_clicked()
{
	this->ui->date_label->moveBackwards();
	update_task_widgets(ui->date_label->getAppDate());
}

void MainWindow::update_task_widgets(QDate date)
{
	QLayoutItem* child;
	QLayout* layout = ui->scrollAreaWidgetContents->layout();
	while ((child = layout->takeAt(0)) != 0)
	{
		delete child->widget();
	}

	if (!task_widgets.contains(date))
		return;

	std::vector<TaskWidget*> task_widgets_vector{ task_widgets[date] };
	for (const auto& task_widget: task_widgets_vector)
	{
		ui->scrollAreaWidgetContents->layout()->addWidget(task_widget);
	}
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

void MainWindow::task_started(Task& task)
{
	map_insert_or_create(tasks, appDate, task);

	auto* task_widget = new TaskWidget(task, ui->scrollAreaWidgetContents);
	ui->scrollAreaWidgetContents->layout()->addWidget(task_widget);

	// TODO if time is 00:00 then start tracking and timing, else just add it to harvest
	map_insert_or_create(task_widgets, appDate, task_widget);
}


void MainWindow::task_to_favourites(Task& task)
{
	std::cout << "Task Favourited: " << task.project.toStdString() << " // " << task.task_name.toStdString() << " at "
			  << task.time_tracked.toString().toStdString() << std::endl;
}


// Ready up the application once we have the basic data from harvest

void MainWindow::harvest_handler_ready()
{
	const std::vector<Project> projects(harvest.get_user_data());
	task_form.add_projects(projects);

	// TODO get tasks history

	show();
}