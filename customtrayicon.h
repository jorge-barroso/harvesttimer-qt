//
// Created by jorge on 8/14/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_CUSTOMTRAYICON_H
#define HARVESTTIMER_QT_CMAKE_CUSTOMTRAYICON_H


#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include "addtaskaction.h"
#include "quitaction.h"
#include "showorhideaction.h"

class CustomTrayIcon : public QSystemTrayIcon
{
	public:

		explicit CustomTrayIcon(QObject* parent, bool is_dark);

		~CustomTrayIcon() override;

        void reset_icon(bool is_dark);

	private slots:

		static void exit_triggered(bool checked = false);

		void show_hide_triggered(bool checked = false);

		void add_task_triggered(bool checked = false);

		void show_hide(const QSystemTrayIcon::ActivationReason& activation_reason);

	private:
		QSystemTrayIcon tray_icon;

		QuitAction quit_action;

        ShowOrHideAction show_hide_action;

		AddTaskAction add_task_action;

		QMenu tray_menu;

    static QIcon get_icon(bool is_dark);
};


#endif //HARVESTTIMER_QT_CMAKE_CUSTOMTRAYICON_H
