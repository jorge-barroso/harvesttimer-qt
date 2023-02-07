#include "addtaskform.h"
#include "ui_addtaskform.h"
#include <QStringList>
#include <QMessageBox>

AddTaskForm::AddTaskForm(QWidget *parent) :
        CustomTaskDialog(parent),
        ui(new Ui::AddTaskForm) {
    ui->setupUi(this);
}

AddTaskForm::~AddTaskForm() {
    delete ui;
}

Task *AddTaskForm::build_task() {
    const HarvestProject chosen_project{projects[ui->project_dropdown->currentIndex()]};
    const HarvestTask chosen_task{chosen_project.task[ui->task_dropdown->currentIndex()]};
    const QTime time{this->ui->time_worked_line->time()};
    Task *task = new Task{
            .project_id = chosen_project.project_id,
            .task_id = chosen_task.task_id,
            .time_entry_id = 0,
            .client_name = chosen_task.client_name,
            .project_name = this->ui->project_dropdown->currentText(),
            .task_name = this->ui->task_dropdown->currentText(),
            .time_tracked = time,
            .note = this->ui->notes_text_edit->toPlainText(),
            .started = zero_time.secsTo(time) == 0,
    };

    return task;
}

void AddTaskForm::on_start_button_clicked() {
    Task *task{build_task()};

    emit task_started(task);

    this->reset_and_close();
}


void AddTaskForm::on_favourite_button_clicked() {
    Task *task{build_task()};

    emit task_to_favourites(task);

    this->reset_and_close();
}

void AddTaskForm::add_projects(const std::vector<HarvestProject> &new_projects) {
    CustomTaskDialog::add_projects(new_projects, ui->project_dropdown, ui->task_dropdown);
}


void AddTaskForm::on_project_dropdown_currentIndexChanged(int index) {
    CustomTaskDialog::new_project_chosen(index, ui->task_dropdown);
}

void AddTaskForm::reset_inputs() {
    ui->project_dropdown->setCurrentIndex(0);
    ui->task_dropdown->setCurrentIndex(0);
    ui->notes_text_edit->setText({});
    ui->time_worked_line->setTime({0, 0});
}

void AddTaskForm::add_task_from_favourites(const Task *task) {
    int index{ui->project_dropdown->findText(task->project_name)};
    if (index == -1) {
        QMessageBox::warning(this,
                             QApplication::translate("TaskForm", "Could not find project"),
                             QApplication::translate("TaskForm",
                                                     "The project this task is assigned to does not exist or has been renamed, please update your favourites to keep using them")
        );
        return;
    }
    ui->project_dropdown->setCurrentIndex(index);

    index = ui->task_dropdown->findText(task->task_name);
    if (index == -1) {
        QMessageBox::warning(this,
                             QApplication::translate("TaskForm", "Could not find task"),
                             QApplication::translate("TaskForm",
                                                     "The task selected does not exist or has been renamed, please update your favourites to keep using them")
        );
        return;
    }
    ui->task_dropdown->setCurrentIndex(index);

    ui->notes_text_edit->setText(task->note);

    this->exec();
}
