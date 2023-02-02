//
// Created by jorge on 8/21/22.
//

#include <QMessageBox>
#include <QApplication>
#include "edittaskform.h"
#include "ui_edittaskform.h"

EditTaskForm::EditTaskForm(QWidget* parent)
		: CustomTaskDialog(parent), ui(new Ui::EditTaskForm)
{
	ui->setupUi(this);
}

EditTaskForm::~EditTaskForm() = default;

void EditTaskForm::add_projects(const std::vector<HarvestProject>& new_projects)
{
	CustomTaskDialog::add_projects(new_projects, ui->project_dropdown, ui->task_dropdown);
}

void EditTaskForm::edit_task(const Task* task, TaskWidget* task_widget)
{
	task_to_modify = task;
	widget_to_modify = task_widget;

	int index{ ui->project_dropdown->findText(task->project_name) };
	if (index == -1)
	{
		QMessageBox::warning(this,
							 QApplication::translate("TaskForm", "Could not find project"),
							 QApplication::translate("TaskForm",
													 "The project this task is assigned to does not exist or has been renamed, please update your favourites to keep using them")
		);
		return;
	}
	ui->project_dropdown->setCurrentIndex(index);

	index = ui->task_dropdown->findText(task->task_name);
	if (index == -1)
	{
		QMessageBox::warning(this,
							 QApplication::translate("TaskForm", "Could not find task"),
							 QApplication::translate("TaskForm",
													 "The task selected does not exist or has been renamed, please update your favourites to keep using them")
		);
		return;
	}

	ui->task_dropdown->setCurrentIndex(index);

	ui->notes_text_edit->setText(task->note);

	ui->time_worked_line->setTime(task->time_tracked);

	this->exec();
}

void EditTaskForm::on_confirm_button_clicked()
{
	const int chosen_project_index = ui->project_dropdown->currentIndex();
	const QString& new_project_name{ ui->project_dropdown->itemText(chosen_project_index) };

	bool edited{ false };

	Task* edited_task{ new Task(*task_to_modify) };
	if (task_to_modify->project_name != new_project_name)
	{
		this->project_changed(chosen_project_index, edited_task, new_project_name);
		edited = true;
	}
	else
	{
		const int chosen_task = ui->task_dropdown->currentIndex();
		const QString& new_task_name{ ui->task_dropdown->itemText(chosen_task) };
		if (task_to_modify->task_name != new_task_name)
		{
			task_changed(edited_task, projects[chosen_project_index], chosen_task);
			edited = true;
		}
	}

	const QString& new_note = ui->notes_text_edit->toPlainText();
	if (task_to_modify->note != new_note)
	{
		edited_task->note = new_note;
		edited = true;
	}

	const QTime& new_tracked_time = ui->time_worked_line->time();
	if (task_to_modify->time_tracked != new_tracked_time)
	{
		edited_task->time_tracked = new_tracked_time;
		edited = true;
	}

	if (edited)
	{
		emit task_edited(edited_task, widget_to_modify);
	}
	else
	{
		delete edited_task;
	}

	this->reset_and_close();
}


void EditTaskForm::on_delete_button_clicked()
{
	QMessageBox::StandardButton reply{ QMessageBox::question(this,
								  QApplication::translate("DeleteTask", "Delete Task"),
								  QApplication::translate("DeleteTask", "Are you sure about deleting this task? This action cannot be undone."),
								  QMessageBox::Yes|QMessageBox::No) };
	if (reply == QMessageBox::Yes) {
		emit widget_to_modify->task_deleted(task_to_modify, widget_to_modify);
		this->reset_and_close();
	}
}

void EditTaskForm::reset_inputs()
{
	ui->project_dropdown->setCurrentIndex(0);
	ui->task_dropdown->setCurrentIndex(0);
	ui->notes_text_edit->setText({ });
	ui->time_worked_line->setTime({ 0, 0 });
}

void EditTaskForm::on_project_dropdown_currentIndexChanged(int index)
{
	CustomTaskDialog::new_project_chosen(index, ui->task_dropdown);
}

void EditTaskForm::project_changed(const int project_index, Task* task_to_edit, const QString& new_project_name)
{
	const HarvestProject chosen_project{ projects[project_index] };
	task_to_edit->project_id = chosen_project.project_id;
	task_to_edit->project_name = new_project_name;


	const int chosen_task{ ui->task_dropdown->currentIndex() };
	task_changed(task_to_edit, chosen_project, chosen_task);
}

void EditTaskForm::task_changed(Task* task_to_edit, const HarvestProject& chosen_project, const int new_task_id)
{
	const HarvestTask task{ chosen_project.task[new_task_id] };
	task_to_edit->task_id = task.task_id;
	task_to_edit->task_name = task.task_name;
}