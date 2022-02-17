#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTime>

struct Task {
    QString project;
    QString task_name;
    QTime time_tracked;
};

#endif // TASK_H
