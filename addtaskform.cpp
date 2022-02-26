#include "addtaskform.h"
#include "ui_addtaskform.h"
#include <QStringList>

AddTaskForm::AddTaskForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTaskForm)
{
    ui->setupUi(this);
    setWindowTitle("Add new task");
}

AddTaskForm::~AddTaskForm()
{
    delete ui;
}

Task* AddTaskForm::build_task()
{
    // TODO improve handling time user input (empty, only minutes, etc.)
	const HarvestProject chosen_project { projects[ui->project_dropdown->currentIndex()] };
	const QTime time{ this->ui->time_worked_line->time() };
    Task* task = new Task{
		chosen_project.project_id,
		chosen_project.task[ui->task_dropdown->currentIndex()].task_id,
		0,
        this->ui->project_dropdown->currentText(),
        this->ui->task_dropdown->currentText(),
		time,
		this->ui->notes_text_edit->toPlainText(),
		zero_time.secsTo(task->time_tracked) == 0,
    };

    return task;
}

void AddTaskForm::on_start_button_clicked()
{
    Task* task{build_task()};

    emit task_started(task);

    this->close();
}


void AddTaskForm::on_favourite_button_clicked()
{
    Task* task{build_task()};

    emit task_to_favourites(task);

    this->close();
}


void AddTaskForm::on_cancel_button_clicked()
{
    this->close();

    // TODO reset
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
	for(const HarvestTask& task: projects[index].task)
	{
		ui->task_dropdown->addItem(task.task_name);
	}
}

