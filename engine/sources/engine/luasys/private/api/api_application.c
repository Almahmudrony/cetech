#include <engine/application/application.h>
#include <engine/plugin/plugin_api.h>
#include "engine/luasys/luasys.h"

#define API_NAME "Application"

static int _application_quit(lua_State *l) {
    application_quit();
    return 0;
}

static int _application_native_platform(lua_State *l) {
    const char *platform = application_native_platform();

    luasys_push_string(l, platform);
    return 1;
}

static int _application_platform(lua_State *l) {
    const char *platform = application_platform();

    luasys_push_string(l, platform);
    return 1;
}


void _register_lua_application_api(get_api_fce_t get_engine_api) {
    luasys_add_module_function(API_NAME, "quit", _application_quit);
    luasys_add_module_function(API_NAME, "get_native_platform", _application_native_platform);
    luasys_add_module_function(API_NAME, "get_platform", _application_platform);
}