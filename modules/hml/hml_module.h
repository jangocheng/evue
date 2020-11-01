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
#ifndef QML_MOUDLE_H
#define QML_MOUDLE_H

#include "evm.h"

enum EVM_QML_TYPE{
    EVM_QML_INT = 1,
    EVM_QML_DOUBLE = 2,
    EVM_QML_BOOLEAN = 4,
    EVM_QML_STRING = 8,
    EVM_QML_CALLBACK = 16,
    EVM_QML_VALUE = 32,
    EVM_QML_ANY = 64,
    EVM_QML_GROUP = 128,
};

typedef struct evm_hml_value_reg_t{
    int type;
    char * name;
    void* api;
}evm_hml_value_reg_t;

typedef struct evm_hml_object_reg_t{
    char * name;
    char * parent_name;
    evm_native_fn api;
    evm_hml_value_reg_t * values;
}evm_hml_object_reg_t;

typedef struct hml_value_t{
    int type;
    intptr_t name;
    void* api;
} hml_value_t;

typedef struct hml_data_binding_t{
    int16_t src_name;
    evm_val_t tar_obj;
    int is_attribute;
    evm_val_t  function;
    struct hml_data_binding_t * next;
} hml_data_binding_t;

typedef struct hml_object_t{
    void *obj;
    const char* name;
    evm_native_fn api;
    evm_native_fn style_api;
    int values_len;
    hml_value_t * values;
    evm_val_t  viewmodel;
    struct hml_object_t * parent;
} hml_object_t;

typedef struct hml_func_binding_t{
    evm_val_t obj;
    int is_attribute;
    int16_t name;
    evm_val_t func;
    evm_val_t ret;
    struct hml_func_binding_t * next;
} hml_func_binding_t;

typedef struct hml_page_t {
    evm_val_t index_object;
    evm_val_t css_object;
    evm_val_t hml_object;
    evm_val_t view_model;

    hml_func_binding_t * func_bindings;
    hml_data_binding_t * data_bindings;

    evm_native_fn init;
    evm_native_fn show;
    evm_native_fn hide;
} hml_page_t;

void evm_hml_object_set_pointer(evm_val_t * o, void * pointer);
void * evm_hml_object_get_pointer(evm_val_t * o);
char * evm_hml_object_get_name(evm_t* e, evm_val_t * o);
int evm_hml_register(evm_t * e, evm_hml_object_reg_t * regs);
evm_err_t hml_module(evm_t * e, evm_native_fn init, evm_native_fn set_display_style, evm_native_fn set_style, evm_native_fn show, evm_native_fn hide);
void hml_object_gc_init(evm_t * e, evm_val_t * old_self, evm_val_t *new_self);
void evm_hml_write_value(evm_t * e, evm_val_t * src_obj, char * name, evm_val_t v);

void merge_obj(evm_t * e, evm_val_t * target, evm_val_t * src);
int hml_page_init(evm_t * e, hml_page_t * page);
int hml_run(evm_t * e, hml_page_t * page);
int css_run(evm_t * e, evm_val_t * hml, evm_val_t * css_label, evm_val_t * css_class, evm_val_t * css_id, evm_val_t * keyframes);
int js_run(evm_t * e, evm_val_t * hml);
#endif
