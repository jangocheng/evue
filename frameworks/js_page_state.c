#include "js_page_state.h"

void js_page_state_handle_init_state(js_page_sm_t * sm){
    int curr_state = js_page_sm_get_current_state(sm);
    if( curr_state == UNDEFINED_STATE ){
        js_page_sm_set_current_state(sm, INIT_STATE);
        evm_print("undefined state -> init state\n");
        js_page_sm_bind_parameters(sm);
        js_page_sm_eval_page(sm);
        js_page_sm_invoke_page_life_cycle_callback(sm, PAGE_LIFECYCLE_ON_INIT);
        js_page_sm_change_state(sm, READY_STATE);
    } else {
        evm_print("current state(%d) is invalid when changing undefined state to init state.", curr_state);
    }
}

void js_page_state_handle_ready_state(js_page_sm_t * sm){
    int curr_state = js_page_sm_get_current_state(sm);
    if (curr_state == INIT_STATE) {
        evm_print("init state -> ready state\n");
        js_page_sm_render_page(sm);
        js_page_sm_invoke_page_life_cycle_callback(sm, PAGE_LIFECYCLE_CALLBACK_ON_READY);
        js_page_sm_set_current_state(sm, READY_STATE);
    } else {
        evm_print("current state(%d) is invalid when changing init state to ready state.",
                    curr_state);
    }
}

void js_page_state_handle_show_state(js_page_sm_t * sm){
    int curr_state = js_page_sm_get_current_state(sm);
    if ((curr_state == READY_STATE) || (curr_state == BACKGROUND_STATE)) {
        evm_print("current state(%d) -> show state\n", curr_state);
        js_page_sm_show_page(sm);
        js_page_sm_invoke_page_life_cycle_callback(sm, PAGE_LIFECYCLE_CALLBACK_ON_SHOW);
        js_page_sm_set_current_state(sm, SHOW_STATE);
    } else {
        evm_print("current state(%d) is invalid when changing ready state to show state.",
                    curr_state);
    }
}

void js_page_state_handle_background_state(js_page_sm_t * sm){
    int curr_state = js_page_sm_get_current_state(sm);
    if ((curr_state == READY_STATE) || (curr_state == SHOW_STATE)) {
        evm_print("current state(%d) -> background state\n", curr_state);
        js_page_sm_hide_page(sm);
        js_page_sm_invoke_page_life_cycle_callback(sm, PAGE_LIFECYCLE_CALLBACK_ON_HIDE);
        js_page_sm_set_current_state(sm, BACKGROUND_STATE);
    } else {
        evm_print("current state(%d) is invalid when changing show state to background state.",
                    curr_state);
    }
}

void js_page_state_handle_destroy_state(js_page_sm_t * sm){
    int curr_state = js_page_sm_get_current_state(sm);
    // any normal state can jump to destory state
    if ( (curr_state >= INIT_STATE) ) {
        evm_print("current state(%d) -> destroy state\n", curr_state);
        js_page_sm_invoke_page_life_cycle_callback(sm, PAGE_LIFECYCLE_CALLBACK_ON_DESTROY);
        js_page_sm_release_history_page_resource(sm);
        js_page_sm_set_current_state(sm, DESTROY_STATE);
    } else {
        evm_print("current state(%d) is invalid when changing show state to destroy state.",
                    curr_state);
    }
}
