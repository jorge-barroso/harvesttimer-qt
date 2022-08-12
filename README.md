[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/U6U6ECJZ8)
[![liberapay](https://img.shields.io/liberapay/receives/jorge-barroso.svg?logo=liberapay)](https://liberapay.com/jorge-barroso/donate)

# Harvest Timer Qt
A Qt app that integrates with Harvest (https://www.getharvest.com) to keep track of the time invested in your projects

# Installation
You can build the app for yourself, or simply grab the prebuilt binary in the [releases section](https://github.com/jorge-barroso/harvesttimer-qt/releases).
Additionally, the following distros have the app in their repos:

## Arch Linux (AUR-based distros)
You can use your favourite installation method (yay, software centre, etc.) to install [harvest-timer-qt](https://aur.archlinux.org/packages/harvest-timer-qt)

## Dependencies
The app has very few dependencies, but you need to bear these in mind especially if you are going to build it from sources:

### Build Dependencies
- Modern C++ compiler (g++, clang)
- CMake

### Runtime Dependencies
- Qt6 Base package, some namings for this, depending on distros, are
  - Ubuntu: qt6-base