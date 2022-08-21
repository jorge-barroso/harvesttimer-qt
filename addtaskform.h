#ifndef ADDTASKFORM_H
#define ADDTASKFORM_H

#include "task.h"
#include "harvestproject.h"
#include "customtaskdialog.h"

namespace Ui
{
	class AddTaskForm;
}

class AddTaskForm : public CustomTaskDialog
{
	Q_OBJECT

	public:
		explicit AddTaskForm(QWidget* parent = nullptr);

		~AddTaskForm() override;

		void add_projects(const std::vector<HarvestProject>& projects);

	signals:

		void task_started(Task* task);

		void task_to_favourites(Task* task);

	public slots:

		void add_task_from_favourites(const Task* task);

	private slots:

		void on_start_button_clicked();

		void on_favourite_button_clicked();

		void on_project_dropdown_currentIndexChanged(int index);

	private:
		Ui::AddTaskForm* ui;

		Task* build_task();

		QTime zero_time{ QTime(0, 0) };

		void reset_inputs() override;
};

#endif // ADDTASKFORM_H
