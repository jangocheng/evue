DEFINES += EVM_LANG_ENABLE_JAVASCRIPT
DEFINES += EVM_LANG_ENABLE_CSS
DEFINES += EVM_LANG_ENABLE_HML
DEFINES += CONFIG_EVM_ECMA_OBJECT
DEFINES += CONFIG_EVM_ECMA_ARRAY
DEFINES += CONFIG_EVM_ECMA_NUMBER
DEFINES += CONFIG_EVM_ECMA_STRING
DEFINES += CONFIG_EVM_ECMA_MATH
DEFINES += CONFIG_EVM_ECMA_DATE
DEFINES += CONFIG_EVM_ECMA_TIMEOUT

DEFINES += LV_LVGL_H_INCLUDE_SIMPLE

INCLUDEPATH += $$PWD/../../lvgl
INCLUDEPATH += $$PWD/../../include
INCLUDEPATH += $$PWD/../../modules/hml
INCLUDEPATH += $$PWD/../../modules/ecma/inc
INCLUDEPATH += $$PWD/../../frameworks
INCLUDEPATH += $$PWD/../evuemain
INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD

include($$PWD/../lvgl.pri)
include($$PWD/../../frameworks/frameworks.pri)
include($$PWD/../../modules/hml/hml.pri)
include($$PWD/../../modules/ecma/qt/ecma.pri)
include($$PWD/../evuemain/evuemain.pri)
include($$PWD/../fonts/fonts.pri)
