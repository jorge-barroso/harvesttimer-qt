//
// Created by jorge on 8/21/22.
//

#include <QFuture>
#include "customtaskdialog.h"

CustomTaskDialog::CustomTaskDialog(QWidget* parent) : QDialog(parent)
{}

CustomTaskDialog::~CustomTaskDialog() = default;

void CustomTaskDialog::reset_and_close()
{
	this->close();
	this->reset_inputs();
}

void CustomTaskDialog::on_cancel_button_clicked()
{
	this->reset_and_close();
}

void CustomTaskDialog::add_projects(const std::vector<HarvestProject>& new_projects, QComboBox* project_dropdown,
									QComboBox* task_dropdown)
{
	projects = new_projects;
	for (const HarvestProject& project: projects)
	{
		project_dropdown->addItem(project.project_name);
	}

	new_project_chosen(0, task_dropdown);
}

void CustomTaskDialog::new_project_chosen(int index, QComboBox* task_dropdown)
{
	task_dropdown->clear();
	for (const HarvestTask& task: projects[index].task)
	{
		task_dropdown->addItem(task.task_name);
	}
}