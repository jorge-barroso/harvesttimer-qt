//
// Created by jorge on 8/21/22.
//

#ifndef HARVESTTIMER_QT_CMAKE_EDITTASKFORM_H
#define HARVESTTIMER_QT_CMAKE_EDITTASKFORM_H

#include <QDialog>
#include "task.h"
#include "customtaskdialog.h"
#include "taskwidget.h"

namespace Ui
{
	class EditTaskForm;
}

class EditTaskForm : public CustomTaskDialog
{
	Q_OBJECT

	public:
		explicit EditTaskForm(QWidget* parent = nullptr);

		~EditTaskForm() override;

		void add_projects(const std::vector<HarvestProject>& new_projects);

		void edit_task(const Task* task, TaskWidget* task_widget);

	signals:

		void task_edited(Task* task, TaskWidget* task_widget);

	private slots:

		void on_delete_button_clicked();

		void on_confirm_button_clicked();

		void on_project_dropdown_currentIndexChanged(int index);

	private:

		Ui::EditTaskForm* ui;

		const Task* task_to_modify;
		TaskWidget* widget_to_modify;

		void reset_inputs() override;


		void project_changed(const int project_index, Task* task_to_edit, const QString& new_project_name);

		static void task_changed(Task* task_to_edit, const HarvestProject& chosen_project, const int new_task_id);

};


#endif //HARVESTTIMER_QT_CMAKE_EDITTASKFORM_H
