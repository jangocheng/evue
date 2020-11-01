QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TEMPLATE = app
TARGET = evuesimulator

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

unix {
    HEADERS +=
    SOURCES +=
}

win32{
    HEADERS += findwindow_win32.h
    SOURCES += findwindow_win32.cpp
}

SOURCES += \
    embedapp.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    embedapp.h \
    mainwindow.h \
    transparentdialog.h


RESOURCES += \
    ../skin.qrc \

win32{
    RC_ICONS = "../skin/logo.ico"
}
