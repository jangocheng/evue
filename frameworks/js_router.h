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
#ifndef JS_ROUTER_H
#define JS_ROUTER_H

#include "evm.h"
#include "js_page_state_machine.h"
#include "app_config.h"

typedef struct js_router_info_t {
    char * uri;
    js_page_sm_t * sm;
} js_router_info_t;

typedef struct js_router_t{
    js_page_sm_t * previous_sm;
    js_page_sm_t * current_sm;
    int sm_index;
    int8_t hidden;
    js_router_info_t infos[APP_ROUTER_STACK_SIZE];
    const char * app_dir;
} js_router_t;

evm_val_t js_router_replace(js_router_t * router, evm_val_t * object);
evm_val_t js_router_push(js_router_t * router, evm_val_t * object);

#endif
