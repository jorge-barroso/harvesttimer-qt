#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include "addtaskform.h"
#include "favourites.h"
#include "harvesthandler.h"
#include "taskwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(const QDir& config_dir, QWidget* parent = nullptr);

		~MainWindow() override;

	private slots:

		void on_date_forward_button_clicked();

		void on_date_current_button_clicked();

		void on_date_back_button_clicked();

		void on_new_task_button_clicked();

		void on_favourites_button_clicked();

		void task_started(Task* task);

		void task_to_favourites(Task* task);

		void harvest_handler_ready();

		void exit_triggered(bool checked = false);

		void show_hide_triggered(bool checked = false);

		void add_task_triggered(bool checked = false);

	private:
		Ui::MainWindow* ui;
		AddTaskForm task_form;
		Favourites favouritesForm;
		QDate app_date;
		HarvestHandler* harvest_handler;

		// systray menu
		QSystemTrayIcon tray_icon;
		QAction quit_action;
		QAction show_hide_action;
		QAction add_task_action;
		QMenu tray_menu;

		void show_hide(const QSystemTrayIcon::ActivationReason& activation_reason);

		void create_tray_icon();

		void closeEvent(QCloseEvent* event) override;

		bool exit_from_menu;
};

#endif // MAINWINDOW_H
