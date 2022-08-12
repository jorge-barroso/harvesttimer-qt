cmake_minimum_required(VERSION 3.22)
project(harvesttimer_qt_cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_executable(harvest
        addtaskform.cpp
        addtaskform.h
        datelabel.cpp
        datelabel.h
        favourite.h
        favourites.cpp
        favourites.h
        favouritewidget.cpp
        favouritewidget.h
        harvesthandler.cpp
        harvesthandler.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        task.h
        taskwidget.cpp
        taskwidget.h
        icons.qrc
        mainwindow.ui
        taskwidget.ui
        favourites.ui
        favouritewidget.ui
        addtaskform.ui
        settingsmanager.cpp
        settingsmanager.h
        harvestproject.h
        harvesttask.h
        tasksscrollarea.cpp
        tasksscrollarea.h
        maputils.cpp
        maputils.h favouritesscrollarea.cpp favouritesscrollarea.h customscrollarea.cpp customscrollarea.h)

install(TARGETS harvest
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)


find_package(Qt6 REQUIRED
        Core
        Gui
        Widgets
        Network
        )

target_link_libraries(harvest Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network)
