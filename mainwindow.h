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
    QDate app_date;
    HarvestHandler harvest;
//	std::map<QDate, std::vector<Task>> tasks;
//	Task* currentTask;

};
#endif // MAINWINDOW_H
