#include "js_page_state_machine.h"
#include "js_page_state.h"

evm_t * js_global_e;

const char PAGE_LIFECYCLE_ON_INIT[] = "onInit";
const char PAGE_LIFECYCLE_CALLBACK_ON_READY[] = "onReady";
const char PAGE_LIFECYCLE_CALLBACK_ON_SHOW[] = "onShow";
const char PAGE_LIFECYCLE_CALLBACK_ON_HIDE[] = "onHide";
const char PAGE_LIFECYCLE_CALLBACK_ON_DESTROY[] = "onDestroy";

const char APP_DIR[] = "appdir";
const char PATH_DEFAULT[] = "pages/index/index";
const char ROUTER_PAGE[] = "$page";
const char ROUTER_PAGE_URI[] = "uri";
const char ROUTER_PAGE_PARAMS[] = "params";
const char ROUTER_PAGE_PATH[] = "path";
const char ROUTER_VIEW_MODEL_PATH[] = "viewmodel";

static char router_temp_path[PATH_LENGTH_MAX];

int8_t js_page_sm_init(evm_t * e, js_page_sm_t * sm, evm_val_t * object, evm_val_t * js_res){
    js_global_e = e;
    sm->current_state = UNDEFINED_STATE;
    sm->js_app_dir =  evm_2_string(evm_prop_get(e, object, APP_DIR, 0));
    sm->uri = NULL;
    sm->native_element = EVM_VAL_UNDEFINED;
    sm->has_params = 0;
    sm->is_entire_hidden = 0;
    sm->object = *object;
    sm->page = evm_malloc(sizeof(hml_page_t));
    return js_page_sm_bind_uri(sm, js_res);
}

void js_page_sm_set_current_state(js_page_sm_t * sm, int8_t state){
    if (state <= UNDEFINED_STATE || state >= END_STATE) {
        evm_print("error input state:%d\n", state);
        return;
    }
    sm->current_state = state;
}

int8_t js_page_sm_get_current_state(js_page_sm_t * sm)
{
    return sm->current_state;
}

void js_page_sm_register_uri_and_params_to_page(js_page_sm_t * sm, const char * uri, evm_val_t * params){
    if (params && !evm_is_undefined(params)) {
        // add $page.path property
        evm_val_t * str = evm_heap_string_create(js_global_e, (char*)uri, strlen(uri));
        evm_prop_set_value(js_global_e, params, (char*)ROUTER_PAGE_PATH, *str);
        evm_prop_set_value(js_global_e, &js_global_e->scope, (char*)ROUTER_PAGE, *params);
        sm->has_params = 1;
    }
}

int8_t js_page_sm_bind_uri(js_page_sm_t * sm, evm_val_t * js_res){
    // check1:object undefined or type error
    if (!evm_is_object(&sm->object)) {
        evm_print("statemachine init failed as object is invalid.\n");
        return 0;
    }
    // check2:object's uri is undefined or is not string
    evm_val_t * uriValue = evm_prop_get(js_global_e, &sm->object, ROUTER_PAGE_URI, 0);
    if (!evm_is_string(uriValue)) {
        evm_print("statemachine init failed as uri is invalid.");
        return 0;
    }
    sm->uri = evm_2_string(uriValue);
    if (sm->uri == NULL) {
        evm_print("statemachine init failed as uri is invalid.\n");
        return 0;
    }
    // check3:object's uri is empty
    if (strlen(sm->uri) == 0) {
        sm->uri = NULL;
        evm_print("statemachine init failed as uri is empty.\n");
        return 0;
    }
    return 1;
}

void js_page_sm_bind_parameters(js_page_sm_t * sm){
    evm_val_t * params = evm_prop_get(js_global_e, &sm->object, ROUTER_PAGE_PARAMS, 0);
    js_page_sm_register_uri_and_params_to_page(sm, sm->uri, params);
}

void js_page_sm_change_state(js_page_sm_t * sm, int new_state){
    if ((new_state <= UNDEFINED_STATE) || (new_state >= END_STATE)) {
        evm_print("error input state:%d", new_state);
        return;
    }
    // jump to new State
    switch (new_state) {
    case INIT_STATE:        js_page_state_handle_init_state(sm);break;
    case READY_STATE:       js_page_state_handle_ready_state(sm);break;
    case SHOW_STATE:        js_page_state_handle_show_state(sm);break;
    case BACKGROUND_STATE:  js_page_state_handle_background_state(sm);break;
    case DESTROY_STATE:     js_page_state_handle_destroy_state(sm);break;
    default:
        break;
    }
}

void js_page_sm_eval_page(js_page_sm_t * sm){
    evm_t * e = js_global_e;
    sprintf(router_temp_path, "%s/%s.js",sm->js_app_dir, sm->uri);
    sm->page->index_object = evm_run_script(e, router_temp_path);
    evm_prop_append(e, &e->root, "data", &sm->page->index_object);
}

static void force_gc(js_page_sm_t * sm, void * data){
    EVM_UNUSED(data);
    evm_heap_gc(js_global_e);
}

void js_page_sm_render_page(js_page_sm_t * sm){
    EVM_UNUSED(sm);
    evm_print("render page\n");
    evm_t * e = js_global_e;

    sprintf(router_temp_path, "%s/%s.hml", sm->js_app_dir, sm->uri);
    sm->page->hml_object = evm_module_load(e, router_temp_path);
    if (evm_is_undefined(&sm->page->hml_object)) {
        evm_print("Eval JS file failed\n");
        sm->js_app_dir = NULL;
        return;
    }

    sprintf(router_temp_path, "%s/%s.css", sm->js_app_dir, sm->uri);
    sm->page->css_object = evm_module_load(e, router_temp_path);

    hml_page_init(e, sm->page);
    evm_val_t * appdir = evm_prop_get(js_global_e, &sm->object, APP_DIR, 0);
    sm->page->init(js_global_e, NULL, 1, appdir);
    hml_run(e, sm->page);

    sprintf(router_temp_path, "%s/%s/viewmodel.js", sm->js_app_dir, ROUTER_VIEW_MODEL_PATH);
    evm_run_script(e, router_temp_path);
}

evm_val_t js_page_sm_show_page(js_page_sm_t * sm){
    EVM_UNUSED(sm);
    evm_t * e = js_global_e;
    sm->page->show(e, &sm->page->hml_object, 0, NULL);
    evm_print("show page\n");
}

void js_page_sm_hide_page(js_page_sm_t * sm){
    EVM_UNUSED(sm);
    evm_t * e = js_global_e;
    sm->page->hide(e, &sm->page->hml_object, 0, NULL);
    evm_print("hide page\n");
}

void js_page_sm_invoke_page_life_cycle_callback(js_page_sm_t * sm, const char * name){
    if ((name == NULL) || (strlen(name) == 0)) {
        evm_print("input parameter is invalid when invoking page life cycle callback.\n");
        return;
    }
    evm_t * e = js_global_e;

    evm_val_t * function = evm_prop_get(e, &sm->page->index_object, name, 0);

    if (function){
        evm_run_callback(e, function, &sm->page->index_object, NULL, 0);
    }
}

void js_page_sm_release_root_object(js_page_sm_t * sm){
    EVM_UNUSED(sm);
}

void js_page_sm_release_history_page_resource(js_page_sm_t * sm){
    EVM_UNUSED(sm);
    evm_heap_gc(js_global_e);
}

void js_page_sm_delete_view_model_properties(js_page_sm_t * sm){
    EVM_UNUSED(sm);
}

void js_page_sm_set_hidden_flag(js_page_sm_t * sm, int8_t flag){
    sm->is_entire_hidden = flag;
}
