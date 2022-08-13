#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSystemTrayIcon>
#include "maputils.h"
#include "tasksscrollarea.h"

MainWindow::MainWindow(const QDir& config_dir, QWidget* parent)
		: QMainWindow(parent), ui(new Ui::MainWindow), favouritesForm(config_dir, this),
		  harvest_handler{ HarvestHandler::get_instance(config_dir) },
		  exit_from_menu{ false },
		  tray_icon{QSystemTrayIcon(QIcon(":/icons/resources/icons/monochrome/32x32.png"), this)},
		  tray_menu(),
		  add_task_action{QAction("Add Task", &tray_icon)},
		  quit_action{QAction("Exit", &tray_icon)},
		  show_hide_action{QAction("Show/Hide", &tray_icon)}
{
	ui->setupUi(this);
	setWindowTitle("Harvest Timer");


	this->app_date = QDate::currentDate();
	this->ui->date_label->setText(this->app_date.toString());
	this->ui->scroll_area_widget_layout->setAlignment(Qt::AlignmentFlag::AlignTop); // TODO can be added on the UI file?
	this->ui->scrollArea->set_harvest_handler(harvest_handler);

	// TODO gather previous data and pass to task form

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
	connect(&task_form, &AddTaskForm::task_started, ui->scrollArea, &TasksScrollArea::add_task);
	connect(&task_form, &AddTaskForm::task_to_favourites, &favouritesForm, &Favourites::add_favourite_task);
	connect(ui->scrollArea, &TasksScrollArea::task_to_favourites, &favouritesForm, &Favourites::add_favourite_task);
	connect(ui->scrollArea, &TasksScrollArea::task_out_of_favourites, &favouritesForm, &Favourites::remove_favourite_task);
	connect(&favouritesForm, &Favourites::add_task, &task_form, &AddTaskForm::add_task_from_favourites);

	create_tray_icon();
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
	app_date = ui->date_label->getAppDate();

	const QDate new_date{ ui->date_label->getAppDate() };
	ui->scrollArea->set_lookup_date(new_date);
	ui->scrollArea->update_task_widgets();
	harvest_handler->list_tasks(app_date.addDays(2), app_date.addDays(2));
}


void MainWindow::on_date_current_button_clicked()
{
	app_date = ui->date_label->getAppDate();
	// If we are already in the current date, there is no need to do anything else here
	if (app_date == QDate::currentDate())
		return;

	this->ui->date_label->resetDate();

	const QDate new_date{ ui->date_label->getAppDate() };
	ui->scrollArea->set_lookup_date(new_date);
	ui->scrollArea->update_task_widgets();
//	harvest_handler->list_tasks(app_date.addDays(-2), app_date.addDays(2));
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
	task_form.exec();
}


void MainWindow::on_favourites_button_clicked()
{
	favouritesForm.exec();
}

// Task widget operations

void MainWindow::task_started(Task* task)
{
	ui->scrollArea->add_task(task);
}

// Ready up the application once we have the basic data from harvest
void MainWindow::harvest_handler_ready()
{
	const std::vector<HarvestProject> projects(harvest_handler->update_user_data());

	task_form.add_projects(projects);

	// we want to load 5 days, two behind and two in advance, to make the day switching experience more fluid
	harvest_handler->list_tasks(app_date.addDays(-2), app_date.addDays(2));

	show();
}

void MainWindow::show_hide(const QSystemTrayIcon::ActivationReason& activation_reason)
{
	switch (activation_reason)
	{
		case QSystemTrayIcon::Trigger:
		{
			// TODO needs improving
			this->tray_menu.popup(QCursor::pos());
			break;
		}
		case QSystemTrayIcon::Context:
		{
			if (this->isVisible())
			{
				this->hide();
			}
			else
			{
				this->show();
				this->raise();
				this->setFocus();
			}
			break;
		}
		default:
			break;
	}
}

void MainWindow::create_tray_icon()
{
	connect(&tray_icon, &QSystemTrayIcon::activated, this, &MainWindow::show_hide);
	connect(&quit_action, &QAction::triggered, this, &MainWindow::exit_triggered);
	connect(&show_hide_action, &QAction::triggered, this, &MainWindow::show_hide_triggered);
	connect(&add_task_action, &QAction::triggered, this, &MainWindow::add_task_triggered);

	tray_menu.addAction(&show_hide_action);
	tray_menu.addAction(&add_task_action);
	tray_menu.addAction(&quit_action);

	tray_icon.setContextMenu(&tray_menu);

	tray_icon.show();
}

void MainWindow::exit_triggered(bool checked)
{
	this->exit_from_menu = true;
	this->close();
}

void MainWindow::show_hide_triggered(bool checked)
{
	this->show_hide(QSystemTrayIcon::ActivationReason::Context);
}

void MainWindow::add_task_triggered(bool checked)
{
	bool was_hidden = !this->isVisible();
	if (was_hidden)
	{
		this->show();
	}

	this->on_new_task_button_clicked();

	if (was_hidden)
	{
		this->hide();
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if(this->exit_from_menu)
	{
		event->accept();
	}
	else
	{
		this->hide();
		event->ignore();
	}
}
