/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、hml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version : 1.0
**  Email   : scriptiot@aliyun.com
**  Website : https://github.com/scriptiot
**  Licence: MIT Licence
****************************************************************************/
#include "hml_lvgl_module.h"
#include "hml_module.h"
#include "widgets/lv_marquee.h"
/********** Global Variables ***********/

static evm_t * global_e;

/********** Callback ***********/
char * lvgl_app_dir(){
    lv_obj_user_data_t data = lv_obj_get_user_data(lv_scr_act());
    if (data) {
       return (char * )lv_obj_get_user_data(lv_scr_act());
    }
    return ".";
}

static bool lvgl_is_obj_type(lv_obj_t * obj, const char* typename)
{
    lv_obj_type_t type;

    lv_obj_get_type(obj, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], typename)) return true;
    }
    return false;
}

static void lvgl_hml_anim_out_all(lv_obj_t * obj, uint32_t time, uint32_t delay) {
    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child) {
        if(child != lv_scr_act()) {
            lv_obj_fade_in(child, time, delay);
        }
        child = lv_obj_get_child_back(obj, child);
    }
}

static void lvgl_hml_run_callback(evm_t * e, lv_obj_t * obj, evm_val_t  viewmodel, const char * name, int argc, evm_val_t * args){
    evm_val_t * m_clicked_fn = evm_prop_get(e, &viewmodel, name, 0);
    if( m_clicked_fn != NULL){
        evm_run_callback(e, m_clicked_fn, &viewmodel, args, argc);
    }
}

static void lv_hml_event_handler(lv_obj_t * obj, lv_event_t event)
{
    evm_val_t * hml = lvgl_get_hml_obj(obj);
    evm_val_t * attributes = evm_prop_get(global_e, hml, "attributes", 0);
    evm_val_t * click = evm_prop_get(global_e, attributes, "onclick", 0);
    evm_val_t * press = evm_prop_get(global_e, attributes, "onpress", 0);
    evm_val_t * release = evm_prop_get(global_e, attributes, "onrelease", 0);
    evm_val_t * change = evm_prop_get(global_e, attributes, "onchange", 0);
    evm_val_t * longpress = evm_prop_get(global_e, attributes, "onlongpress", 0);
    evm_val_t * swipe = evm_prop_get(global_e, attributes, "onswipe", 0);

    hml_object_t *  hml_ext_data = (hml_object_t *)evm_object_get_ext_data(hml);
    evm_val_t viewmodel = hml_ext_data->viewmodel;

    evm_val_t  args[3];
    args[0] = *hml;
    lv_point_t globalClickPoint;
    lv_indev_get_point(lv_indev_get_act(), &globalClickPoint);
    lv_coord_t x = globalClickPoint.x;
    x -= obj->coords.x1;
    lv_coord_t y = globalClickPoint.y;
    y -= obj->coords.y1;
    args[1] = evm_mk_number(x);
    args[2] = evm_mk_number(y);


    if (click && evm_is_string(click) && event == LV_EVENT_CLICKED){
        lvgl_hml_run_callback(global_e, obj, viewmodel, evm_2_string(click), 3, args);
    }else if (press && evm_is_string(press) && event == LV_EVENT_PRESSED){
        lvgl_hml_run_callback(global_e, obj, viewmodel, evm_2_string(press), 3, args);
    }else if (release && evm_is_string(release) && event == LV_EVENT_RELEASED){
        lvgl_hml_run_callback(global_e, obj, viewmodel, evm_2_string(release), 3, args);
    }else if (longpress && evm_is_string(longpress) && event == LV_EVENT_LONG_PRESSED){
        lvgl_hml_run_callback(global_e, obj, viewmodel, evm_2_string(longpress), 3, args);
    }else if(event == LV_EVENT_VALUE_CHANGED) {
        if (lvgl_is_obj_type(obj, "lv_tileview")){
            if (change && evm_is_string(change)){
                lv_coord_t x, y, index = 0;
                lv_tileview_get_tile_act(obj, &x, &y);
                if (lv_page_get_scrl_layout(obj) == LV_LAYOUT_ROW_MID){
                    index = x;
                }else if (lv_page_get_scrl_layout(obj) == LV_LAYOUT_COLUMN_MID){
                    index = y;
                }
                evm_val_t  changeargs[2];
                changeargs[0] = *hml;
                changeargs[1] = evm_mk_number(index);

                evm_val_t * attributes_index = evm_prop_get(global_e, attributes, "index", 0);
                if (!attributes_index){
                    evm_prop_append(global_e, attributes, "index", &changeargs[0]);
                }
                attributes_index = evm_prop_get(global_e, attributes, "index", 0);
                if (attributes_index && evm_is_integer(attributes_index)){
                    int _index = evm_2_integer(attributes_index);
                    if (index == _index){
                        lv_tileview_ext_t * ext = lv_obj_get_ext_attr(obj);
                        if (index ==  ext->valid_pos_cnt - 1 && index!=0){
                            lv_tileview_set_tile_act(obj, 0, 0, LV_ANIM_OFF);
                        }else if (index == 0){
                            if (lv_page_get_scrl_layout(obj) == LV_LAYOUT_ROW_MID){
                                lv_tileview_set_tile_act(obj, ext->valid_pos_cnt - 1, 0, LV_ANIM_OFF);
                            }else if (lv_page_get_scrl_layout(obj) == LV_LAYOUT_COLUMN_MID){
                                lv_tileview_set_tile_act(obj, 0, ext->valid_pos_cnt - 1, LV_ANIM_OFF);
                            }
                        }
                    }else{
                        evm_prop_set_value(global_e, attributes, "index", changeargs[0]);
                        lvgl_hml_run_callback(global_e, obj, viewmodel, evm_2_string(change), 2, changeargs);
                    }
                }else if (!attributes_index){
                    evm_prop_append(global_e, attributes, "index", &changeargs[0]);
                }
            }
        }
    }else if (event == LV_EVENT_GESTURE){
        lv_gesture_dir_t  dircetion  = lv_indev_get_gesture_dir(lv_indev_get_act());
        evm_val_t  dircetion_args[2];
        dircetion_args[0] = *hml;

    }else if (event == LV_EVENT_DRAG_BEGIN){
        if (lvgl_is_obj_type(obj, "lv_tileview")){

        }
    }else if (swipe && evm_is_string(swipe) && event == LV_EVENT_PRESSING){
        lv_gesture_dir_t  dircetion  = lv_indev_get_gesture_dir(lv_indev_get_act());
        evm_val_t  dircetion_args[2];
        dircetion_args[0] = *hml;
        if (dircetion == LV_GESTURE_DIR_LEFT){
            int key = evm_str_insert(global_e, "left", 0);
            dircetion_args[1] = *evm_string_pool_get(global_e, key);
        }else if (dircetion == LV_GESTURE_DIR_TOP){
            int key = evm_str_insert(global_e, "top", 0);
            dircetion_args[1] = *evm_string_pool_get(global_e, key);
        }else if (dircetion == LV_GESTURE_DIR_RIGHT){
            int key = evm_str_insert(global_e, "right", 0);
            dircetion_args[1] = *evm_string_pool_get(global_e, key);
        }else if (dircetion == LV_GESTURE_DIR_BOTTOM){
            int key = evm_str_insert(global_e, "bottom", 0);
            dircetion_args[1] = *evm_string_pool_get(global_e, key);
        }

        lvgl_hml_run_callback(global_e, obj, viewmodel, evm_2_string(swipe), 2, dircetion_args);
    }
}

static void lvgl_hml_TextInput_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        evm_val_t v = evm_mk_object(lv_obj_get_user_data(obj));
        evm_hml_write_value(global_e, &v, "text", evm_mk_foreign_string((intptr_t)lv_textarea_get_text(obj)));
//        lvgl_hml_run_callback(global_e, &v, "onTextEdited", 0, NULL);
    }
}

/********** div ***********/

static evm_val_t  hml_Div(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_cont_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);

    lv_obj_set_click(obj, true);
    lv_obj_set_parent_event(obj, true);
//    lv_obj_set_drag(obj, true);
//    lv_obj_set_drag_throw(obj, true);
//    lv_obj_set_gesture_parent(obj, true);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
//    lv_obj_set_gesture_parent(obj, true);
    if (lvgl_is_obj_type(parent, "lv_tileview")){
        lv_tileview_add_element(parent, obj);
        lv_obj_set_drag(obj, true);
        lv_obj_set_gesture_parent(obj, true);
        evm_val_t * attributes = evm_prop_get(e, v, "attributes", 0);
        evm_val_t * vertical = evm_prop_get(e, attributes, "vertical", 0);
//        if (vertical){
//            if (!strcmp(evm_2_string(vertical), "true")){
////                lv_obj_set_size();
//            }else{
////                lv_obj_set_size();
//            }
//        }

    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_List(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_page_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);

    lv_page_set_scrollable_fit2(obj, LV_FIT_PARENT, LV_FIT_TIGHT);
    lv_page_set_scrl_layout(obj, LV_LAYOUT_CENTER);
    lv_page_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_DRAG);
    lv_page_set_edge_flash(obj, true);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_ListItem(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_cont_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);

    lv_page_glue_obj(obj, true);

    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Stack(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_cont_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Swiper(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_tileview_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);

    lv_tileview_set_edge_flash(obj, true);

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * vertical = evm_prop_get(e, attributes, "vertical", 0);
    bool isVertical = false;
    if (vertical){

        if (evm_2_boolean(vertical)){
            isVertical = true;
        }else{
            isVertical = false;
        }
    }
    evm_val_t * elements = evm_prop_get(e, p, "elements", 0);
    int count = evm_prop_len(elements) + 1;
    static lv_point_t valid_pos[48];
    if (isVertical){
        lv_page_set_scrl_layout(obj, LV_LAYOUT_COLUMN_MID);
        for (int i=0; i < count; i++){
            lv_point_t pageIndex;
            pageIndex.x = 0;
            pageIndex.y = i;
            valid_pos[i] = pageIndex;
        }
    }else{
        lv_page_set_scrl_layout(obj, LV_LAYOUT_ROW_MID);
        for (int i=0; i < count; i++){
            lv_point_t pageIndex;
            pageIndex.x = i;
            pageIndex.y = 0;
            valid_pos[i] = pageIndex;
        }
    }

    lv_tileview_set_valid_positions(obj, valid_pos, count);
    lv_page_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_DRAG);

    evm_val_t * index = evm_prop_get(e, attributes, "index", 0);
    if (index && evm_is_string(index)){
        lv_point_t activeIndex;
        if (isVertical){
            activeIndex.x = 0;
            activeIndex.y = atoi(evm_2_string(index));
        }else{
            activeIndex.x = atoi(evm_2_string(index));
            activeIndex.y = 0;
        }
        lv_tileview_set_tile_act(obj, activeIndex.x, activeIndex.y, LV_ANIM_ON);
    }
    if (index && evm_is_integer(index)){
        lv_point_t activeIndex;
        if (isVertical){
            activeIndex.x = 0;
            activeIndex.y = evm_2_integer(index);
        }else{
            activeIndex.x = evm_2_integer(index);
            activeIndex.y = 0;
        }
        lv_tileview_set_tile_act(obj, activeIndex.x, activeIndex.y, LV_ANIM_ON);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Image(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_img_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);
    lv_obj_set_parent_event(obj, true);
    lv_img_set_auto_size(obj, true);

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * src = evm_prop_get(e, attributes, "src", 0);

    if (src && evm_is_string(src)){
        char * appdir = lvgl_app_dir();
        char img[evm_string_len(src) +  strlen(appdir) + 3];
        sprintf(img, "P:%s/%s", lvgl_app_dir() , evm_2_string(src));
        if (strstr(img, ".bin")){
            lv_img_set_src(obj, img);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_ImageAnimator(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_cont_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Progress(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    bool isHorizontal = false;
    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * type = evm_prop_get(e, attributes, "type", 0);
    if (type && evm_is_string(type)){
        if (!strcmp(evm_2_string(type), "horizontal")){
            obj = (lv_obj_t*)lv_bar_create(parent, NULL);
            isHorizontal = true;
        }else{
            obj = (lv_obj_t*)lv_arc_create(parent, NULL);
        }
    }else{
        obj = (lv_obj_t*)lv_arc_create(parent, NULL);
    }

    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    evm_val_t * percent = evm_prop_get(e, attributes, "percent", 0);
    if (!isHorizontal){
        lv_arc_set_adjustable(obj, true);
        lv_arc_set_bg_angles(obj, 0, 360);
        lv_arc_set_angles(obj, 270, 90);

        if (percent && evm_is_number(percent)){
            lv_arc_set_angles(obj, 0, 360 * evm_2_integer(percent) / 100);
        }

        if (percent && evm_is_string(percent)){
            lv_arc_set_angles(obj, 0, 360 * atoi(evm_2_string(percent)) / 100);
        }
    }else{
        lv_bar_set_range(obj, 0, 100);
        if (percent && evm_is_number(percent)){
            lv_bar_set_value(obj, evm_2_integer(percent), LV_ANIM_ON);
        }

        if (percent && evm_is_string(percent)){
            lv_bar_set_value(obj, atoi(evm_2_string(percent)), LV_ANIM_ON);
        }
    }

    return EVM_VAL_UNDEFINED;
}

/********** Text ***********/
static evm_val_t  hml_Text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1) parent = evm_hml_object_get_pointer(v);
    obj = (lv_obj_t*)lv_label_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);

    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);

    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);
    lv_obj_set_parent_event(obj, true);
    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_Text_text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v) ){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_label_set_text(obj, evm_2_string(v));
    }else if ( argc >= 1 && evm_is_integer(v) ){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        char text[32];
        sprintf(text, "%ld", evm_2_integer(v));
        lv_label_set_text(obj, text);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Marquee(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_marquee_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    lv_marquee_set_long_mode(obj, LV_LABEL_LONG_SROLL_CIRC);     /*Circular scroll*/

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * scrollamount = evm_prop_get(e, attributes, "scrollamount", 0);
    if (scrollamount && evm_is_string(scrollamount)){
        lv_marquee_set_anim_speed(obj, atoi(evm_2_string(scrollamount)));
    }

    if (scrollamount && evm_is_integer(scrollamount)){
        lv_marquee_set_anim_speed(obj, evm_2_integer(scrollamount));
    }

    evm_val_t * value = evm_prop_get(e, p, "value", 0);
    if (value && evm_is_string(value)){
        lv_marquee_set_text(obj, evm_2_string(value));
    }

    lv_point_t size = lv_marquee_get_text_size(obj);
    lv_obj_set_size(obj, size.x, size.y);

    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_Chart(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_chart_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    lv_obj_set_size(obj, 200, 150);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(obj, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add a faded are effect*/
    lv_obj_set_style_local_bg_opa(obj, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(obj, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(obj, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(obj, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/


    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(obj, LV_COLOR_RED);
    lv_chart_series_t * ser2 = lv_chart_add_series(obj, LV_COLOR_GREEN);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next(obj, ser1, 31);
    lv_chart_set_next(obj, ser1, 66);
    lv_chart_set_next(obj, ser1, 10);
    lv_chart_set_next(obj, ser1, 89);
    lv_chart_set_next(obj, ser1, 63);
    lv_chart_set_next(obj, ser1, 56);
    lv_chart_set_next(obj, ser1, 32);
    lv_chart_set_next(obj, ser1, 35);
    lv_chart_set_next(obj, ser1, 57);
    lv_chart_set_next(obj, ser1, 85);

    /*Directly set points on 'ser2'*/
    ser2->points[0] = 92;
    ser2->points[1] = 71;
    ser2->points[2] = 61;
    ser2->points[3] = 15;
    ser2->points[4] = 21;
    ser2->points[5] = 35;
    ser2->points[6] = 35;
    ser2->points[7] = 58;
    ser2->points[8] = 31;
    ser2->points[9] = 53;

    lv_chart_refresh(obj); /*Required after direct set*/

    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Input(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * type = evm_prop_get(e, attributes, "type", 0);
    evm_val_t * value = evm_prop_get(e, attributes, "value", 0);
    evm_val_t * checked = evm_prop_get(e, attributes, "checked", 0);

    if (type && evm_is_string(type)  && !strcmp(evm_2_string(type), "button")){
        obj = (lv_obj_t*)lv_btn_create(parent, NULL);
        if (value && evm_is_string(value)){
            lv_obj_t * label = lv_label_create(obj, NULL);
            lv_label_set_text(label, evm_2_string(value));
        }
        if (checked && evm_is_string(checked)){
            lv_btn_set_checkable(obj, true);
            if (!strcmp(evm_2_string(checked), "true")){
                lv_btn_set_state(obj, LV_BTN_STATE_CHECKED_RELEASED);
            }else{
                lv_btn_set_state(obj, LV_BTN_STATE_RELEASED);
            }
        }

    }else if (type && evm_is_string(type)  && !strcmp(evm_2_string(type), "checkbox")){
        obj = (lv_obj_t*)lv_checkbox_create(parent, NULL);
        if (value && evm_is_string(value)){
            lv_checkbox_set_text(obj, evm_2_string(value));
        }
        if (checked && evm_is_string(checked)){
            lv_btn_set_checkable(obj, true);
            if (!strcmp(evm_2_string(checked), "true")){
                lv_checkbox_set_state(obj, LV_BTN_STATE_CHECKED_RELEASED);
            }else{
                lv_checkbox_set_state(obj, LV_BTN_STATE_RELEASED);
            }
        }
    }else if (type&& evm_is_string(type)  && !strcmp(evm_2_string(type), "radio")){
        obj = (lv_obj_t*)lv_checkbox_create(parent, NULL);
    }
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_Slider(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_slider_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * min = evm_prop_get(e, attributes, "min", 0);
    evm_val_t * max = evm_prop_get(e, attributes, "max", 0);
    evm_val_t * value = evm_prop_get(e, attributes, "value", 0);

    lv_slider_set_range(obj, lv_slider_get_min_value(obj), 100);

    if (min && evm_is_number(min)){
        lv_slider_set_range(obj, evm_2_integer(min), lv_slider_get_max_value(obj));
    }
    if (max && evm_is_number(max)){
        lv_slider_set_range(obj, lv_slider_get_min_value(obj), evm_2_integer(max));
    }
    if (value && evm_is_number(value)){
        lv_slider_set_value(obj, evm_2_integer(value), LV_ANIM_ON);
    }

    if (min && evm_is_string(min)){
        lv_slider_set_range(obj, atoi(evm_2_string(min)), lv_slider_get_max_value(obj));
    }
    if (max && evm_is_string(max)){
        lv_slider_set_range(obj, lv_slider_get_min_value(obj), atoi(evm_2_string(max)));
    }
    if (value && evm_is_string(value)){
        lv_slider_set_value(obj, atoi(evm_2_string(value)), LV_ANIM_ON);
    }


    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_Switch(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_switch_create(parent, NULL);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * checked = evm_prop_get(e, attributes, "checked", 0);
    if (checked && evm_is_string(checked)){
        if (!strcmp(evm_2_string(checked), "true")){
            lv_switch_on(obj, LV_ANIM_ON);
        }else{
            lv_switch_off(obj, LV_ANIM_OFF);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_PickerView(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_hml_object_get_pointer(v);
    }
    obj = (lv_obj_t*)lv_roller_create(parent, NULL);
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_hml_object_set_pointer(p, obj);
    lvgl_hml_obj_add_style(obj, p);
    lv_obj_set_event_cb(obj, lv_hml_event_handler);
    lv_obj_set_click(obj, true);

    evm_val_t * attributes = evm_prop_get(e, p, "attributes", 0);
    evm_val_t * type = evm_prop_get(e, attributes, "type", 0);
    evm_val_t * range = evm_prop_get(e, attributes, "range", 0);
    evm_val_t * selected = evm_prop_get(e, attributes, "selected", 0);

    if (selected && evm_is_string(selected)){
        lv_roller_set_selected(obj, atoi(evm_2_string(selected)), LV_ANIM_ON);
        int count = evm_list_len(range);
        int len = 0;
        for(int i=0; i < count; i++){
            len += evm_string_len(evm_list_get(global_e, range, i));
        }
        native_print(global_e, NULL, 1, range);
        char options[2*len -1 ];
        for(int i=0; i < count; i++){
            sprintf(options, "%s\n", evm_2_string( range + i));
        }
//        lv_roller_set_options(obj, options, LV_ROLLER_MODE_INIFINITE);
    }

    return EVM_VAL_UNDEFINED;
}

/********** Item ***********/
static evm_val_t  hml_obj_set_display(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        if (!strcmp(evm_2_string(v), "none")){
            lv_obj_set_hidden(obj, true);
        }else{
            lv_obj_set_hidden(obj, false);
        }
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_obj_set_x(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_x(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_y(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_y(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_pos(evm_t * e, evm_val_t *p, int argc, evm_val_t * x, evm_val_t * y){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(x) && evm_is_number(y)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_pos(obj, evm_2_integer(x), evm_2_integer(y));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_width(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_width(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_size(evm_t * e, evm_val_t *p, int argc, evm_val_t * width, evm_val_t * height){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(width) && evm_is_number(height)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_size(obj, evm_2_integer(width), evm_2_integer(height));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_height(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_height(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_alignitems(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        if (!strcmp(evm_2_string(v), "center")){
            lv_cont_set_layout(obj, LV_LAYOUT_ROW_MID);
        }else if (!strcmp(evm_2_string(v), "flex-start")){
            lv_cont_set_layout(obj, LV_LAYOUT_ROW_TOP);
        }else if (!strcmp(evm_2_string(v), "flex-end")){
            lv_cont_set_layout(obj, LV_LAYOUT_ROW_BOTTOM);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_flexdirection(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        if (!strcmp(evm_2_string(v), "row")){
            lv_cont_set_layout(obj, LV_LAYOUT_ROW_MID);
        }else if (!strcmp(evm_2_string(v), "column")){
            lv_cont_set_layout(obj, LV_LAYOUT_COLUMN_MID);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_backgroundcolor(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && (evm_is_string(v) || evm_is_number(v)) ){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_color_t color = lvgl_hml_style_get_color(e, v);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        if (lvgl_is_obj_type(obj, "lv_label")){
            lv_style_set_bg_opa(style, LV_STATE_DEFAULT, LV_OPA_COVER);
        }
        lv_style_set_bg_color(style, LV_STATE_DEFAULT, color);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_gradient_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 2 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_color_t color = lvgl_hml_style_get_color(e, v);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        lv_style_set_bg_grad_color(style, LV_STATE_DEFAULT, color);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_obj_set_gradient_dir(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        if (!strcmp(evm_2_string(v), "horizontal")){
            lv_style_set_bg_grad_dir(style, LV_STATE_DEFAULT, LV_GRAD_DIR_HOR);
        }else{
            lv_style_set_bg_grad_dir(style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        }
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_gradient_main_stop(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        lv_style_set_bg_main_stop(style, LV_STATE_DEFAULT, evm_2_integer(v));
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_gradient_grad_stop(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        lv_style_set_bg_grad_stop(style, LV_STATE_DEFAULT, evm_2_integer(v));
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_obj_set_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        int opacity = 100 * evm_2_double(v);
        if (lvgl_is_obj_type(obj, "lv_label")){
            lv_style_set_text_opa(style, LV_STATE_DEFAULT, lvgl_hml_style_get_opa(opacity));
        }else if (lvgl_is_obj_type(obj, "lv_img")){
            lv_style_set_image_opa(style, LV_STATE_DEFAULT, lvgl_hml_style_get_opa(opacity));
        }else{
            lv_style_set_bg_opa(style, LV_STATE_DEFAULT, lvgl_hml_style_get_opa(opacity));
        }
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t  hml_obj_set_bordercolor(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_color_t color = lvgl_hml_style_get_color(e, v);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        lv_style_set_border_color(style, LV_STATE_DEFAULT, color);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_borderwidth(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        lv_style_set_border_width(style, LV_STATE_DEFAULT, evm_2_integer(v));
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_borderradius(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        lv_style_set_radius(style, LV_STATE_DEFAULT, evm_2_integer(v));
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_margin(evm_t * e, evm_val_t *p, int argc, evm_val_t * v, lv_style_property_t type){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        if (type == LV_STYLE_MARGIN_RIGHT + 1){
            lv_style_set_margin_all(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_MARGIN_LEFT){
            lv_style_set_margin_left(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_MARGIN_TOP){
            lv_style_set_margin_top(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_MARGIN_RIGHT){
            lv_style_set_margin_right(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_MARGIN_BOTTOM){
            lv_style_set_margin_bottom(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }

    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_padding(evm_t * e, evm_val_t *p, int argc, evm_val_t * v, lv_style_property_t type){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        if (type == LV_STYLE_PAD_INNER){
            lv_style_set_pad_all(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_PAD_LEFT){
            lv_style_set_pad_left(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_PAD_TOP){
            lv_style_set_pad_top(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_PAD_RIGHT){
            lv_style_set_pad_right(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }else if (type == LV_STYLE_PAD_BOTTOM){
            lv_style_set_pad_bottom(style, LV_STATE_DEFAULT, evm_2_integer(v));
        }
        lv_style_set_pad_inner(style, LV_STATE_DEFAULT, 0);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_text_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        evm_val_t * hml = lvgl_get_hml_obj(obj);
        evm_val_t * name = evm_prop_get(global_e, hml, "name", 0);
        const char * bname = evm_2_string(name);
        lv_color_t color = lvgl_hml_style_get_color(e, v);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);

        if (!strcmp(bname, "progress")){
            static lv_style_t style_indic;
            lv_style_init(&style_indic);
            lv_style_set_bg_color(&style_indic, LV_STATE_DEFAULT, color);
            lv_style_list_t * list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            _lv_style_list_add_style(list, &style_indic);
        }else{
            lv_style_set_text_color(style, LV_STATE_DEFAULT, color);
            lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_text_align(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        if (lvgl_is_obj_type(obj, "lv_label")){
            if (!strcmp(evm_2_string(v), "left")){
                lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);
            }else if (!strcmp(evm_2_string(v), "center")){
                lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
            }else if (!strcmp(evm_2_string(v), "right")){
                lv_label_set_align(obj, LV_LABEL_ALIGN_RIGHT);
            }else{
                lv_label_set_align(obj, LV_LABEL_ALIGN_AUTO);
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_text_font_size(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        int font_size = evm_2_integer(v);
        lv_style_set_text_font(style, LV_STATE_DEFAULT, lvgl_hml_style_get_font_size(font_size));
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_text_letter_space(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_style_t * style = lvgl_hml_obj_get_style(obj);
        int letter_space = evm_2_integer(v);
        lv_style_set_text_letter_space(style, LV_STATE_DEFAULT, letter_space);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
    }
    return EVM_VAL_UNDEFINED;
}

LV_FONT_DECLARE(HYQiHei_65S)
LV_FONT_DECLARE(simsun_30)
LV_FONT_DECLARE(simsun_20)
static evm_val_t  hml_obj_set_text_font_family(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 2 && evm_is_string(v) && evm_is_integer(v+1)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);

        if (!strcmp(evm_2_string(v), "HYQiHei-65S")){
            lv_style_set_text_font(lvgl_hml_obj_get_style(obj), LV_OBJ_PART_MAIN, &HYQiHei_65S);
        }else if (!strcmp(evm_2_string(v), "simsun") && evm_2_integer(v+1) == 20){
            lv_style_set_text_font(lvgl_hml_obj_get_style(obj), LV_OBJ_PART_MAIN, &simsun_20);
        }else if (!strcmp(evm_2_string(v), "simsun") && evm_2_integer(v+1) == 30){
            lv_style_set_text_font(lvgl_hml_obj_get_style(obj), LV_OBJ_PART_MAIN, &simsun_30);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_text_overflow(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        if (lvgl_is_obj_type(obj, "lv_label")){
            if (!strcmp(evm_2_string(v), "clip")){
                lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
            }else if (!strcmp(evm_2_string(v), "ellipsis")){
                lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            }else if (!strcmp(evm_2_string(v), "longexpand")){
                lv_label_set_long_mode(obj, LV_LABEL_LONG_EXPAND);
            }else if (!strcmp(evm_2_string(v), "longbreak")){
                lv_label_set_long_mode(obj, LV_LABEL_LONG_BREAK);
            }else if (!strcmp(evm_2_string(v), "scroll")){
                lv_label_set_long_mode(obj, LV_LABEL_LONG_SROLL);
                lv_label_set_anim_speed(obj, 40);
            }else if (!strcmp(evm_2_string(v), "scrollcircle")){
                lv_label_set_long_mode(obj, LV_LABEL_LONG_SROLL_CIRC);
                lv_label_set_anim_speed(obj, 40);
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_text_animation_speed(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        if (lvgl_is_obj_type(obj, "lv_label")){
            lv_label_set_anim_speed(obj, evm_2_integer(v));
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_fade_in(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 2 && evm_is_number(v) && evm_is_number(v+1)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_fade_in(obj, evm_2_integer(v), evm_2_integer(v+1));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_fade_out(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 2 && evm_is_number(v) && evm_is_number(v+1)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_fade_out(obj, evm_2_integer(v), evm_2_integer(v+1));
    }
    return EVM_VAL_UNDEFINED;
}

static void lv_obj_set_bg_color(lv_obj_t * obj, lv_anim_value_t v)
{
    lv_style_t * style = lvgl_hml_obj_get_style(obj);
    lvgl_animationcolor_userdata * userdata = (lvgl_animationcolor_userdata *)(obj->ext_attr);
    lv_color_t from = *userdata->from;
    lv_color_t to = *userdata->to;
    double value = (from.full - to.full) * v / 255;
    evm_val_t color_v = evm_mk_number(value);
    lv_color_t c = lvgl_hml_style_get_color(global_e, &color_v);
    lv_color_t c1 = lv_color_mix(from, to, v);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, c1);
//    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, style);
}

static evm_val_t  hml_obj_set_animation(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 9){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_anim_t a;
        lv_anim_t * b = &a;

        int delay = evm_2_integer(v+1);
        int duration = evm_2_integer(v+2);
        int iteration_count = evm_2_integer(v+3);
        const char * timing_function = evm_2_string(v+4);
        const char * fill_mode = evm_2_string(v+5);
        const char * property = evm_2_string(v+6);
        lv_anim_value_t from, to;
        if (!strcmp(property, "background-color")){
            lv_color_t _from = lvgl_hml_style_get_color(e, v+7);
            lv_color_t _to = lvgl_hml_style_get_color(e, v+8);
            lvgl_animationcolor_userdata * userdata = evm_malloc(sizeof(lvgl_animationcolor_userdata));
            userdata->from = &_from;
            userdata->to = &_to;
            obj->ext_attr  = userdata;
            from = 0;
            to = 255;
        }else{
            if (evm_is_number(v+7) && evm_is_number(v+8)){
                from = (lv_anim_value_t)evm_2_integer(v+7);
                to = (lv_anim_value_t)evm_2_integer(v+8);
            }else{
                return EVM_VAL_UNDEFINED;
            }        }

        lv_anim_path_t path;
        lv_anim_path_init(&path);
        if (!strcmp(timing_function, "linear")){
            lv_anim_path_set_cb(&path, lv_anim_path_linear);
        }else if (!strcmp(timing_function, "ease-in")){
            lv_anim_path_set_cb(&path, lv_anim_path_ease_in);
        }else if (!strcmp(timing_function, "ease-out")){
            lv_anim_path_set_cb(&path, lv_anim_path_ease_out);
        }else if (!strcmp(timing_function, "ease-in-out")){
            lv_anim_path_set_cb(&path, lv_anim_path_ease_in_out);
        }else if (!strcmp(timing_function, "overshoot")){
            lv_anim_path_set_cb(&path, lv_anim_path_overshoot);
        }else if (!strcmp(timing_function, "bounce")){
            lv_anim_path_set_cb(&path, lv_anim_path_bounce);
        }else if (!strcmp(timing_function, "step")){
            lv_anim_path_set_cb(&path, lv_anim_path_step);
        }else{
            path = lv_anim_path_def;
        }

        if (!strcmp(property, "background-color")){
            path = lv_anim_path_def;
        }

        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_time(&a, duration);
        lv_anim_set_delay(&a, delay);
        if (!strcmp(property, "x")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_x);
        }else if (!strcmp(property, "y")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        }else if (!strcmp(property, "width")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_width);
        }else if (!strcmp(property, "height")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_height);
        }else if (lvgl_is_obj_type(obj, "lv_img") && !strcmp(property, "zoom")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
        }else if (lvgl_is_obj_type(obj, "lv_img") && !strcmp(property, "angle")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_angle);
            lv_img_set_pivot(obj, 200, 200);
        }else if (!strcmp(property, "background-color")){
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_bg_color);
        }

        if (lvgl_is_obj_type(obj, "lv_img") && !strcmp(property, "zoom")){
            lv_anim_set_values(&a, LV_IMG_ZOOM_NONE, to);
        }else{
            lv_anim_set_values(&a, from, to);
        }

        lv_anim_set_path(&a, &path);
        lv_anim_set_repeat_count(&a, iteration_count);
        lv_anim_start(&a);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_visible(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_boolean(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);
        lv_obj_set_hidden(obj, evm_2_intptr(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  hml_obj_set_style(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_object(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_hml_object_get_pointer(p);

    }
    return EVM_VAL_UNDEFINED;
}


/********** Properties ***********/

#define BUTTON_CALLBACKS    {EVM_QML_CALLBACK, "onClicked", NULL},\
                            {EVM_QML_CALLBACK, "onPressed", NULL},\
                            {EVM_QML_CALLBACK, "onReleased", NULL},


#define HML_OBJ_PROPERTIES {EVM_QML_INT, "x", (evm_native_fn)hml_obj_set_x}, \
                            {EVM_QML_INT, "y", (evm_native_fn)hml_obj_set_y}, \
                            {EVM_QML_INT, "width", (evm_native_fn)hml_obj_set_width}, \
                            {EVM_QML_INT, "height", (evm_native_fn)hml_obj_set_height}, \
                            {EVM_QML_GROUP, "style", (evm_native_fn)hml_obj_set_style}, \
                            {EVM_QML_DOUBLE, "opacity", (evm_native_fn)hml_obj_set_opacity},\
                            {EVM_QML_BOOLEAN, "visible", (evm_native_fn)hml_obj_set_visible}, \

static evm_hml_value_reg_t hml_obj_properties[] = {
    HML_OBJ_PROPERTIES
    {0, NULL, NULL}
};

static evm_hml_value_reg_t hml_text_properties[] = {
    HML_OBJ_PROPERTIES
    {EVM_QML_STRING, "text", (evm_native_fn)hml_Text_text},
    {0, NULL, NULL}
};

/* 赋值 appdir 给lv_scr_act()， 后续获取资源文件路径，需要根据 appdir 进行路径拼接*/
static evm_val_t hml_lv_init(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_set_user_data(lv_scr_act(), (lv_obj_user_data_t)evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

/* 设置display屏幕的样式*/
static evm_val_t hml_lv_set_display_style(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    if( argc >= 1 && evm_is_object(v)){
        evm_val_t * body = evm_prop_get(e, v, "body", 0);
        if (body){
            evm_val_t * bg_img = evm_prop_get(e, body, "background-image", 0);
            if (bg_img && evm_is_string(bg_img)){
                char * appdir = lvgl_app_dir();
                char img[evm_string_len(bg_img) +  strlen(appdir) + 3];
                sprintf(img, "P:%s/%s", lvgl_app_dir() , "./logo.bin");
                if (strstr(img, ".bin")){
                    lv_disp_set_bg_image(lv_disp_get_default(), img);
                }
            }

            evm_val_t * background_color = evm_prop_get(e, body, "background-color", 0);
            if (background_color){
                lv_disp_set_bg_color(lv_disp_get_default(), lvgl_hml_style_get_color(e, background_color));
            }

            evm_val_t * opacity = evm_prop_get(e, body, "opacity", 0);
            if (opacity && evm_is_number(opacity)) {
                lv_disp_set_bg_opa(lv_disp_get_default(), evm_2_double(opacity));
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t hml_lv_set_style(evm_t * e, evm_val_t *hml_obj, int argc, evm_val_t * css_obj){
    EVM_UNUSED(e);
    evm_val_t * display = evm_prop_get(e, css_obj, "display", 0);
    if (display) {
        hml_obj_set_display(e, hml_obj, 1, display);
    }

    evm_val_t * x = evm_prop_get(e, css_obj, "x", 0);
    evm_val_t * y = evm_prop_get(e, css_obj, "y", 0);

    if (x && y){
        hml_obj_set_pos(e, hml_obj, 1, x, y);
    }else if (x && !y){
        hml_obj_set_x(e, hml_obj, 1, x);
    }else if (!x && y){
        hml_obj_set_y(e, hml_obj, 1, y);
    }

    evm_val_t * left = evm_prop_get(e, css_obj, "left", 0);
    evm_val_t * top = evm_prop_get(e, css_obj, "top", 0);

    if (left && top){
        hml_obj_set_pos(e, hml_obj, 1, left, top);
    }else if (left && !top){
        hml_obj_set_x(e, hml_obj, 1, left);
    }else if (!left && top){
        hml_obj_set_y(e, hml_obj, 1, top);
    }

    evm_val_t * width = evm_prop_get(e, css_obj, "width", 0);
    evm_val_t * height = evm_prop_get(e, css_obj, "height", 0);

    if (width && height) {
        hml_obj_set_size(e, hml_obj, 1, width, height);
    }else if (width && !height){
        hml_obj_set_width(e, hml_obj, 1, width);
    }else if (!width && height){
        hml_obj_set_height(e, hml_obj, 1, height);
    }

    evm_val_t * alignitems = evm_prop_get(e, css_obj, "align-items", 0);
    if (alignitems) {
        hml_obj_set_alignitems(e, hml_obj, 1, alignitems);
    }

    evm_val_t * flexdirection = evm_prop_get(e, css_obj, "flex-direction", 0);
    if (flexdirection) {
        hml_obj_set_flexdirection(e, hml_obj, 1, flexdirection);
    }

    evm_val_t * backgroundcolor = evm_prop_get(e, css_obj, "background-color", 0);
    if (backgroundcolor) {
        hml_obj_set_backgroundcolor(e, hml_obj, 1, backgroundcolor);
    }

    evm_val_t * opacity = evm_prop_get(e, css_obj, "opacity", 0);
    if (opacity) {
        hml_obj_set_opacity(e, hml_obj, 1, opacity);
    }

    evm_val_t * gradient_color = evm_prop_get(e, css_obj, "gradient-color", 0);
    if (gradient_color) {
        hml_obj_set_gradient_color(e, hml_obj, 1, gradient_color);
    }

    evm_val_t * gradient_dir = evm_prop_get(e, css_obj, "gradient-dir", 0);
    if (gradient_dir) {
        hml_obj_set_gradient_dir(e, hml_obj, 1, gradient_dir);
    }

    evm_val_t * gradient_main_stop = evm_prop_get(e, css_obj, "gradient-main-stop", 0);
    if (gradient_main_stop) {
        hml_obj_set_gradient_main_stop(e, hml_obj, 1, gradient_main_stop);
    }

    evm_val_t * gradient_grad_stop = evm_prop_get(e, css_obj, "gradient-grad-stop", 0);
    if (gradient_grad_stop) {
        hml_obj_set_gradient_grad_stop(e, hml_obj, 1, gradient_grad_stop);
    }

    evm_val_t * bordercolor = evm_prop_get(e, css_obj, "border-color", 0);
    if (bordercolor) {
        hml_obj_set_bordercolor(e, hml_obj, 1, bordercolor);
    }

    evm_val_t * borderwidth = evm_prop_get(e, css_obj, "border-width", 0);
    if (borderwidth) {
        hml_obj_set_borderwidth(e, hml_obj, 1, borderwidth);
    }

    evm_val_t * borderradius = evm_prop_get(e, css_obj, "border-radius", 0);
    if (borderradius) {
        hml_obj_set_borderradius(e, hml_obj, 1, borderradius);
    }

    evm_val_t * margin = evm_prop_get(e, css_obj, "margin", 0);
    if (margin) {
        hml_obj_set_margin(e, hml_obj, 1, margin, LV_STYLE_MARGIN_RIGHT + 1);
    }

    evm_val_t * marginleft = evm_prop_get(e, css_obj, "margin-left", 0);
    if (marginleft) {
        hml_obj_set_margin(e, hml_obj, 1, marginleft, LV_STYLE_MARGIN_LEFT);
    }

    evm_val_t * margintop = evm_prop_get(e, css_obj, "margin-top", 0);
    if (margintop) {
        hml_obj_set_margin(e, hml_obj, 1, margintop, LV_STYLE_MARGIN_TOP);
    }

    evm_val_t * marginright = evm_prop_get(e, css_obj, "margin-right", 0);
    if (marginright) {
        hml_obj_set_margin(e, hml_obj, 1, marginright, LV_STYLE_MARGIN_RIGHT);
    }

    evm_val_t * marginbottom = evm_prop_get(e, css_obj, "margin-bottom", 0);
    if (marginbottom) {
        hml_obj_set_margin(e, hml_obj, 1, marginbottom, LV_STYLE_MARGIN_BOTTOM);
    }

    evm_val_t * padding = evm_prop_get(e, css_obj, "padding", 0);
    if (padding) {
        hml_obj_set_padding(e, hml_obj, 1, padding, LV_STYLE_PAD_INNER); //
    }

    evm_val_t * paddingleft = evm_prop_get(e, css_obj, "padding-left", 0);
    if (paddingleft) {
        hml_obj_set_padding(e, hml_obj, 1, paddingleft, LV_STYLE_PAD_LEFT);
    }

    evm_val_t * paddingtop = evm_prop_get(e, css_obj, "padding-top", 0);
    if (paddingtop) {
        hml_obj_set_padding(e, hml_obj, 1, paddingtop, LV_STYLE_PAD_TOP);
    }

    evm_val_t * paddingright = evm_prop_get(e, css_obj, "padding-right", 0);
    if (paddingright) {
        hml_obj_set_padding(e, hml_obj, 1, paddingright, LV_STYLE_PAD_RIGHT);
    }

    evm_val_t * paddingbottom = evm_prop_get(e, css_obj, "padding-bottom", 0);
    if (paddingbottom) {
        hml_obj_set_padding(e, hml_obj, 1, paddingbottom, LV_STYLE_PAD_BOTTOM);
    }

    evm_val_t * color = evm_prop_get(e, css_obj, "color", 0);
    if (color) {
        hml_obj_set_text_color(e, hml_obj, 1, color);
    }

    evm_val_t * textalign = evm_prop_get(e, css_obj, "text-align", 0);
    if (textalign) {
        hml_obj_set_text_align(e, hml_obj, 1, textalign);
    }

    evm_val_t * fontsize = evm_prop_get(e, css_obj, "font-size", 0);
    if (fontsize) {
        hml_obj_set_text_font_size(e, hml_obj, 1, fontsize);
    }

    evm_val_t * letter_spacing = evm_prop_get(e, css_obj, "letter-spacing", 0);
    if (letter_spacing) {
        hml_obj_set_text_letter_space(e, hml_obj, 1, letter_spacing);
    }

    evm_val_t * text_overflow = evm_prop_get(e, css_obj, "text-overflow", 0);
    if (text_overflow) {
        hml_obj_set_text_overflow(e, hml_obj, 1, text_overflow);
    }

    evm_val_t * animation_speed = evm_prop_get(e, css_obj, "animation-speed", 0);
    if (animation_speed) {
        hml_obj_set_text_animation_speed(e, hml_obj, 1, animation_speed);
    }

    evm_val_t * font_family = evm_prop_get(e, css_obj, "font-family", 0);

    if (!fontsize){
        evm_val_t size = evm_mk_number(30);
        fontsize = &size;
    }

    if (font_family && fontsize) {
        evm_val_t  args[2];
        args[0] = *font_family;
        args[1] = *fontsize;
        hml_obj_set_text_font_family(e, hml_obj, 2, args);
    }

    evm_val_t * fade_in_time = evm_prop_get(e, css_obj, "fade-in-time", 0);
    evm_val_t * fade_in_delay = evm_prop_get(e, css_obj, "fade-in-delay", 0);
    if (!fade_in_delay){
        evm_val_t delay = evm_mk_number(0);
        fade_in_delay = &delay;
    }
    if (fade_in_time && evm_is_integer(fade_in_time)) {
        evm_val_t  args[2];
        args[0] = *fade_in_time;
        args[1] = *fade_in_delay;
        hml_obj_set_fade_in(e, hml_obj, 2, args);
    }

    evm_val_t * fade_out_time = evm_prop_get(e, css_obj, "fade-out-time", 0);
    evm_val_t * fade_out_delay = evm_prop_get(e, css_obj, "fade-in-delay", 0);
    if (!fade_out_delay){
        evm_val_t delay = evm_mk_number(0);
        fade_out_delay = &delay;
    }
    if (fade_out_time && evm_is_integer(fade_out_time)) {
        evm_val_t  args[2];
        args[0] = *fade_out_time;
        args[1] = *fade_out_delay;
        hml_obj_set_fade_out(e, hml_obj, 1, args);
    }

    evm_val_t * animation_name = evm_prop_get(e, css_obj, "animation-name", 0);
    if (animation_name && evm_is_string(animation_name)){
        evm_val_t * animation_delay = evm_prop_get(e, css_obj, "animation-delay", 0);
        if (!animation_delay){
            evm_val_t delay = evm_mk_number(0);
            animation_delay = &delay;
        }

        evm_val_t * animation_duration = evm_prop_get(e, css_obj, "animation-duration", 0);
        if (!animation_duration){
            evm_val_t duration = evm_mk_number(0);
            animation_duration = &duration;
        }

        evm_val_t * animation_iteration_count = evm_prop_get(e, css_obj, "animation-iteration-count", 0);
        if (!animation_iteration_count){
            evm_val_t count = evm_mk_number(0);
            animation_iteration_count = &count;
        }

        evm_val_t * animation_timing_function = evm_prop_get(e, css_obj, "animation-timing-function", 0);
        if (!animation_timing_function){
            evm_val_t * function = evm_string_pool_get(global_e, evm_str_insert(global_e, "linear", 0));
            animation_timing_function = function;
        }

        evm_val_t * animation_fill_mode = evm_prop_get(e, css_obj, "animation-fill-mode", 0);
        if (!animation_fill_mode){
            evm_val_t * mode = evm_string_pool_get(global_e, evm_str_insert(global_e, "none", 0));
            animation_fill_mode = mode;
        }

        evm_val_t * animation_property = evm_prop_get(e, css_obj, "animation-property", 0);
        if (!animation_property){
            evm_val_t * property = evm_string_pool_get(global_e, evm_str_insert(global_e, "y", 0));
            animation_property = property;
        }

        evm_val_t * animation_property_from = evm_prop_get(e, css_obj, "animation-property-from", 0);
        if (!animation_property_from){
            evm_val_t property_from = evm_mk_number(0);
            animation_property_from = &property_from;
        }

        evm_val_t * animation_property_to = evm_prop_get(e, css_obj, "animation-property-to", 0);
        if (!animation_property_to){
            evm_val_t property_to = evm_mk_number(0);
            animation_property_to = &property_to;
        }

        evm_val_t  args[9];
        args[0] = *animation_name;
        args[1] = *animation_delay;
        args[2] = *animation_duration;
        args[3] = *animation_iteration_count;
        args[4] = *animation_timing_function;
        args[5] = *animation_fill_mode;
        args[6] = *animation_property;
        args[7] = *animation_property_from;
        args[8] = *animation_property_to;

        hml_obj_set_animation(e, hml_obj, 9, args);
    }

    return EVM_VAL_UNDEFINED;
}

static evm_val_t hml_show(evm_t * e, evm_val_t *hml, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    obj = evm_hml_object_get_pointer(hml);
    if(obj){
        lv_obj_set_parent(obj, lv_scr_act());
        lv_obj_set_hidden(obj, false);
    }
}

static evm_val_t hml_hide(evm_t * e, evm_val_t *hml, int argc, evm_val_t * v){
    EVM_UNUSED(e);
    EVM_UNUSED(e);
    lv_obj_t * obj = NULL;
    obj = evm_hml_object_get_pointer(hml);
    if(obj){
        lv_obj_set_hidden(obj, true);
    }
}

int hml_lvgl_module(evm_t * e){
    global_e = e;
    evm_hml_object_reg_t hml_objects[] = {
        {"div", NULL, (evm_native_fn)hml_Div, hml_obj_properties},
        {"list", NULL, (evm_native_fn)hml_List, hml_obj_properties},
        {"list-item", NULL, (evm_native_fn)hml_ListItem, hml_obj_properties},
        {"stack", NULL, (evm_native_fn)hml_Stack, hml_obj_properties},
        {"swiper", NULL, (evm_native_fn)hml_Swiper, hml_obj_properties},
        {"image", NULL, (evm_native_fn)hml_Image, hml_obj_properties},
        {"image-animator", NULL, (evm_native_fn)hml_ImageAnimator, hml_obj_properties},
        {"progress", NULL, (evm_native_fn)hml_Progress, hml_obj_properties},
        {"text", NULL, (evm_native_fn)hml_Text, hml_text_properties},
        {"marquee", NULL, (evm_native_fn)hml_Marquee, hml_obj_properties},
        {"chart", NULL, (evm_native_fn)hml_Chart, hml_obj_properties},
        {"input", NULL, (evm_native_fn)hml_Input, hml_obj_properties},
        {"slider", NULL, (evm_native_fn)hml_Slider, hml_obj_properties},
        {"switch", NULL, (evm_native_fn)hml_Switch, hml_obj_properties},
        {"picker-view", NULL, (evm_native_fn)hml_PickerView, hml_obj_properties},
        {NULL, NULL, NULL, NULL}
    };

    evm_hml_register(e, hml_objects);
    int err = hml_module(e, (evm_native_fn)hml_lv_init,
                            (evm_native_fn)hml_lv_set_display_style,
                            (evm_native_fn)hml_lv_set_style,
                            (evm_native_fn)hml_show,
                            (evm_native_fn)hml_hide);

    static lv_style_t scr_style;
    lv_style_init(&scr_style);
    lv_style_set_bg_opa(&scr_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_add_style(lv_scr_act(), LV_OBJ_PART_MAIN, &scr_style);
    return err;
}

