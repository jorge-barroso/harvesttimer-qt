#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTime>
#include <QTextStream>
#include <ostream>

struct Task {
    long long project_id;
    long long task_id;
    long long time_entry_id;
    QString client_name;
    QString project_name;
    QString task_name;
    QTime time_tracked;
    QString note;
    bool started{false};
    QDate date{QDate::currentDate()};

    // data starting here won't be saved to streams
    bool favourited{false};

    static const QString values_separator;
    static const QString end_line_separator;

    friend QTextStream &operator<<(QTextStream &stream, const Task &task) {
        stream << task.project_id << values_separator << task.task_id << values_separator << task.time_entry_id
               << values_separator << task.client_name << values_separator << task.project_name << values_separator
               << task.task_name << values_separator << task.time_tracked.toString() << values_separator << task.note
               << values_separator << task.started << values_separator
               << task.date.toString() << end_line_separator;

        return stream;
    }

    friend QTextStream &operator>>(QTextStream &stream, Task &task) {
        QString contents{stream.readLine()};
        QStringList values{contents.split(values_separator)};

        int i{-1};
        task.project_id = values[++i].toLongLong();
        task.task_id = values[++i].toLongLong();
        task.time_entry_id = values[++i].toLongLong();
        if (values.length() == 10) {
            task.client_name = values[++i];
        }
        task.project_name = values[++i];
        task.task_name = values[++i];
        task.time_tracked = QTime::fromString(values[++i]);
        task.note = values[++i];
        task.started = QVariant(values[++i]).toBool();
        task.date = QDate::fromString(values[++i]);

        return stream;
    }

    bool operator==(const Task &other_task) const {
        return project_id == other_task.project_id &&
               task_id == other_task.task_id &&
               time_entry_id == other_task.time_entry_id &&
               client_name == other_task.client_name &&
               project_name == other_task.project_name &&
               task_name == other_task.task_name;
    }

    bool operator!=(const Task &other_task) const {
        return !(other_task == *this);
    }

    [[nodiscard]] QString get_project_label() const {
        if(client_name.isNull() || client_name.isEmpty()) {
            return project_name;
        }

        return project_name + " (" + client_name + ")";
    }
};

#endif // TASK_H
