#include "js_app.h"
#include "ecma.h"
#include "lvgl/hml_lvgl_module.h"

#define JS_APP_FILE_NAME "app.js"

const char ABILITY_LIFECYCLE_CALLBACK_ON_CREATE[] = "onCreate";
const char ABILITY_LIFECYCLE_CALLBACK_ON_DESTROY[] = "onDestroy";

js_app_t * app = NULL;

evm_t * js_app_on_create(const char * app_dir){
    if( app ){
        evm_print("frameworks can only have one app now.");
        return NULL;
    }

    app = evm_malloc(sizeof(js_app_t));
    if(!app){
        evm_print("Failed to create app, not enough memory");
        return NULL;
    }

    evm_t * env = (evm_t*)evm_malloc(sizeof(evm_t));
    if( !env ){
        evm_print("Failed to create vm, not enough memory");
        return NULL;
    }
    js_global_e = env;
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, APP_HEAP_SIZE, APP_STACK_SIZE, APP_MODULE_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    if( err ) {
        evm_print("Failed to initialize evm\r\n");
        evm_free(env);
        return NULL;
    }

    err = ecma_module(env);
    if( err ) {
        evm_print("Failed to add ecma module\r\n");
        evm_free(env);
        return NULL;
    }

    err = hml_lvgl_module(env);
    if( err ) {
        evm_print("Failed to add ecma module\r\n");
        evm_free(env);
        return NULL;
    }

    err = js_fwk_module(env);
    if( err ) {
        evm_print("Failed to add framework module\r\n");
        evm_free(env);
        return NULL;
    }

    char app_js_full_path[PATH_LENGTH_MAX];
    sprintf(app_js_full_path, "%s/%s", app_dir, JS_APP_FILE_NAME);

    err = evm_boot(env, (char*)app_js_full_path);
    if( err != ec_ok ){
        evm_print("Failed to boot vm\r\n");
        evm_free(env);
        return NULL;
    }
    evm_start(env);

    app->ability_model = env->scope;
    app->router = evm_malloc(sizeof(js_router_t));
    memset(app->router, 0, sizeof(js_router_t));
    evm_val_t * onCreateFunction = evm_prop_get(env, &app->ability_model, ABILITY_LIFECYCLE_CALLBACK_ON_CREATE, 0);
    if (evm_is_undefined(onCreateFunction)) {
        evm_free(env);
        return NULL;
    }
    evm_run_callback(env, onCreateFunction, &app->ability_model, NULL, 0);
    evm_val_t * object = evm_object_create(env, GC_OBJECT, 1, 0);
    evm_prop_set(env, object, 0, (char*)ROUTER_PAGE_URI, evm_mk_foreign_string((intptr_t)PATH_DEFAULT));
    app->router->app_dir = app_dir;

    js_router_replace(app->router, object);
    return env;
}

void js_app_on_destroy(){
    evm_val_t * onDestroyFunction = evm_prop_get(js_global_e, &app->ability_model, ABILITY_LIFECYCLE_CALLBACK_ON_DESTROY, 0);
    if (evm_is_undefined(onDestroyFunction)) {
        return;
    }
    evm_run_callback(js_global_e, onDestroyFunction, &app->ability_model, NULL, 0);
}
