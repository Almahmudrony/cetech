#include <stdbool.h>
#include <corelib/module.h>
#include <corelib/memory.h>
#include <corelib/api_system.h>
#include <corelib/log.h>
#include "corelib/private/api_private.h"

void memory_register_api(struct ct_api_a0 *api);


bool ct_corelib_init() {
    struct ct_alloc *core_alloc =ct_memory_a0->system;

    api_init(core_alloc);

    CETECH_LOAD_STATIC_MODULE(ct_api_a0, log);
    ct_api_a0->register_api("ct_log_a0", ct_log_a0);

    memory_register_api(ct_api_a0);

    CETECH_LOAD_STATIC_MODULE(ct_api_a0, hashlib);

    CETECH_LOAD_STATIC_MODULE(ct_api_a0, os);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, task);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, cdb);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, ebus);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, yamlng);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, config);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, filesystem);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, module);
    CETECH_LOAD_STATIC_MODULE(ct_api_a0, ydb);

    return true;
}

void memsys_shutdown();

bool ct_corelib_shutdown() {

    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, os);
    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, task);
    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, yamlng);
    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, config);

    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, filesystem);
    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, cdb);
    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, module);
    CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, hashlib);
    // CETECH_UNLOAD_STATIC_MODULE(ct_api_a0, ydb);

    memsys_shutdown();
    api_shutdown();

    return true;
}
