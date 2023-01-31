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
#include "edittaskform.h"

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

        void changeEvent(QEvent * event) override;

		void task_started(Task* task);

		void harvest_handler_ready();

		void task_added(Task* task);

		void reachability_changed(const QNetworkInformation::Reachability& reachability);

	private:
		Ui::MainWindow* ui;
		AddTaskForm add_task_form;
		EditTaskForm edit_task_form;
		Favourites favourites_form;
		QDate app_date;

		HarvestHandler* harvest_handler;
		const QNetworkInformation* network_information;

		// systray menu
		CustomTrayIcon *tray_icon;

		void closeEvent(QCloseEvent* event) override;

        bool isDarkTheme();
};

#endif // MAINWINDOW_H
