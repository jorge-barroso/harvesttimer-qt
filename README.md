[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/U6U6ECJZ8)
[![liberapay](https://img.shields.io/liberapay/receives/jorge-barroso.svg?logo=liberapay)](https://liberapay.com/jorge-barroso/donate)

# Harvest Timer Qt
A [Qt](https://www.qt.io/) app that integrates with [Harvest](https://www.getharvest.com) to keep track of the time invested in your projects.

Ideal for [KDE](https://kde.org/) desktops but 100% suitable for any other OS and DE!

## First Run
You will need to log in the first time you use the application.

The first run system needs some work, but still, you should be able to use it to log in and come back to the app once you're done.
If that seems to fail, you can just reopen the app, and you will see how everything has been done: you are logged in and ready to go.

## App Screenshots
### Main Window
![Main Window](/assets/main_window.png?raw=true "the main window of the application")

### Adding a Task
![Adding a Task](/assets/add_task_window.png?raw=true "the form to add a new task from the applications")

### System Tray Icon
![System Tray](/assets/systray_icon.png?raw=true "the app giving you quick access ")


## Installation
You can build the app for yourself, or simply grab the prebuilt binary in the [releases section](https://github.com/jorge-barroso/harvesttimer-qt/releases).
Additionally, the distros listed below have the app in their repos.

### Binary File
If you are happy with just grabbing the binary file, you can do so from the latest release in the [releases section](https://github.com/jorge-barroso/harvesttimer-qt/releases), download the binary pack and use it on your system as you prefer.

### DEB File
You can install the deb package attached on every [release](https://github.com/jorge-barroso/harvesttimer-qt/releases).

### Arch Linux (AUR-based distros)
You can use your favourite installation method (yay, software centre, etc.) to install [harvest-timer-qt](https://aur.archlinux.org/packages/harvest-timer-qt)

### Dependencies
The app has very few dependencies, but you need to bear these in mind especially if you are going to build it from sources:

#### Build Dependencies
- Modern C++ compiler ([g++](https://gcc.gnu.org/), [clang](http://clang.org/))
- [CMake](https://cmake.org/)

#### Runtime Dependencies
- Qt6 Base package, some namings for this, depending on distros, are
    - Ubuntu: [qt6-base-dev](https://packages.ubuntu.com/search?keywords=qt6-base-dev&searchon=names&suite=all&section=all)
    - Fedora: [qt6-qtbase](https://src.fedoraproject.org/rpms/qt6-qtbase)
    - Arch: [qt6-base](https://archlinux.org/packages/extra/x86_64/qt6-base/)

