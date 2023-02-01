[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/U6U6ECJZ8)
[![liberapay](https://img.shields.io/liberapay/receives/jorge-barroso.svg?logo=liberapay)](https://liberapay.com/jorge-barroso/donate)
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://paypal.me/jorgebaam)

# Harvest Timer Qt
A [Qt](https://www.qt.io/) app that integrates with [Harvest](https://www.getharvest.com) to keep track of the time invested in your projects.

Ideal for [KDE](https://kde.org/) desktops but 100% suitable for any other OS and DE!

## First Run
You will need to log in the first time you use the application.

On the first run, the app will open the Harvest login page in a browser for you to log in and provide access to the app.
Once you have logged in and see the message "Authentication successful, you may now close this tab" (depending on your system language) the app will load your data and open up ready to use

## App Screenshots
### Main Window
![Main Window](/assets/main_window.png?raw=true "the main window of the application")

### Adding a Task
![Adding a Task](/assets/add_task_window.png?raw=true "the form to add a new task from the applications")

### Favourites
![Favourites Window](/assets/favourites_window.png?raw=true "the application with a separate dialog to quickly choose one of your saved tasks for quick access")

### System Tray Icon
![System Tray](/assets/systray_icon.png?raw=true "the app giving you quick access ")


## Installation
You can build the app for yourself,
or simply grab the prebuilt binary in the [releases section](https://github.com/jorge-barroso/harvesttimer-qt/releases).

Additionally, in Arch-based distros you can find a package in the [AUR repository](https://aur.archlinux.org/packages/harvest-timer-qt).

### Binary File
If you are happy with just grabbing the binary file,
you can do so from the latest release in the [releases section](https://github.com/jorge-barroso/harvesttimer-qt/releases),
download the binary pack and use it on your system as you prefer.

### DEB and RPM Files
You can install both the deb and rpm packages attached on every [release](https://github.com/jorge-barroso/harvesttimer-qt/releases).

### Arch Linux (AUR-enabled distros)
You can use your favourite installation method (yay, software centre, etc.) to install [harvest-timer-qt](https://aur.archlinux.org/packages/harvest-timer-qt)

### Dependencies
The app has very few dependencies, but you need to bear these in mind especially if you are going to build it from sources:

#### Build Dependencies
- Modern C++ compiler ([g++](https://gcc.gnu.org/), [clang](http://clang.org/))
- [CMake](https://cmake.org/)
- Qt6 Tools, the package name of which may vary:
  - Ubuntu: [qt6-tools-dev](https://packages.ubuntu.com/search?keywords=qt6-tools-dev&searchon=names&suite=all&section=all)
  - Fedora: [qt6-qttools](https://src.fedoraproject.org/rpms/qt6-qttools)
  - Arch: [qt6-tools](https://archlinux.org/packages/extra/x86_64/qt6-tools/)

#### Runtime Dependencies
##### Required
- The **Qt6 Base** package, some namings for this, depending on distros, are
  - Ubuntu: [qt6-base-dev](https://packages.ubuntu.com/search?keywords=qt6-base-dev&searchon=names&suite=all&section=all)
  - Fedora: [qt6-qtbase](https://src.fedoraproject.org/rpms/qt6-qtbase)
  - Arch: [qt6-base](https://archlinux.org/packages/extra/x86_64/qt6-base/)
- Similarly, you will need the **Qt6 SVG** package to display icons:
  - Ubuntu: [libqt6svg6](https://packages.ubuntu.com/search?suite=all&section=all&arch=any&keywords=libqt6svg6&searchon=names)
  - Fedora: [qt6-qtsvg](https://src.fedoraproject.org/rpms/qt6-qtsvg)
  - Arch: [qt6-svg](https://archlinux.org/packages/extra/x86_64/qt6-svg)

##### Optional
- If you are running Harvest in a wayland system, installing the **Qt6 Wayland** package to allow Qt to interact directly with the compositor:
  - Ubuntu: [qt6-wayland](https://packages.ubuntu.com/search?suite=all&section=all&arch=any&keywords=qt6-wayland&searchon=names)
  - Fedora: [qt6-qtwayland](https://src.fedoraproject.org/rpms/qt6-qtwayland)
  - Arch: [qt6-wayland](https://archlinux.org/packages/extra/x86_64/qt6-wayland)

## Troubleshoot
### Missing Data
If you have logged in and the list of projects/tasks is empty, login to the web portal and verify that you have projects and tasks assigned to you.

If that fails, you can try re-running the login process by deleting all files under "~/.config/Harvest Timer Qt": ```rm ~/.config/Harvest\ Timer\ Qt/*```