#include <QSystemTrayIcon>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tasksscrollarea.h"

MainWindow::MainWindow(const QDir& config_dir, QWidget* parent)
		: QMainWindow(parent)
		, ui(new Ui::MainWindow)
		, favourites_form(config_dir, this)
		, harvest_handler{ HarvestHandler::get_instance(config_dir) }
		, tray_icon(this)
{
	ui->setupUi(this);

	network_information = QNetworkInformation::instance();
	if(network_information!=nullptr)
	{
		connect(network_information, &QNetworkInformation::reachabilityChanged, this, &MainWindow::reachability_changed);
	}

	this->app_date = QDate::currentDate();
	this->ui->date_label->resetDate();
	this->ui->scroll_area_widget_layout->setAlignment(Qt::AlignmentFlag::AlignTop);
	this->ui->scrollArea->set_harvest_handler(harvest_handler);

	// Wait for our harvest handler to be ready before we show the main window
	// Harvest will be ready automatically when we have user credentials already present
	// otherwise we'll have to wait for the user to log in and then receive the auth credentials
	if (!harvest_handler->is_ready())
	{
		connect(harvest_handler, &HarvestHandler::ready, this, &MainWindow::harvest_handler_ready);
	}
	else
	{
		harvest_handler_ready();
	}

	// connect to signals from modal forms
	connect(harvest_handler, &HarvestHandler::task_added, this, &MainWindow::task_added);
	connect(&add_task_form, &AddTaskForm::task_started, ui->scrollArea, &TasksScrollArea::add_task);
	connect(&add_task_form, &AddTaskForm::task_to_favourites, &favourites_form, &Favourites::add_favourite_task);
	connect(&edit_task_form, &EditTaskForm::task_edited, ui->scrollArea, &TasksScrollArea::task_edited);
	connect(ui->scrollArea, &TasksScrollArea::task_to_favourites, &favourites_form, &Favourites::add_favourite_task);
	connect(ui->scrollArea, &TasksScrollArea::task_out_of_favourites, &favourites_form, &Favourites::remove_favourite_task);
	connect(ui->scrollArea, &TasksScrollArea::task_to_edit, &edit_task_form, &EditTaskForm::edit_task);
	connect(&favourites_form, &Favourites::add_task, &add_task_form, &AddTaskForm::add_task_from_favourites);
	connect(&favourites_form, &Favourites::task_removed_from_favourites, ui->scrollArea, &TasksScrollArea::uncheck_task_favourite);
	connect(&favourites_form, &Favourites::task_added_to_favourites, ui->scrollArea, &TasksScrollArea::check_task_favourite);
}

MainWindow::~MainWindow()
{
	delete ui;
	HarvestHandler::reset_instance();
}


// Date operations
void MainWindow::on_date_forward_button_clicked()
{
	this->ui->date_label->moveForward();
	const QDate new_date{ ui->date_label->getAppDate() };
	app_date = new_date;

	ui->scrollArea->set_lookup_date(new_date);
	ui->scrollArea->update_task_widgets();
	harvest_handler->list_tasks(app_date.addDays(2), app_date.addDays(2));
}


void MainWindow::on_date_current_button_clicked()
{
	const QDate new_date{ ui->date_label->getAppDate() };
	app_date = new_date;

	// If we are already in the current date, there is no need to do anything else here
	if (app_date == QDate::currentDate())
		return;

	this->ui->date_label->resetDate();

	ui->scrollArea->set_lookup_date(new_date);
	ui->scrollArea->update_task_widgets();
}


void MainWindow::on_date_back_button_clicked()
{
	this->ui->date_label->moveBackwards();

	const QDate new_date{ ui->date_label->getAppDate() };
	app_date = new_date;
	ui->scrollArea->set_lookup_date(new_date);
	ui->scrollArea->update_task_widgets();
	harvest_handler->list_tasks(app_date.addDays(-2), app_date.addDays(-2));
}

// Bottom buttons operations

void MainWindow::on_new_task_button_clicked()
{
	add_task_form.exec();
}


void MainWindow::on_favourites_button_clicked()
{
	favourites_form.exec();
}

// Task widget operations

void MainWindow::task_started(Task* task)
{
	ui->scrollArea->add_task(task);
}

// Ready up the application once we have the basic data from harvest
void MainWindow::harvest_handler_ready()
{
	const std::vector<HarvestProject> projects{ harvest_handler->update_user_data() };

	if(!projects.empty())
	{
		add_task_form.add_projects(projects);
		edit_task_form.add_projects(projects);
	}

	// we want to load 5 days, two behind and two in advance, to make the day switching experience more fluid
	harvest_handler->list_tasks(app_date.addDays(-2), app_date.addDays(2));

	show();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	this->hide();
	event->ignore();
}

void MainWindow::task_added(Task* task)
{
	task->favourited = favourites_form.contains(task);
	ui->scrollArea->task_added(task);
}

void MainWindow::reachability_changed(const QNetworkInformation::Reachability& reachability)
{
	// if the reachability has changed, and we are now online, reload the application
	if(network_information->reachability() == QNetworkInformation::Reachability::Online)
	{
		harvest_handler->set_network_reachability(reachability);
		this->harvest_handler_ready();
	}
}
