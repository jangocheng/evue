#include "hml_module.h"


#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "Invalid argument length");return EVM_VAL_UNDEFINED; }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "Invalid argument type");return EVM_VAL_UNDEFINED; }
#define MEMORY_ERR   { evm_set_err(e, ec_memory, "Insufficient static memory");return NULL; }

extern evm_val_t native_print(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

static hml_page_t * current_page;

static hml_object_t * hml_objects;
int hml_objects_len;

hml_value_t * __hml_value_get(hml_object_t * object, const char * name){
    hml_object_t * parent = object;
    while(parent){
        for(int i = 0; i < parent->values_len; i++){
            hml_value_t * o = parent->values + i;
            if( !strcmp(o->name, name) )
                return o;
        }
        parent = parent->parent;
    }
    return NULL;
}

void evm_hml_write_value(evm_t * e, evm_val_t * src_obj, char * name, evm_val_t v){
    evm_val_t * local_v = evm_prop_get(e, src_obj, name, 0);
    if( !local_v ) return;
    *local_v = v;
}

hml_func_binding_t * __hml_add_func_binding(hml_page_t * page, evm_val_t * obj, int is_attribute, int16_t name, evm_val_t * function){
    static hml_func_binding_t * next_binding = NULL;
    if( page->func_bindings == NULL ){
        page->func_bindings = (hml_func_binding_t*)evm_malloc(sizeof(hml_func_binding_t));
        page->func_bindings->obj = *obj;
        page->func_bindings->is_attribute = is_attribute;
        page->func_bindings->name = name;
        page->func_bindings->func = *function;
        page->func_bindings->next = NULL;
        next_binding = page->func_bindings;
        return page->func_bindings;
    } else {
        hml_func_binding_t * bindings = (hml_func_binding_t*)evm_malloc(sizeof(hml_func_binding_t));
        bindings->obj = *obj;
        bindings->is_attribute = is_attribute;
        bindings->name = name;
        bindings->func = *function;
        bindings->next = NULL;
        next_binding->next = bindings;
        next_binding = bindings;
        return bindings;
    }

}

void __hml_add_binding(hml_page_t * page, uint32_t src_name, evm_val_t tar_obj, int is_attribute, evm_val_t function){
    static hml_data_binding_t * hml_bindings;
    if( page->data_bindings == NULL ){
        page->data_bindings = (hml_data_binding_t*)evm_malloc(sizeof(hml_data_binding_t));
        page->data_bindings->is_attribute = is_attribute;
        page->data_bindings->src_name = src_name;
        page->data_bindings->tar_obj = tar_obj;
        page->data_bindings->function = function;
        page->data_bindings->next = NULL;
        hml_bindings = page->data_bindings;
    } else {
        hml_data_binding_t * bindings = (hml_data_binding_t*)evm_malloc(sizeof(hml_data_binding_t));
        bindings->is_attribute = is_attribute;
        bindings->src_name = src_name;
        bindings->tar_obj = tar_obj;
        bindings->function = function;
        bindings->next = NULL;
        hml_bindings->next = bindings;
        hml_bindings = bindings;
    }
}

//name
static evm_val_t propertyChanged(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(p);
    if( argc != 1 ) return EVM_VAL_UNDEFINED;
    int triggered = 0;
    hml_data_binding_t * bindings = current_page->data_bindings;
    uint16_t name = evm_str_insert(e, evm_2_string(v), 0);
    while(bindings){
        if( name == bindings->src_name ){
            triggered = 1;
            hml_object_t * hml_object = (hml_object_t *)evm_object_get_ext_data( &bindings->tar_obj );
            for(int i = 0; i < hml_object->values_len; i++){
                hml_value_t * value = hml_object->values + i;
                evm_native_fn api = value->api;
                evm_val_t res = evm_run_callback(e, &bindings->function, &bindings->function, NULL, 0);
                api(e, &bindings->tar_obj, 1, &res);
            }
        }
        bindings = bindings->next;
    }
    if( !triggered ){
        hml_func_binding_t * local_func_bindings = current_page->func_bindings;
        while(local_func_bindings){
            evm_val_t ret = evm_run_callback(e, &local_func_bindings->func, &current_page->index_object, NULL, 0);
            if(ret != local_func_bindings->ret){
                local_func_bindings->ret = ret;
                __hml_add_binding(current_page, name, local_func_bindings->obj, local_func_bindings->is_attribute, local_func_bindings->func);

                hml_object_t * hml_obj = evm_object_get_ext_data(&local_func_bindings->obj);
                evm_val_t * value = evm_prop_get(e, &local_func_bindings->obj, "value", 0);
                hml_value_t * hml_value = __hml_value_get(hml_obj, "text");
                if( hml_value) {
                    if(hml_value->api) {
                        evm_native_fn api = hml_value->api;
                        api(e, &local_func_bindings->obj, 1, &ret);
                    }
                }
            }
            local_func_bindings = local_func_bindings->next;
        }
    }
    return EVM_VAL_UNDEFINED;
}


int __hml_object_reg_len(evm_hml_object_reg_t * regs){
    if(regs == NULL) return 0;
    int i = 0;
    while(1){
        if( regs[i].name == NULL) return i;
        i++;
    }
    return i;
}

int __hml_value_reg_len(evm_hml_value_reg_t * regs){
    if(regs == NULL) return 0;
    int i = 0;
    while(1){
        if( regs[i].name == NULL ) return i;
        i++;
    }
    return i;
}

hml_object_t * __evm_hml_find_parent(evm_t *e, char * name, int len){
    for(int i = 0; i < len; i++){
        hml_object_t *obj = hml_objects + i;
        if( evm_str_lookup(e, name) == obj->name ){
            return obj;
        }
    }
    return NULL;
}


int evm_hml_register(evm_t * e, evm_hml_object_reg_t * regs){
    if( !regs ) return ec_err;
    int objects_len = __hml_object_reg_len(regs);
    hml_objects_len = objects_len;
    hml_objects = (hml_object_t*)evm_malloc(objects_len * sizeof(hml_object_t));
    memset(hml_objects, 0, objects_len * sizeof(hml_object_t));
    if(!hml_objects) return ec_memory;
    for(int i = 0; i < objects_len; i++){
        hml_objects[i].name = regs[i].name;
        hml_objects[i].api = regs[i].api;
        if(regs[i].parent_name != NULL)
            hml_objects[i].parent = __evm_hml_find_parent(e, regs[i].parent_name, i);
        int values_len = __hml_value_reg_len(regs[i].values);
        if( values_len ){
            hml_objects[i].values_len = values_len;
            hml_objects[i].values = (hml_value_t*)evm_malloc(values_len * sizeof(hml_value_t));
            if(!hml_objects[i].values) return ec_memory;
            memset(hml_objects[i].values, 0, values_len * sizeof(hml_value_t));
            for(int j = 0; j < values_len; j++){
                hml_objects[i].values[j].api = regs[i].values[j].api;
                hml_objects[i].values[j].name = regs[i].values[j].name;
                hml_objects[i].values[j].type = regs[i].values[j].type;
            }
        }
    }
    return ec_ok;
}

hml_object_t * __hml_object_get(const char * name){
    for(int i = 0; i < hml_objects_len; i++){
        hml_object_t * o = hml_objects + i;
        if( !strcmp(o->name, name) )
            return o;
    }
    return NULL;
}

void evm_hml_object_set_pointer(evm_val_t * o, void * pointer){
    if( !evm_is_object(o) && !evm_is_class(o)) return;
    hml_object_t * hml_obj = (hml_object_t*)evm_object_get_ext_data(o);
    if( !hml_obj ) return;
    hml_obj->obj = pointer;
}

void * evm_hml_object_get_pointer(evm_val_t * o){
    if( !evm_is_object(o) && !evm_is_class(o)) return NULL;
    hml_object_t * hml_obj = (hml_object_t*)evm_object_get_ext_data(o);
    if( !hml_obj ) return NULL;
    return hml_obj->obj;
}

char * evm_hml_object_get_name(evm_t* e, evm_val_t * o){
    if( !evm_is_object(o) && !evm_is_class(o)) return NULL;
    hml_object_t * hml_obj = (hml_object_t*)evm_object_get_ext_data(o);
    if( !hml_obj ) return NULL;
    return evm_string_get(e, hml_obj->name);
}

void * __hml_object_create(evm_t * e, hml_page_t * page, evm_val_t * parent, const char* name, evm_val_t * hml, evm_val_t viewmodel, evm_val_t * css_label, evm_val_t * css_class, evm_val_t * css_id, evm_val_t * keyframes){
    evm_val_t * obj = hml;
    hml_object_t * hml_obj = (hml_object_t*)evm_malloc(sizeof(hml_object_t));
    if(!hml_obj) MEMORY_ERR;
    evm_object_set_ext_data(obj, (intptr_t)hml_obj);

    evm_val_t * vname = evm_prop_get(e, obj, "name", 0);
    if (!vname){
        return NULL;
    }
    const char * class_hash_name = evm_2_string( vname );
    hml_object_t * object_template = __hml_object_get(class_hash_name);
    if( object_template ) {
        memcpy(hml_obj, object_template, sizeof(hml_object_t));
        // create UI object
        if( parent == NULL && hml_obj->api)
            hml_obj->api(e, obj, 0, NULL);
        else if( hml_obj->api )
            hml_obj->api(e, obj, 1, parent);
        // set css style
        css_run(e, obj, css_label, css_class, css_id, keyframes);

        // create js function bindings
        hml_obj->viewmodel = viewmodel;
    } else {
        memset(hml_obj, 0, sizeof(hml_object_t));
    }

    if( parent ){
        hml_object_t * parent_hml_obj = (hml_object_t*)evm_object_get_ext_data(parent);
        hml_value_t * value = __hml_value_get(parent_hml_obj, name);
        if(value && value->api){
            evm_native_fn api = value->api;
            api(e, obj, 1, obj);
        }
    }

    evm_val_t * value = evm_prop_get(e, obj, "value", 0);
    hml_value_t * hml_value = __hml_value_get(hml_obj, "text");
    if( hml_value) {
        evm_val_t argv;
        if( evm_is_script(value) ){
            hml_func_binding_t * binding = __hml_add_func_binding(page, hml, 0, evm_str_insert(e, "value", 1), value);
            evm_val_t * data_value = evm_prop_get(e, &current_page->index_object, "data", 0);
            evm_object_set_scope(value, data_value);
            argv = evm_run_callback(e, value, data_value, NULL, 0);

            binding->ret = argv;
        } else {
            argv = *value;
        }

        if(hml_value->api) {
            evm_native_fn api = hml_value->api;
            api(e, obj, 1, &argv);
        }
    }

    evm_run_callback(e, hml, obj, NULL, 0);

    evm_val_t * elements = evm_prop_get(e, obj, "elements", 0);

    if( elements) {
        for(int i = 0; i < evm_list_len(elements); i++ ){
            evm_val_t * local_v = evm_list_get(e, elements, i);
            if( evm_is_object(local_v) ){
                __hml_object_create(e, page, obj, "component", local_v, viewmodel, css_label, css_class, css_id, keyframes);
            }
        }
    }
    return hml_obj->obj;
}

evm_native_fn hml_init_api;
evm_native_fn hml_set_display_style;
evm_native_fn hml_set_style;
evm_native_fn hml_show;
evm_native_fn hml_hide;

int hml_page_init(evm_t * e, hml_page_t * page){
    current_page = page;
    evm_object_set_ext_data(&page->index_object, (intptr_t)page);
    evm_object_set_ext_data(&page->hml_object, (intptr_t)page);
    evm_object_set_ext_data(&page->css_object, (intptr_t)page);
    page->init = hml_init_api;
    page->show = hml_show;
    page->hide = hml_hide;
}


int hml_run(evm_t * e, hml_page_t * page){
    evm_val_t * css_label = evm_prop_get(e, &page->css_object, "label", 0);
    evm_val_t * css_class = evm_prop_get(e, &page->css_object, "class", 0);
    evm_val_t * css_ids = evm_prop_get(e, &page->css_object, "id", 0);
    evm_val_t * css_keyframes = evm_prop_get(e, &page->css_object, "keyframes", 0);

//    native_print(e, NULL, 1, &page->hml_object);
//    native_print(e, NULL, 1, css_label);
//    native_print(e, NULL, 1, css_class);
//    native_print(e, NULL, 1, css_ids);
//    native_print(e, NULL, 1, css_keyframes);

    hml_set_display_style(e, NULL, 1, css_label);
    __hml_object_create(e, page, NULL, "root", &page->hml_object, page->index_object, css_label, css_class, css_ids, css_keyframes);

    return ec_ok;
}

void show(evm_t * e, evm_val_t * hml_object, int vc, evm_val_t * v)
{
    EVM_UNUSED(e);
    hml_show(e, hml_object, 0, NULL);
}


void hide(evm_t * e, evm_val_t * hml_object)
{
    hml_hide(e, hml_object, 0, NULL);
}

void merge_obj(evm_t * e, evm_val_t * target, evm_val_t * src){
    if (src){
        for(int i=0; i< evm_prop_len(src); i++){
            int keyIndex = evm_prop_get_key_by_index(e, src, i);
            if (keyIndex == 0){
                continue;
            }
            char * key = evm_string_get(e, keyIndex);
            if (evm_prop_get(e, target, key, 0)){
                evm_prop_set_value(e, target, key, *evm_prop_get(e, src, key, 0));
            }else{
                evm_prop_append(e, target, key, evm_prop_get(e, src, key, 0));
            }
        }
    }
}

int css_run(evm_t * e, evm_val_t * hml, evm_val_t * css_label, evm_val_t * css_class, evm_val_t * css_id, evm_val_t * keyframes){
    if( !evm_is_object(hml) ) return ec_type;

    if (!css_label && !css_class && !css_id){
        return ec_type;
    }
    evm_val_t * attributes = evm_prop_get(e, hml, "attributes", 0);

    //native_print(e, NULL, 1, hml);

    evm_val_t * label = evm_prop_get(e, hml, "name", 0);
    evm_val_t * class = evm_prop_get(e, attributes, "class", 0);
    evm_val_t * id = evm_prop_get(e, attributes, "id", 0);
    evm_val_t * style = evm_prop_get(e, attributes, "style", 0);

    evm_val_t * css_obj = evm_object_create(e, GC_DICT, 0, 0);
    if (label){
        evm_val_t * css_label_properties = evm_prop_get(e, css_label, evm_2_string(label), 0);
        merge_obj(e, css_obj, css_label_properties);
    }
    if (class){
        evm_val_t * css_class_properties = evm_prop_get(e, css_class, evm_2_string(class), 0);
        merge_obj(e, css_obj, css_class_properties);
    }
    if (id){
        evm_val_t * css_id_properties = evm_prop_get(e, css_id, evm_2_string(id), 0);
        if (css_id_properties){
//            native_print(e, NULL, 1, css_id_properties);
            merge_obj(e, css_obj, css_id_properties);
        }
    }
    if (style){
        merge_obj(e, css_obj, style);
    }

    evm_val_t * animation_name = evm_prop_get(e, css_obj, "animation-name", 0);
    if (animation_name && evm_is_string(animation_name)){
        const char * _name = evm_2_string(animation_name);
        evm_val_t* animation_keyframes =  evm_prop_get(e, keyframes, _name, 0);
        if (animation_keyframes && evm_is_object(animation_keyframes)){
            evm_val_t * from = evm_prop_get(e, animation_keyframes, "from", 0);
            evm_val_t * to = evm_prop_get(e, animation_keyframes, "to", 0);

            int from_key = evm_prop_get_key_by_index(e, from, 0);
            int to_key = evm_prop_get_key_by_index(e, from, 0);
            if (from_key == to_key){
                if (evm_prop_get(e, css_obj,  "animation-property", 0)){
                    evm_prop_set_value(e, css_obj, "animation-property", *(evm_string_pool_get(e, from_key)));
                }else{
                    evm_prop_append(e, css_obj, "animation-property", evm_string_pool_get(e, from_key));
                }
                if (evm_prop_get(e, css_obj,  "animation-property-from", 0)){
                    evm_prop_set_value(e, css_obj, "animation-property-from", *(evm_prop_get_by_key(e, from, from_key, 0)));
                }else{
                    evm_prop_append(e, css_obj, "animation-property-from", evm_prop_get_by_key(e, from, from_key, 0));
                }
                if (evm_prop_get(e, css_obj,  "animation-property-to", 0)){
                    evm_prop_set_value(e, css_obj, "animation-property-to", *(evm_prop_get_by_key(e, to, from_key, 0)));
                }else{
                    evm_prop_append(e, css_obj, "animation-property-to", evm_prop_get_by_key(e, to, from_key, 0));
                }
//                 native_print(e, NULL, 1, css_obj);
            }
        }
    }
    hml_set_style(e, hml, 0, css_obj);
//    evm_val_t * elements = evm_prop_get(e, hml, "elements", 0);
//    for(int i=0; i< evm_list_len(elements); i++){
//        evm_val_t * element = evm_list_get(e, elements, i);
//        css_run(e, element, css_label, css_class, css_id);
//    }
    return ec_ok;
}

evm_val_t hml_eval(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( argc > 0 ){
        const char * string = evm_2_string(v);
        evm_val_t * data_value = evm_prop_get(e, &current_page->index_object, "data", 0);
        return evm_run_eval(e, data_value, string, EVM_LANG_JS);
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t hml_css_url(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    evm_print("hml_css_url");
    return EVM_VAL_UNDEFINED;
}

evm_err_t hml_module(evm_t * e,
               evm_native_fn init_api,
               evm_native_fn set_display_style,
               evm_native_fn set_style,
               evm_native_fn show,
               evm_native_fn hide){
    evm_builtin_t natives[] = {
        {"propertyChanged", evm_mk_native( (intptr_t)propertyChanged )},
        {"hml_eval", evm_mk_native( (intptr_t)hml_eval )},

        {".css_url", evm_mk_native( (intptr_t)hml_css_url )},
        {NULL, EVM_VAL_UNDEFINED}
    };
    hml_init_api = init_api;
    hml_set_display_style = set_display_style;
    hml_set_style = set_style;
    hml_show = show;
    hml_hide = hide;
    return evm_native_add(e, natives);
}

