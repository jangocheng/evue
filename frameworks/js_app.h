#ifndef JS_APP_H
#define JS_APP_H

#include "app_config.h"
#include "js_router.h"

typedef struct js_app_t{
    js_router_t * router;
    evm_val_t ability_model;
}js_app_t;

evm_t *js_app_on_create(const char * app_dir);
void js_app_on_destroy();

int js_fwk_module(evm_t * e);

extern js_app_t * app;

#endif
