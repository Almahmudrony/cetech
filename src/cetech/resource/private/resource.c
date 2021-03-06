//==============================================================================
// Includes
//==============================================================================

#include <corelib/array.inl>
#include <corelib/hash.inl>

#include <corelib/ebus.h>
#include <corelib/api_system.h>
#include <corelib/memory.h>
#include <corelib/fs.h>
#include <corelib/config.h>
#include <corelib/os.h>
#include <corelib/log.h>
#include <cetech/resource/package.h>
#include <corelib/module.h>
#include <corelib/cdb.h>
#include <cetech/kernel/kernel.h>
#include <corelib/buffer.inl>

#include "include/SDL2/SDL.h"

#include "resource.h"

//==============================================================================
// Gloals
//==============================================================================

#define _G ResourceManagerGlobals
#define LOG_WHERE "resource_manager"

//==============================================================================
// Gloals
//==============================================================================

struct _G {
    struct ct_hash_t type_map;

    bool autoload_enabled;

    struct ct_cdb_t db;
    uint64_t resource_db;

    uint64_t config;
    struct ct_alloc *allocator;
} _G = {};

//==============================================================================
// Private
//==============================================================================


//==============================================================================
// Public interface
//==============================================================================

static int type_name_string(char *str,
                            size_t max_len,
                            struct ct_resource_id resourceid) {
    return snprintf(str, max_len, "%"
            PRIx64
            "%"
            PRIx64, resourceid.type, resourceid.name);
}


static void set_autoload(bool enable) {
    _G.autoload_enabled = enable;
}


static void _resource_api_add(uint64_t name,
                              void *api) {
    struct ct_resource_i0 *ct_resource_i = api;

    uint64_t type_obj = ct_cdb_a0->create_object(ct_cdb_a0->db(), 0);

    ct_cdb_obj_o *w = ct_cdb_a0->write_begin(_G.resource_db);
    ct_cdb_a0->set_subobject(w, ct_resource_i->cdb_type(), type_obj);
    ct_cdb_a0->write_commit(w);

    ct_hash_add(&_G.type_map, ct_resource_i->cdb_type(), (uint64_t) api,
                _G.allocator);
}

static void load(uint64_t type,
                 uint64_t *names,
                 size_t count,
                 int force);

static void load_now(uint64_t type,
                     uint64_t *names,
                     size_t count) {
    load(type, names, count, 0);
}

static int can_get(uint64_t type,
                   uint64_t name) {

    uint64_t type_obj = ct_cdb_a0->read_subobject(_G.resource_db, type, 0);

    return ct_cdb_a0->prop_exist(type_obj, name);
}

static int can_get_all(uint64_t type,
                       uint64_t *names,
                       size_t count) {
    uint64_t type_obj = ct_cdb_a0->read_subobject(_G.resource_db, type, 0);

    for (size_t i = 0; i < count; ++i) {
        if (!ct_cdb_a0->prop_exist(type_obj, names[i])) {
            return 0;
        }
    }

    return 1;
}

static struct ct_resource_i0 *get_resource_interface(uint64_t type) {
    return (struct ct_resource_i0 *) ct_hash_lookup(&_G.type_map, type, 0);
}

static void load(uint64_t type,
                 uint64_t *names,
                 size_t count,
                 int force) {
    struct ct_resource_i0 *resource_i = get_resource_interface(type);

    if (!resource_i) {
        return;
    }

    const uint64_t root_name = BUILD_ROOT;

    uint64_t resource_objects[count];

    for (uint32_t i = 0; i < count; ++i) {
        resource_objects[i] = 0;

        const uint64_t asset_name = names[i];

        if (!force && can_get(type, asset_name)) {
            continue;
        };

        uint64_t object = ct_cdb_a0->create_object(_G.db,
                                                   resource_i->cdb_type());
        resource_objects[i] = object;

        char build_name[128] = {};

        struct ct_resource_id rid = (struct ct_resource_id) {
                .name = asset_name,
                .type = type,
        };

        type_name_string(build_name, CT_ARRAY_LEN(build_name), rid);

        char filename[1024] = {};
        resource_compiler_get_filename(filename, CT_ARRAY_LEN(filename), rid);

        ct_log_a0->debug("resource", "Loading resource %s from %s",
                         filename, build_name);


        char *build_full = NULL;
        ct_os_a0->path->join(&build_full,
                             _G.allocator, 2,
                             ct_cdb_a0->read_str(_G.config,
                                                 CONFIG_PLATFORM, ""),
                             build_name);

        struct ct_vio *resource_file = ct_fs_a0->open(root_name,
                                                      build_full,
                                                      FS_OPEN_READ);

        ct_buffer_free(build_full, _G.allocator);

        if (!resource_file) {
            continue;
        }

        resource_i->online(names[i], resource_file, object);
        ct_fs_a0->close(resource_file);

    }

    uint64_t type_obj = ct_cdb_a0->read_subobject(_G.resource_db, type, 0);
    ct_cdb_obj_o *w;
    do {
        w = ct_cdb_a0->write_begin(type_obj);
        for (uint32_t i = 0; i < count; ++i) {
            if(!resource_objects[i]) continue;

            const uint64_t asset_name = names[i];

            ct_cdb_a0->set_subobject(w, asset_name, resource_objects[i]);
        }
    } while (!ct_cdb_a0->write_try_commit(w));
}

static void unload(uint64_t type,
                   uint64_t *names,
                   size_t count) {
    uint64_t type_obj = ct_cdb_a0->read_subobject(_G.resource_db, type, 0);

    struct ct_resource_i0 *resource_i = get_resource_interface(type);

    if (!resource_i) {
        return;
    }

    for (uint32_t i = 0; i < count; ++i) {
        if (1) {// TODO: ref counting
            char build_name[128] = {};
            struct ct_resource_id rid = (struct ct_resource_id) {
                    .type = type,
                    .name = names[i],
            };

            type_name_string(build_name, CT_ARRAY_LEN(build_name), rid);

            char filename[1024] = {};
            resource_compiler_get_filename(filename,
                                           CT_ARRAY_LEN(filename),
                                           rid);

            ct_log_a0->debug("resource", "Unload resource %s ", filename);

            uint64_t object;
            object = ct_cdb_a0->read_subobject(type_obj, rid.name, 0);

            if (!object) {
                continue;
            }

            resource_i->offline(names[i], object);
        }
    }
}

static uint64_t get_obj(struct ct_resource_id resource_id) {
    uint64_t type_obj = ct_cdb_a0->read_subobject(_G.resource_db,
                                                  resource_id.type, 0);

    uint64_t object;
    object = ct_cdb_a0->read_subobject(type_obj, resource_id.name, 0);

    if (!object) {
        char build_name[128] = {};
        type_name_string(build_name, CT_ARRAY_LEN(build_name), resource_id);

        if (_G.autoload_enabled) {
            char filename[1024] = {};
            resource_compiler_get_filename(filename,
                                           CT_ARRAY_LEN(filename),
                                           resource_id);

            ct_log_a0->warning(LOG_WHERE, "Autoloading resource %s", filename);
            load_now(resource_id.type, &resource_id.name, 1);
        } else {
            // TODO: fallback resource #205
            CETECH_ASSERT(LOG_WHERE, false);
        }

        object = ct_cdb_a0->read_subobject(type_obj, resource_id.name, 0);
    }

    return object;
}

static void reload(uint64_t type,
                   uint64_t *names,
                   size_t count) {

//        const uint32_t idx = map::get<uint32_t>(_G.type_map, ptype, 0);
//        void* data = NULL;
//        ct_resource_type_t type_clb = _G.resource_callbacks[idx];
//
//        const uint32_t type_item_idx = _find_type(ptype);
//        type_item_t* type_item = &_G.type_items[type_item_idx];
//
//        load(ptype, names, count, 1);
//        for (uint32_t i = 0; i < count; ++i) {
//
//            char filename[1024] = {};
//            resource_compiler_get_filename(filename, CETECH_ARRAY_LEN(filename),
//                                           ptype,
//                                           names[i]);
//#else
//            char build_name[128] = {};
//            type_name_string(build_name, CETECH_ARRAY_LEN(build_name),
//                                       ptype, names[i]);
//
//            char *filename = build_name;
//#endif
//            ct_log_a0->debug("resource", "Reload resource %s ", filename);

//            void *old_data = get(ptype, names[i]);
//
//            void *new_data = type_clb.reloader(names[i], old_data,
//                                               loaded_data[i],
//                                               _G.allocator);
//
//            uint64_t id = hash_combine(ptype, names[i]);
//            uint32_t item_idx = map::get(_G.resource_map, id, UINT32_MAX);
//            if (item_idx == UINT32_MAX) {
//                continue;
//            }
//
//            resource_item_t item = _G.resource_data[item_idx];
//            item.data = new_data;
//            //--item.ref_count; // Load call increase item.ref_count, because is loaded
//            _G.resource_data[item_idx] = item;
//        }
}

static void reload_all() {
}

void resource_memory_reload(struct ct_resource_id resource_id,
                            char **blob) {
}

static struct ct_resource_a0 resource_api = {
        .get_interface = get_resource_interface,
        .set_autoload = set_autoload,
        .load = load,
        .load_now = load_now,
        .unload = unload,
        .reload = reload,
        .reload_all = reload_all,
        .can_get = can_get,
        .can_get_all = can_get_all,
        .get = get_obj,
        .type_name_string = type_name_string,

        .compile_and_reload = compile_and_reload,
        .compiler_compile_all = resource_compiler_compile_all,
        .compiler_get_filename = resource_compiler_get_filename,
        .compiler_get_tmp_dir = resource_compiler_get_tmp_dir,
        .compiler_external_join = resource_compiler_external_join,
        .type_name_from_filename = type_name_from_filename,

};

static struct ct_package_a0 package_api = {
        .load = package_load,
        .unload = package_unload,
        .is_loaded = package_is_loaded,
        .flush = package_flush,
};

struct ct_resource_a0 *ct_resource_a0 = &resource_api;
struct ct_package_a0 *ct_package_a0 = &package_api;

static void _init_api(struct ct_api_a0 *api) {
    api->register_api("ct_resource_a0", &resource_api);
    api->register_api("ct_package_a0", &package_api);
}


static void _init_cvar(struct ct_config_a0 *config) {
    _G = (struct _G) {};

    ct_config_a0 = config;
    _G.config = ct_config_a0->obj();

    ct_cdb_obj_o *writer = ct_cdb_a0->write_begin(_G.config);
    if (!ct_cdb_a0->prop_exist(_G.config, CONFIG_BUILD)) {
        ct_cdb_a0->set_str(writer, CONFIG_BUILD, "build");
    }
    ct_cdb_a0->write_commit(writer);

}


static void _init(struct ct_api_a0 *api) {
    _init_api(api);
    _init_cvar(ct_config_a0);

    _G = (struct _G) {
            .allocator = ct_memory_a0->system,
            .config = ct_config_a0->obj(),
            .db = ct_cdb_a0->db()
    };

    _G.resource_db = ct_cdb_a0->create_object(ct_cdb_a0->db(), 0);

    ct_fs_a0->map_root_dir(BUILD_ROOT,
                           ct_cdb_a0->read_str(_G.config, CONFIG_BUILD, ""),
                           false);

    ct_api_a0->register_on_add(RESOURCE_I, _resource_api_add);

}

static void _shutdown() {
    package_shutdown();

    ct_cdb_a0->destroy_db(_G.db);

    ct_hash_free(&_G.type_map, _G.allocator);
}


CETECH_MODULE_DEF(
        resourcesystem,
        {
            CT_INIT_API(api, ct_memory_a0);
            CT_INIT_API(api, ct_fs_a0);
            CT_INIT_API(api, ct_config_a0);
            CT_INIT_API(api, ct_os_a0);
            CT_INIT_API(api, ct_log_a0);
            CT_INIT_API(api, ct_hashlib_a0);
            CT_INIT_API(api, ct_cdb_a0);
        },
        {
            CT_UNUSED(reload);
            _init(api);
        },
        {
            CT_UNUSED(reload);
            CT_UNUSED(api);

            _shutdown();

        }
)
