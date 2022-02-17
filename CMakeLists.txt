cmake_minimum_required(VERSION 3.22)
project(harvesttimer_qt_cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_executable(harvest-timer
        src/addtaskform.cpp
        src/addtaskform.h
        src/datelabel.cpp
        src/datelabel.h
        src/favourite.h
        src/favourites.cpp
        src/favourites.h
        src/favouritewidget.cpp
        src/favouritewidget.h
        src/harvesthandler.cpp
        src/harvesthandler.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/task.h
        src/taskwidget.cpp
        src/taskwidget.h
        resources/images.qrc
        src/mainwindow.ui
        src/taskwidget.ui
        src/favourites.ui
        src/favouritewidget.ui
        src/addtaskform.ui src/settingsmanager.cpp src/settingsmanager.h src/project.h src/harvesttask.h)

find_package(Qt6 REQUIRED
        Core
        Gui
        Widgets
        Network
        )

target_link_libraries(harvest-timer Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network)
