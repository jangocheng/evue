#include "js_app.h"

/******应用上下文 *****/
static evm_val_t js_fwk_system_app_getInfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_app_terminate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_app_module(evm_t * e){
    evm_builtin_t module[] = {
        {"getInfo", evm_mk_native((intptr_t)js_fwk_system_app_getInfo)},
        {"terminate", evm_mk_native((intptr_t)js_fwk_system_app_terminate)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.app", module);
    return e->err;
}

/******页面路由******/
/**
 * @brief router.push(OBJECT)
 * @param OBJECT {uri, params}
 *
 router.push({
  uri: 'pages/routerpage2/routerpage2',
  params: {
      data1: 'message',
      data2: {
          data3: [123,456,789],
          data4: {
              data5: 'message'
          },
      },
  },
});
 */
static evm_val_t js_fwk_system_router_push(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    if( argc > 0 && evm_is_object(v) ){
        js_router_push(app->router, v);
    }
    return EVM_VAL_UNDEFINED;
}
/**
 * @brief router.replace(OBJECT)
 * @param OBJECT {uri, params}
 *
router.replace({
  uri: 'pages/detail/detail',
  params: {
      data1: 'message',
  },
});
 */
static evm_val_t js_fwk_system_router_repalce(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    if( argc > 0 && evm_is_object(v) ){
        js_router_replace(app->router, v);
    }
    return EVM_VAL_UNDEFINED;
}
/**
 * @brief router.back(OBJECT)
 * @param OBJECT {uri}
 *
// detail页面通过back，将返回index页面
router.back();
// 通过back，返回到detail页面
router.back({path:'pages/detail/detail'});
 */
static evm_val_t js_fwk_system_router_back(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}
/**
 * @brief router.clear()
 *
 router.clear();
 */
static evm_val_t js_fwk_system_router_clear(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}
/**
 * @brief //router.getLength()
 * @return {size: number}
 *
var size = router.getLength();
console.log('pages stack size = ' + size);
 */
static evm_val_t js_fwk_system_router_getLength(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}
/**
 * @brief router.getState()
 * @return {index: number | name: string | path: string}
var page = router.getState();
console.log('current index = ' + page.index);
 */
static evm_val_t js_fwk_system_router_getState(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_router_module(evm_t * e){
    evm_builtin_t module[] = {
        {"push", evm_mk_native((intptr_t)js_fwk_system_router_push)},
        {"replace", evm_mk_native((intptr_t)js_fwk_system_router_repalce)},
        {"back", evm_mk_native((intptr_t)js_fwk_system_router_back)},
        {"clear", evm_mk_native((intptr_t)js_fwk_system_router_clear)},
        {"getLength", evm_mk_native((intptr_t)js_fwk_system_router_getLength)},
        {"getState", evm_mk_native((intptr_t)js_fwk_system_router_getState)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.router", module);
    return e->err;
}

/******弹窗******/
static evm_val_t js_fwk_system_prompt_showToast(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_prompt_showDialog(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_prompt_module(evm_t * e){
    evm_builtin_t module[] = {
        {"showToast", evm_mk_native((intptr_t)js_fwk_system_prompt_showToast)},
        {"showDialog", evm_mk_native((intptr_t)js_fwk_system_prompt_showDialog)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.prompt", module);
    return e->err;
}

/******上传下载******/
static evm_val_t js_fwk_system_request_upload(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_request_download(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_request_onDownloadComplete(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_request_module(evm_t * e){
    evm_builtin_t module[] = {
        {"upload", evm_mk_native((intptr_t)js_fwk_system_request_upload)},
        {"download", evm_mk_native((intptr_t)js_fwk_system_request_download)},
        {"onDownloadComplete", evm_mk_native((intptr_t)js_fwk_system_request_onDownloadComplete)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.request", module);
    return e->err;
}

/******数据请求******/
static evm_val_t js_fwk_system_fetch_fetch(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_fetch_module(evm_t * e){
    evm_builtin_t module[] = {
        {"fetch", evm_mk_native((intptr_t)js_fwk_system_fetch_fetch)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.fetch", module);
    return e->err;
}


/******数据存储******/
static evm_val_t js_fwk_system_storage_get(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_storage_set(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_storage_clear(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_storage_delete(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_storage_module(evm_t * e){
    evm_builtin_t module[] = {
        {"get", evm_mk_native((intptr_t)js_fwk_system_storage_get)},
        {"set", evm_mk_native((intptr_t)js_fwk_system_storage_set)},
        {"clear", evm_mk_native((intptr_t)js_fwk_system_storage_clear)},
        {"delete", evm_mk_native((intptr_t)js_fwk_system_storage_delete)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.storage", module);
    return e->err;
}

/******通知消息******/
static evm_val_t js_fwk_system_notification_show(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_notification_module(evm_t * e){
    evm_builtin_t module[] = {
        {"show", evm_mk_native((intptr_t)js_fwk_system_notification_show)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.notification", module);
    return e->err;
}

/******振动******/
static evm_val_t js_fwk_system_vibrator_vibrate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_vibrator_module(evm_t * e){
    evm_builtin_t module[] = {
        {"vibrate", evm_mk_native((intptr_t)js_fwk_system_vibrator_vibrate)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.vibrator", module);
    return e->err;
}


/******传感器******/
static evm_val_t js_fwk_system_sensor_subscribeAccelerometer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_unsubscribeAccelerometer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_subscribeCompass(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_unsubscribeCompass(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_subscribeLight(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_unsubscribeLight(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_subscribeBarometer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_unsubscribeBarometer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_subscribeHeartRate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_sensor_unsubscribeHeartRate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}


static int js_fwk_system_sensor_module(evm_t * e){
    evm_builtin_t module[] = {
        {"subscribeAccelerometer", evm_mk_native((intptr_t)js_fwk_system_sensor_subscribeAccelerometer)},
        {"unsubscribeAccelerometer", evm_mk_native((intptr_t)js_fwk_system_sensor_unsubscribeAccelerometer)},
        {"subscribeCompass", evm_mk_native((intptr_t)js_fwk_system_sensor_subscribeCompass)},
        {"unsubscribeCompass", evm_mk_native((intptr_t)js_fwk_system_sensor_unsubscribeCompass)},
        {"subscribeLight", evm_mk_native((intptr_t)js_fwk_system_sensor_subscribeLight)},
        {"unsubscribeLight", evm_mk_native((intptr_t)js_fwk_system_sensor_unsubscribeLight)},
        {"subscribeBarometer", evm_mk_native((intptr_t)js_fwk_system_sensor_subscribeBarometer)},
        {"unsubscribeBarometer", evm_mk_native((intptr_t)js_fwk_system_sensor_unsubscribeBarometer)},
        {"subscribeHeartRate", evm_mk_native((intptr_t)js_fwk_system_sensor_subscribeHeartRate)},
        {"unsubscribeHeartRate", evm_mk_native((intptr_t)js_fwk_system_sensor_unsubscribeHeartRate)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.sensor", module);
    return e->err;
}

/******地理位置******/
static evm_val_t js_fwk_system_geolocation_getLocation(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_geolocation_getLocationType(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_geolocation_subscribe(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_geolocation_unsubscribe(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_geolocation_getSupportedCoordTypes(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_geolocation_module(evm_t * e){
    evm_builtin_t module[] = {
        {"getLocation", evm_mk_native((intptr_t)js_fwk_system_geolocation_getLocation)},
        {"getLocationType", evm_mk_native((intptr_t)js_fwk_system_geolocation_getLocationType)},
        {"subscribe", evm_mk_native((intptr_t)js_fwk_system_geolocation_subscribe)},
        {"unsubscribe", evm_mk_native((intptr_t)js_fwk_system_geolocation_unsubscribe)},
        {"getSupportedCoordTypes", evm_mk_native((intptr_t)js_fwk_system_geolocation_getSupportedCoordTypes)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.geolocation", module);
    return e->err;
}

/******网络状态******/
static evm_val_t js_fwk_system_network_getType(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_network_subscribe(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_network_unsubscribe(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_network_module(evm_t * e){
    evm_builtin_t module[] = {
        {"getType", evm_mk_native((intptr_t)js_fwk_system_network_getType)},
        {"subscribe", evm_mk_native((intptr_t)js_fwk_system_network_subscribe)},
        {"unsubscribe", evm_mk_native((intptr_t)js_fwk_system_network_unsubscribe)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.network", module);
    return e->err;
}

/******设备信息******/
static evm_val_t js_fwk_system_device_getInfo(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    evm_val_t * o = evm_object_create(e, GC_DICT, 10, 0);
    if( o ){
        evm_prop_set(e, o, 0, "brand", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_BRAND));
        evm_prop_set(e, o, 1, "manufacturer", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_MANUFACTURE));
        evm_prop_set(e, o, 2, "model", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_MODEL));
        evm_prop_set(e, o, 3, "product", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_PRODUCT));
        evm_prop_set(e, o, 4, "language", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_LANG));
        evm_prop_set(e, o, 5, "region", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_REGION));
        evm_prop_set(e, o, 6, "windowWidth", evm_mk_number((intptr_t)APP_CONF_DEV_windowWidth));
        evm_prop_set(e, o, 7, "windowHeight", evm_mk_number((intptr_t)APP_CONF_DEV_windowHeight));
        evm_prop_set(e, o, 8, "screenDensity", evm_mk_number((intptr_t)APP_CONF_DEV_screenDensity));
        evm_prop_set(e, o, 9, "screenShape", evm_mk_foreign_string((intptr_t)APP_CONF_DEV_screenShape));
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_device_module(evm_t * e){
    evm_builtin_t module[] = {
        {"getInfo", evm_mk_native((intptr_t)js_fwk_system_device_getInfo)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.device", module);
    return e->err;
}

/******屏幕亮度******/
static evm_val_t js_fwk_system_brightness_getValue(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_brightness_setValue(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_brightness_getMode(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_brightness_setMode(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t js_fwk_system_brightness_setKeepScreenOn(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}


static int js_fwk_system_brightness_module(evm_t * e){
    evm_builtin_t module[] = {
        {"getValue", evm_mk_native((intptr_t)js_fwk_system_brightness_getValue)},
        {"setValue", evm_mk_native((intptr_t)js_fwk_system_brightness_setValue)},
        {"getMode", evm_mk_native((intptr_t)js_fwk_system_brightness_getMode)},
        {"setMode", evm_mk_native((intptr_t)js_fwk_system_brightness_setMode)},
        {"setKeepScreenOn", evm_mk_native((intptr_t)js_fwk_system_brightness_setKeepScreenOn)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.brightness", module);
    return e->err;
}

/******电量信息******/
static evm_val_t js_fwk_system_battery_getStatus(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    return EVM_VAL_UNDEFINED;
}

static int js_fwk_system_battery_module(evm_t * e){
    evm_builtin_t module[] = {
        {"getStatus", evm_mk_native((intptr_t)js_fwk_system_battery_getStatus)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "@system.battery", module);
    return e->err;
}

int js_fwk_module(evm_t * e){
    int err = js_fwk_system_app_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_router_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_prompt_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_request_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_fetch_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_storage_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_notification_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_vibrator_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_sensor_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_geolocation_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_network_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_device_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_brightness_module(e);
    if( err != ec_ok ) return err;

    err = js_fwk_system_battery_module(e);
    if( err != ec_ok ) return err;

    return err;
}
