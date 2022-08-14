//
// Created by jorge on 8/14/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_CUSTOMTRAYICON_H
#define HARVESTTIMER_QT_CMAKE_CUSTOMTRAYICON_H


#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

class CustomTrayIcon : public QSystemTrayIcon
{
	public:

		explicit CustomTrayIcon(QObject* parent);

		~CustomTrayIcon() override;

	private slots:

		static void exit_triggered(bool checked = false);

		void show_hide_triggered(bool checked = false);

		void add_task_triggered(bool checked = false);

		void show_hide(const QSystemTrayIcon::ActivationReason& activation_reason);

	private:
		QSystemTrayIcon tray_icon;

		QAction quit_action;

		QAction show_hide_action;

		QAction add_task_action;

		QMenu tray_menu;
};


#endif //HARVESTTIMER_QT_CMAKE_CUSTOMTRAYICON_H
