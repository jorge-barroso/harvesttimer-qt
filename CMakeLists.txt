cmake_minimum_required(VERSION 3.22)
project(harvesttimer_qt_cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(QT6_MIN_VERSION 6.2)

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
        customtaskdialog.h
        addtaskaction.cpp
        addtaskaction.h
        quitaction.cpp
        quitaction.h
        showorhideaction.cpp
        showorhideaction.h
        logoutaction.cpp
        logoutaction.h
        customcombobox.cpp
        customcombobox.h)

if (QT_VERSION VERSION_LESS 6.4)
    set_property(TARGET harvest PROPERTY AUTORCC_OPTIONS "--no-zstd")
endif()

install(TARGETS harvest
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        )
install(FILES resources/desktop/org.jb.harvest.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
install(FILES resources/desktop/org.jb.harvest.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)

find_package(Qt6 ${QT6_MIN_VERSION} REQUIRED
        Core
        Gui
        Widgets
        Network
        LinguistTools
        Svg
        )

target_link_libraries(harvest Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network Qt6::Svg)

qt6_add_translations(harvest TS_FILES
        harvesttimer-qt_es_ES.ts
        harvesttimer-qt_en_US.ts
        )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
