#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDate>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include "task.h"
#include "favourites.h"
#include "addtaskform.h"
#include "harvesthandler.h"
#include "customtrayicon.h"

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

	public slots:

		void on_new_task_button_clicked();

	private slots:

		void on_date_forward_button_clicked();

		void on_date_current_button_clicked();

		void on_date_back_button_clicked();

		void on_favourites_button_clicked();

		void task_started(Task* task);

		void harvest_handler_ready();

		void task_added(Task* task);

	private:
		Ui::MainWindow* ui;
		AddTaskForm task_form;
		Favourites favouritesForm;
		QDate app_date;
		HarvestHandler* harvest_handler;

		// systray menu
		CustomTrayIcon tray_icon;

		void closeEvent(QCloseEvent* event) override;
};

#endif // MAINWINDOW_H
