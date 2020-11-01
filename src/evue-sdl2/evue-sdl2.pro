TEMPLATE = app
TARGET = evue


LIBS += -L$$PWD -lSDL2 -lm


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
include($$PWD/../lvgl_drivers.pri)
INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD

HEADERS += \
    ../lv_conf.h \
    ../lv_drv_conf.h

SOURCES += \
    main.c \
    mouse_cursor_icon.c
