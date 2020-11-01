TEMPLATE = subdirs

SUBDIRS += \
    evue-qt5 \
    evue-sdl2 \
    evuesimulator

evuesimulator.depends = evue-sdl2
