#include <celib/allocator.h>

#include <cetech/core/api_system.h>
#include <cetech/core/os/path.h>
#include <cetech/core/memory.h>
#include <cetech/core/config.h>
#include <cetech/core/module.h>

#include "../../core/log/log_system_private.h"
#include "../../core/memory/memory_private.h"
#include "../../core/api/api_private.h"
#include "../../core/memory/allocator_core_private.h"


#include <celib/fpumath.h>

CETECH_DECL_API(ct_log_a0);
CETECH_DECL_API(ct_config_a0);
CETECH_DECL_API(ct_memory_a0);
CETECH_DECL_API(ct_path_a0);
CETECH_DECL_API(ct_module_a0);

#include "../static_module.h"

#define LOG_WHERE "kernel"

namespace os {
    void register_api(ct_api_a0 *api);
}

extern "C" void application_start();

const char *_platform() {
#if defined(CETECH_LINUX)
    return "linux";
#elif defined(CETECH_WINDOWS)
    return "windows";
#elif defined(CETECH_DARWIN)
    return "darwin";
#endif
    return NULL;
}

int load_config(int argc,
                const char **argv) {

    if (!ct_config_a0.parse_args(argc, argv)) {
        return 0;
    }

    cel_alloc *a = ct_memory_a0.main_allocator();

    ct_cvar bd = ct_config_a0.find("build");
    const char *build_dir_str = ct_config_a0.get_string(bd);

    char *build_dir = ct_path_a0.join(a, 2, build_dir_str, _platform());
    char *build_config = ct_path_a0.join(a, 2, build_dir, "global.config");

#ifdef CETECH_CAN_COMPILE
    ct_cvar source_dir = ct_config_a0.find("src");
    const char *source_dir_str = ct_config_a0.get_string(source_dir);
    char *source_config = ct_path_a0.join(a, 2, source_dir_str,
                                        "global.config");

    ct_cvar compile = ct_config_a0.find("compile");
    if (ct_config_a0.get_int(compile)) {
        ct_path_a0.make_path(build_dir);
        ct_path_a0.copy_file(a, source_config, build_config);
    }
    CEL_FREE(a, source_config);
#endif

    ct_config_a0.load_from_yaml_file(build_config, a);

    CEL_FREE(a, build_config);
    CEL_FREE(a, build_dir);

    if (!ct_config_a0.parse_args(argc, argv)) {
        return 0;
    }

    ct_config_a0.log_all();

    return 1;
}

void application_register_api(ct_api_a0 *api);

extern "C" int cetech_kernel_init(int argc,
                       const char **argv) {
    auto *core_alloc = core_allocator::get();

    api::init(core_alloc);
    ct_api_a0 *api = api::v0();

    LOAD_STATIC_MODULE(api, log);

    memory::init(4 * 1024 * 1024);

    core_allocator::register_api(api);

    LOAD_STATIC_MODULE(api, hashlib);

    memory::register_api(api);

    LOAD_STATIC_MODULE(api, error);
    LOAD_STATIC_MODULE(api, vio);
    LOAD_STATIC_MODULE(api, process);
    LOAD_STATIC_MODULE(api, cpu);
    LOAD_STATIC_MODULE(api, time);
    LOAD_STATIC_MODULE(api, thread);
    LOAD_STATIC_MODULE(api, path);
    LOAD_STATIC_MODULE(api, config);
    LOAD_STATIC_MODULE(api, object);
    LOAD_STATIC_MODULE(api, module);

    CETECH_GET_API(api, ct_log_a0);
    CETECH_GET_API(api, ct_memory_a0);
    CETECH_GET_API(api, ct_path_a0);
    CETECH_GET_API(api, ct_config_a0);
    CETECH_GET_API(api, ct_module_a0);

    load_config(argc, argv);

    application_register_api(api);
    init_static_modules();

    ct_module_a0.load_dirs("./bin");

    return 1;
}


extern "C" int cetech_kernel_shutdown() {
    ct_log_a0.debug(LOG_WHERE, "Shutdown");

    ct_module_a0.unload_all();

    auto* api = api::v0();

    UNLOAD_STATIC_MODULE(api, error);
    UNLOAD_STATIC_MODULE(api, vio);
    UNLOAD_STATIC_MODULE(api, process);
    UNLOAD_STATIC_MODULE(api, cpu);
    UNLOAD_STATIC_MODULE(api, time);
    UNLOAD_STATIC_MODULE(api, thread);
    UNLOAD_STATIC_MODULE(api, path);
    UNLOAD_STATIC_MODULE(api, config);
    UNLOAD_STATIC_MODULE(api, object);
    UNLOAD_STATIC_MODULE(api, module);

    api::shutdown();
    memory::memsys_shutdown();
    logsystem::shutdown();

    return 1;
}

int main(int argc,
         const char **argv) {

    if (cetech_kernel_init(argc, argv)) {
        application_start();
    }

    return cetech_kernel_shutdown();
}
