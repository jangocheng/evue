#ifndef QML_LVGL_UTILS_H
#define QML_LVGL_UTILS_H

#include <lvgl.h>
#include "hml_module.h"

typedef struct user_data{
    lv_style_t * style;
    evm_val_t  * hml_obj;
} lvgl_userdata;

typedef struct animation_color_data{
    lv_color_t * from;
    lv_color_t * to;
} lvgl_animationcolor_userdata;

lv_style_t * lvgl_hml_obj_add_style(lv_obj_t * obj, evm_val_t * hml);
lv_style_t * lvgl_hml_obj_get_style(lv_obj_t * obj);
evm_val_t * lvgl_get_hml_obj(lv_obj_t * obj);
lv_opa_t lvgl_hml_style_get_opa(int opacity);
lv_color_t lvgl_hml_style_get_color(evm_t * e, evm_val_t * v);
lv_font_t *lvgl_hml_style_get_font_size(int font_size);
lv_layout_t lvgl_hml_get_layout(evm_t * e, evm_val_t * v);
lv_fit_t lvgl_hml_get_fit(evm_t * e, evm_val_t * v);
lv_label_align_t lvgl_hml_ta_get_text_align(evm_t * e, evm_val_t * v);
char * lvgl_hml_get_symbol(evm_t * e, evm_val_t * v);
lv_chart_type_t lvgl_hml_get_ChartType(evm_t * e, evm_val_t * v);

#endif
