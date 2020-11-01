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

#include "js_router.h"
#include "js_page_state_machine.h"

evm_val_t js_router_add(js_router_t * router, evm_val_t * object, int index){
    evm_val_t * uriValue = evm_prop_get(js_global_e, object, ROUTER_PAGE_URI, 0);
    if( !uriValue || !evm_is_string(uriValue) ) return EVM_VAL_UNDEFINED;
    char * uri = evm_2_string(uriValue);

    js_page_sm_t *newSm = evm_malloc(sizeof(js_page_sm_t));
    if (newSm == NULL) {
        evm_print("malloc state machine memory heap failed.");
        return EVM_VAL_UNDEFINED;
    }

    router->current_sm = newSm;
    router->infos[index].uri = uri;
    router->infos[index].sm = newSm;

    evm_val_t dirVal = evm_mk_foreign_string((intptr_t)router->app_dir);
    evm_prop_append(js_global_e, object, APP_DIR, &dirVal);

    // init new state machine
    evm_val_t jsRes = EVM_VAL_UNDEFINED;

    int8_t res = js_page_sm_init(js_global_e, newSm, object, &jsRes);
    if (!res) {
        evm_free(newSm);
        return jsRes;
    }

    js_page_sm_change_state(router->current_sm, INIT_STATE);
    js_page_sm_change_state(router->current_sm, SHOW_STATE);
    router->sm_index = index;
    return jsRes;
}

evm_val_t js_router_push(js_router_t * router, evm_val_t * object){
    evm_val_t * uriValue = evm_prop_get(js_global_e, object, ROUTER_PAGE_URI, 0);
    if( !uriValue || !evm_is_string(uriValue) ) return EVM_VAL_UNDEFINED;
    char * uri = evm_2_string(uriValue);

    for(int i = 0; i < APP_ROUTER_STACK_SIZE; i++){
        if( router->infos[i].uri == NULL ){
            return js_router_add(router, object, i);
        }

        if( !strcmp(router->infos[i].uri, uri) ){
            js_page_sm_show_page(router->infos[i].sm);
            router->current_sm = router->infos[i].sm;
            router->sm_index = i;
            return EVM_VAL_UNDEFINED;
        }
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t js_router_replace(js_router_t * router, evm_val_t * object){
    if( router->current_sm ){
        js_page_sm_change_state(router->current_sm, BACKGROUND_STATE);
        js_page_sm_change_state(router->current_sm, DESTROY_STATE);
        evm_free(router->current_sm);
        router->infos[router->sm_index].uri = NULL;
        router->infos[router->sm_index].sm = NULL;
    }

    return js_router_add(router, object, router->sm_index);
}
