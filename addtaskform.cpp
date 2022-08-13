#include "addtaskform.h"
#include "ui_addtaskform.h"
#include <QStringList>
#include <QMessageBox>

AddTaskForm::AddTaskForm(QWidget* parent) :
		QDialog(parent),
		ui(new Ui::AddTaskForm)
{
	ui->setupUi(this);
}

AddTaskForm::~AddTaskForm()
{
	delete ui;
}

Task* AddTaskForm::build_task()
{
	const HarvestProject chosen_project{ projects[ui->project_dropdown->currentIndex()] };
	const QTime time{ this->ui->time_worked_line->time() };
	Task* task = new Task{
			chosen_project.project_id,
			chosen_project.task[ui->task_dropdown->currentIndex()].task_id,
			0,
			this->ui->project_dropdown->currentText(),
			this->ui->task_dropdown->currentText(),
			time,
			this->ui->notes_text_edit->toPlainText(),
			zero_time.secsTo(time) == 0,
	};

	return task;
}

void AddTaskForm::on_start_button_clicked()
{
	Task* task{ build_task() };

	emit task_started(task);

	this->reset_and_close();
}


void AddTaskForm::on_favourite_button_clicked()
{
	Task* task{ build_task() };

	emit task_to_favourites(task);

	this->reset_and_close();
}


void AddTaskForm::on_cancel_button_clicked()
{
	this->reset_and_close();
}

void AddTaskForm::add_projects(const std::vector<HarvestProject>& new_projects)
{
	projects = new_projects;
	for (const HarvestProject& project: projects)
	{
		ui->project_dropdown->addItem(project.project_name);
	}

	on_project_dropdown_currentIndexChanged(0);
}

void AddTaskForm::on_project_dropdown_currentIndexChanged(int index)
{
	ui->task_dropdown->clear();
	for (const HarvestTask& task: projects[index].task)
	{
		ui->task_dropdown->addItem(task.task_name);
	}
}

void AddTaskForm::reset_and_close()
{
	this->close();
	this->reset_inputs();
}

void AddTaskForm::reset_inputs()
{
	ui->project_dropdown->setCurrentIndex(0);
	ui->task_dropdown->setCurrentIndex(0);
	ui->notes_text_edit->setText({ });
	ui->time_worked_line->setTime({ 0, 0 });
}

void AddTaskForm::add_task_from_favourites(const Task* task)
{
	int index{ ui->project_dropdown->findText(task->project_name) };
	if (index==-1)
	{
		QMessageBox::warning(this,
							 QApplication::translate("AddTaskForm", "Could not find project"),
							 QApplication::translate("AddTaskForm", "The project this task is assigned to does not exist or has been renamed, please update your favourites to keep using them")
		);
		return;
	}
	ui->project_dropdown->setCurrentIndex(index);

	index = ui->task_dropdown->findText(task->task_name);
	if (index==-1)
	{
		QMessageBox::warning(this,
							 QApplication::translate("AddTaskForm", "Could not find task"),
							 QApplication::translate("AddTaskForm", "The task selected does not exist or has been renamed, please update your favourites to keep using them")
		);
		return;
	}
	ui->task_dropdown->setCurrentIndex(index);

	this->exec();
}
