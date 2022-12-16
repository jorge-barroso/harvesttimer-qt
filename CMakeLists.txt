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
        maputils.h
        favouriteslist.cpp
        favouriteslist.h
        customscrollarea.cpp
        customscrollarea.h
        task.cpp
        customtrayicon.cpp
        customtrayicon.h
        edittaskform.cpp
        edittaskform.h
        edittaskform.ui
        customtaskdialog.cpp
        customtaskdialog.h addtaskaction.cpp addtaskaction.h quitaction.cpp quitaction.h showorhideaction.cpp showorhideaction.h)

#

install(TARGETS harvest
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        )


find_package(Qt6 REQUIRED
        Core
        Gui
        Widgets
        Network
        LinguistTools
        )

target_link_libraries(harvest Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network)

qt6_add_translations(harvest TS_FILES
        harvesttimer-qt_es_ES.ts
        harvesttimer-qt_en_US.ts
        )