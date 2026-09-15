# Countdown
English | [中文](README.md)
## Introduction

A countdown desktop application built with Kirigami / Qt 6, used to record and track important days such as birthdays, anniversaries, and deadlines.

The UI is built with KDE Frameworks 6 Kirigami components and QML. It's simple and modern, supports Linux and Windows, and has Android support reserved.

Main features:

- Create, edit, and delete countdown items, intuitively showing the days remaining until the target date (or the days that have passed)
- Three countdown modes: no repeat, monthly repeat, yearly repeat
- Remind on the set date, optionally on the day itself, one day before, or a custom number of days before
- In-app update check (automatic check is off by default)
- Supports Chinese and English UI (i18n)
- Silent (minimized) launch parameter and a Debug log switch

Data is saved locally as JSON; settings are stored in `~/.config/yancat/Countdown.conf`.

## Gallery

<p align="center">
  <table>
    <tr>
      <td><img src="readme_img/en/app.png" width="600"></td>
      <td><img src="readme_img/en/new.png" width="600"></td>
      <td><img src="readme_img/en/reminder.png" width="600"></td>
    </tr>
  </table>
</p>

## Installation

Linux binaries are provided in the Releases.

The following packages are usually required at runtime:

ArchLinux:
```bash
sudo pacman -S qt6-base qt6-declarative kirigami kcoreaddons kiconthemes breeze qqc2-desktop-style qt6-wayland
```

Ubuntu / Debian:
```bash
sudo apt install libqt6core6t64 libqt6qml6 libqt6quick6 \
  libkf6coreaddons6 libkf6iconthemes6 libkf6kirigami6 \
  breeze qml6-module-qtquick-controls qml6-module-qt-labs-platform \
  qml6-module-qtquick-layouts qt6-wayland
```
Only **Ubuntu 24.10 (Oracular) or newer** is supported.

## Technical Details

Project structure:

```plaintext
countdown
├── .github
├── .gitignore
├── android                           # Not used yet
├── CHANGELOG.md                      # Changelog
├── CMakeLists.txt
├── com.countdown.desktop             # Linux desktop file
├── Countdown.py                      # KDE Craft blueprint
├── exclude_list.txt                  # Packaging blacklist
├── LICENSE
├── README.md
├── README_en.md
├── readme_img
├── src
│   ├── countdowndata.cpp             # Handles the data to be displayed
│   ├── datediff.cpp                  # Computes years/months/days from days
│   ├── debug.cpp                     # Debug related
│   ├── main.cpp                      # Main program
│   ├── manager.cpp                   # Manages data
│   ├── reminder.cpp                  # Countdown reminders
│   ├── updater.cpp                   # Checks and applies updates
│   ├── include                       # Header files
│   └── resources
│       ├── icon                      # Icons
│       └── qml
│           ├── AboutPageWindow.qml   # About window
│           ├── DetailsWindow.qml     # Details window
│           ├── Main.qml              # Main page
│           ├── ReminderWindow.qml    # Standalone reminder popup for Windows
│           ├── SettingsWindow.qml    # Settings window
│           └── UpdaterWindow.qml     # Update window
└── translations                      # I18n
```

Files (and folders) created by the application:

```plaintext
~
├── .local
│   └── share
│       └── yancat
│           └── Countdown
│               └── countdowns.json # Countdown data
└── .config
    └── yancat
        └── Countdown.conf          # Settings
```

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for the detailed changes in each version.

## Planned

- [ ] WebDAV cloud sync of items
- [ ] Desktop tiles/widget
- [ ] Optional app background
- [ ] Android support (too difficult, no plan for now)

This program was developed with AI assistance.

> **Note:** This English README may not be the latest version. Please refer to the [Chinese README](README.md) (or the source code) for the most up-to-date information.
