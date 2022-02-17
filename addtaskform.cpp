#include "addtaskform.h"
#include "ui_addtaskform.h"
#include <QTime>
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

Task AddTaskForm::build_task()
{
    // TODO improve handling time user input (empty, only minutes, etc.)
    QString time_worked{this->ui->time_worked_line->text()};
    QTime time_tracked;
    if (time_worked.isEmpty())
    {
        time_tracked = QTime(0, 0);
    }
    else
    {
        QStringList time_components{time_worked.split(":")};
        time_tracked = QTime(time_components[0].toInt(), time_components[1].toInt());
    }

    Task task{
        this->ui->project_dropdown->currentText(),
        this->ui->task_dropdown->currentText(),
        time_tracked
    };

    return task;
}

void AddTaskForm::on_start_button_clicked()
{
    Task task{build_task()};

    emit task_started(task);

    this->close();
}


void AddTaskForm::on_favourite_button_clicked()
{
    Task task{build_task()};

    emit task_to_favourites(task);

    this->close();
}


void AddTaskForm::on_cancel_button_clicked()
{
    this->close();

    // TODO reset
}

void AddTaskForm::add_projects(const std::vector<Project>& new_projects)
{
	projects = new_projects;
	for (const Project& project: projects)
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

