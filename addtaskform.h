#ifndef ADDTASKFORM_H
#define ADDTASKFORM_H

#include <QDialog>
#include "task.h"
#include "harvestproject.h"

namespace Ui
{
	class AddTaskForm;
}

class AddTaskForm : public QDialog
{
	Q_OBJECT

	public:
		explicit AddTaskForm(QWidget* parent = nullptr);

		~AddTaskForm();

		void add_projects(const std::vector<HarvestProject>& projects);

	signals:

		void task_started(Task* task);

		void task_to_favourites(Task* task);

	private slots:

		void on_start_button_clicked();

		void on_favourite_button_clicked();

		void on_cancel_button_clicked();

        void on_project_dropdown_currentIndexChanged(int index);

private:
		Ui::AddTaskForm* ui;

		Task* build_task();

		std::vector<HarvestProject> projects;

		QTime zero_time{ QTime(0, 0) };

		void reset_and_close();

		void reset_inputs();
};

#endif // ADDTASKFORM_H
