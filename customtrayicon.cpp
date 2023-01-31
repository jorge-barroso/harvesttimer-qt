//
// Created by jorge on 8/14/22.
//

#include "customtrayicon.h"
#include "mainwindow.h"
#include <QApplication>

CustomTrayIcon::CustomTrayIcon(QObject* parent, const bool is_dark)
	: QSystemTrayIcon(parent)
    , tray_icon{QSystemTrayIcon(CustomTrayIcon::get_icon(is_dark), this)}
	, tray_menu()
	, add_task_action{ AddTaskAction(&tray_icon) }
	, quit_action{ QuitAction(&tray_icon) }
	, show_hide_action{ ShowOrHideAction(&tray_icon) }
{
	connect(&tray_icon, &QSystemTrayIcon::activated, this, &CustomTrayIcon::show_hide);
	connect(&quit_action, &QAction::triggered, this, &CustomTrayIcon::exit_triggered);
	connect(&show_hide_action, &QAction::triggered, this, &CustomTrayIcon::show_hide_triggered);
	connect(&add_task_action, &QAction::triggered, this, &CustomTrayIcon::add_task_triggered);

	tray_menu.addAction(&show_hide_action);
	tray_menu.addAction(&add_task_action);
	tray_menu.addAction(&quit_action);

	tray_icon.setContextMenu(&tray_menu);

	tray_icon.show();
}

CustomTrayIcon::~CustomTrayIcon()= default;

void CustomTrayIcon::exit_triggered(bool checked)
{
	QApplication::quit();
}

void CustomTrayIcon::show_hide_triggered(bool checked)
{
	this->show_hide(QSystemTrayIcon::ActivationReason::Context);
}

void CustomTrayIcon::add_task_triggered(bool checked)
{
	auto parent{ dynamic_cast<MainWindow*>(this->parent()) };
	bool was_hidden = !parent->isVisible();
	if (was_hidden)
	{
		parent->show();
	}

	parent->on_new_task_button_clicked();

	if (was_hidden)
	{
		parent->hide();
	}
}

void CustomTrayIcon::show_hide(const QSystemTrayIcon::ActivationReason& activation_reason)
{
	auto parent{ dynamic_cast<MainWindow*>(this->parent()) };
	switch (activation_reason)
	{
		case QSystemTrayIcon::Context:
		{
			if (parent->isVisible())
			{
				parent->hide();
			}
			else
			{
				parent->show();
				parent->raise();
				parent->setFocus();
			}
			break;
		}
		default:
			break;
	}
}

QIcon CustomTrayIcon::get_icon(const bool is_dark) {
    const QString resource = is_dark ? ":/icons/resources/icons/monochrome/dark/harvest.svg"
                                     : ":/icons/resources/icons/monochrome/light/harvest.svg";
    return QIcon(resource);
}
