QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = evue-qt5


unix {
    exists($$PWD/../../lib/x86_64-linux-gnu/libhml.a) {
        LIBS += $$PWD/../../lib/x86_64-linux-gnu/libhml.a
    }
}

win32{
    exists($$PWD/../../lib/x86_64-window-mingw/libhml.a) {
        LIBS += $$PWD/../../lib/x86_64-window-mingw/libhml.a
    }
}



include($$PWD/../libevue/libevue.pri)

SOURCES += \
    assets/img_cogwheel_alpha16.c \
    assets/img_cogwheel_argb.c \
    assets/img_cogwheel_chroma_keyed.c \
    assets/img_cogwheel_indexed16.c \
    assets/img_cogwheel_rgb.c \
    assets/img_hand.c \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../lv_conf.h \
    mainwindow.h

RESOURCES += \
    ../skin.qrc


