#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "addtaskform.h"
#include "favourites.h"
#include "harvesthandler.h"
#include "taskwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QDir & config_dir, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_date_forward_button_clicked();

    void on_date_current_button_clicked();

    void on_date_back_button_clicked();

    void on_new_task_button_clicked();

    void on_favourites_button_clicked();

    void task_started(Task &task);

    void task_to_favourites(Task &task);

	void harvest_handler_ready();

private:
    Ui::MainWindow *ui;
    AddTaskForm task_form;
    Favourites favouritesForm;
    QDate appDate;
    HarvestHandler harvest;
	std::map<QDate, std::vector<Task>> tasks;
	std::map<QDate, std::vector<TaskWidget*>> task_widgets;

	template<typename T, typename U>
	void map_insert_or_create(std::map<T, std::vector<U>>& map, T key, U vector_value);

	void update_task_widgets(QDate date);
};

template<typename T, typename U>
void MainWindow::map_insert_or_create(std::map<T, std::vector<U>>& map, const T key, const U vector_value)
{
	typename std::map<T, std::vector<U>>::iterator lb{ map.lower_bound(key) };
	if(lb != map.end() && !(map.key_comp()(key, lb->first)))
	{
		// key already exists
		lb->second.emplace_back(vector_value);
	}
	else
	{
		// the key does not exist in the map
		// Use lb as a hint to insert,
		// so it can avoid another lookup
		map.insert(lb, typename std::map<T, std::vector<U>>::value_type(key, { vector_value }));
	}
}
#endif // MAINWINDOW_H
