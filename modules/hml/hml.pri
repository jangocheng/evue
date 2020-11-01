INCLUDEPATH += $$PWD/../lvgl

HEADERS += \
    $$PWD/hml_module.h \
    $$PWD/lvgl/hml_lvgl_module.h \
    $$PWD/lvgl/hml_lvgl_utils.h \
    $$PWD/lvgl/widgets/lv_marquee.h

SOURCES += \
    $$PWD/hml_module.c \
    $$PWD/lvgl/hml_lvgl_module.c \
    $$PWD/lvgl/hml_lvgl_utils.c \
    $$PWD/lvgl/widgets/lv_marquee.c
