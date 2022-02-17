#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>

namespace Ui {
class TaskWidget;
}

class TaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWidget(QWidget *parent = nullptr);
    ~TaskWidget();

private:
    Ui::TaskWidget *ui;
};

#endif // TASKWIDGET_H
