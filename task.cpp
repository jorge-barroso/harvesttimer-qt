//
// Created by jorge on 8/13/22.
//

#include "task.h"

const QString Task::values_separator = "~~harvest~~";

const QString newline('\n');
const QString cr('\r');

#ifdef __linux__
const QString Task::end_line_separator = newline;
#elif _WIN32
const QString Task::end_line_separator = cr + newline;
#else
const QString Task::end_line_separator = cr;
#endif