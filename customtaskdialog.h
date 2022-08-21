//
// Created by jorge on 8/21/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_CUSTOMTASKDIALOG_H
#define HARVESTTIMER_QT_CMAKE_CUSTOMTASKDIALOG_H


#include <QDialog>
#include <QComboBox>
#include "harvestproject.h"

class CustomTaskDialog : public QDialog
{
	Q_OBJECT

	public:
		CustomTaskDialog(QWidget* parent);

		~CustomTaskDialog() override;

	protected:
		std::vector<HarvestProject> projects;

		void reset_and_close();

		void add_projects(const std::vector<HarvestProject>& new_projects, QComboBox* project_dropdown,
						  QComboBox* task_dropdown);


		virtual void reset_inputs() = 0;

	protected:

		void new_project_chosen(int index, QComboBox* task_dropdown);

	private slots:

		void on_cancel_button_clicked();
};


#endif //HARVESTTIMER_QT_CMAKE_CUSTOMTASKDIALOG_H
