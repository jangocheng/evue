/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot
**  Licence: 个人免费，企业授权
****************************************************************************/

#ifndef JS_PAGE_STATE_MACHINE_H
#define JS_PAGE_STATE_MACHINE_H

#include "evm.h"
#include "hml_module.h"

enum {
    UNDEFINED_STATE = -1,
    INIT_STATE, // 0
    READY_STATE,
    SHOW_STATE,
    BACKGROUND_STATE,
    DESTROY_STATE,
    END_STATE
};

#define PAGE_STATE_SIZE END_STATE

#define PATH_LENGTH_MAX 256
enum {
    FAILED = -1,
    SUCCESS = 0,
    ERROR_BASE = 0x10000,
    ERROR_INPUT_PARAMETER,
    ERROR_MALLOC,
    ERROR_SECURE_USE // use secure function error
};

extern const char JS_INDEX_FILE_PATH[];
extern const char APP_DIR[];
extern const char PATH_DEFAULT[];
extern const char ROUTER_PAGE[];
extern const char ROUTER_PAGE_URI[];
extern const char ROUTER_PAGE_PARAMS[];
extern const char ROUTER_PAGE_PATH[];

extern const char PAGE_LIFECYCLE_ON_INIT[];
extern const char PAGE_LIFECYCLE_CALLBACK_ON_READY[];
extern const char PAGE_LIFECYCLE_CALLBACK_ON_SHOW[];
extern const char PAGE_LIFECYCLE_CALLBACK_ON_HIDE[];
extern const char PAGE_LIFECYCLE_CALLBACK_ON_DESTROY[];

typedef struct js_page_sm_t{
    int8_t current_state;
    char * js_app_dir;
    char * uri;
    evm_val_t native_element;
    evm_val_t object;
    int8_t has_params;
    int8_t is_entire_hidden;

    hml_page_t * page;
}js_page_sm_t;

extern evm_t * js_global_e;

void js_page_sm_set_current_state(js_page_sm_t * sm, int8_t state);
int8_t js_page_sm_get_current_state(js_page_sm_t * sm);
int js_page_sm_generate_js_page_path(js_page_sm_t * sm, const char * uri);
void js_page_sm_register_uri_and_params_to_page(js_page_sm_t * sm, const char * uri, evm_val_t * params);
int8_t js_page_sm_init(evm_t *e, js_page_sm_t * sm, evm_val_t * object, evm_val_t * js_res);
int8_t js_page_sm_bind_uri(js_page_sm_t * sm, evm_val_t * js_res);
void js_page_sm_bind_parameters(js_page_sm_t * sm);
void js_page_sm_change_state(js_page_sm_t * sm, int new_state);
void js_page_sm_eval_page(js_page_sm_t * sm);
void js_page_sm_render_page(js_page_sm_t * sm);
evm_val_t js_page_sm_show_page(js_page_sm_t * sm);
void js_page_sm_hide_page(js_page_sm_t * sm);
void js_page_sm_invoke_page_life_cycle_callback(js_page_sm_t * sm, const char * name);
void js_page_sm_release_root_object(js_page_sm_t * sm);
void js_page_sm_release_history_page_resource(js_page_sm_t * sm);
void js_page_sm_delete_view_model_properties(js_page_sm_t * sm);
void js_page_sm_set_hidden_flag(js_page_sm_t * sm, int8_t flag);


#endif // OHOS_ACELITE_JS_PAGE_STATE_MACHINE_H
