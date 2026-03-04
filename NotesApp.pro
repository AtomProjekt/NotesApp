QT += core gui widgets

CONFIG += c++17
CONFIG += warn_on

TARGET   = NotesApp
TEMPLATE = app

# Скрыть консоль на Windows (только GUI-окно)
win32: CONFIG += windows

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += resources.qrc

# Windows — иконка .exe через .rc файл
win32: RC_FILE = app.rc

# Версия .exe (Windows)
win32: VERSION = 1.0.0
QMAKE_TARGET_DESCRIPTION = "Notes App — Telegram Style"
QMAKE_TARGET_COPYRIGHT   = "2025"

# Linux/macOS — иконка окна через ресурсы
!win32: ICON = logo/myicon.ico

# Оптимизация Release-сборки
QMAKE_CXXFLAGS_RELEASE += -O2
