#include <cetech/core/api.h>
#include <cetech/core/module.h>

#include <cetech/core/os/cpu.h>
#include <cetech/core/os/object.h>
#include <cetech/core/os/time.h>
#include <cetech/core/os/process.h>
#include <cetech/core/log.h>


IMPORT_API(log_api_v0);

#include "path.h"
#include "process.h"
#include "hash.inl"
#include "vio_base.h"

#if defined(CETECH_USE_SDL)

#include "cetech/core/os/private/os_sdl2/sdl2_cpu.h"
#include "cetech/core/os/private/os_sdl2/sdl2_window.h"
#include "cetech/core/os/private/os_sdl2/sdl2_thread.h"
#include "cetech/core/os/private/os_sdl2/sdl2_object.h"
#include "cetech/core/os/private/os_sdl2/sdl2_time.h"
#include "cetech/core/os/private/os_sdl2/sdl2_vio.h"

#endif

static struct thread_api_v0 thread_api = {
        .create = thread_create,
        .kill = thread_kill,
        .wait = thread_wait,
        .get_id = thread_get_id,
        .actual_id = thread_actual_id,
        .yield = thread_yield,
        .spin_lock = thread_spin_lock,
        .spin_unlock = thread_spin_unlock
};

static struct window_api_v0 window_api = {
        .create = window_new,
        .create_from = window_new_from,
        .destroy = window_destroy,
        .set_title = window_set_title,
        .get_title = window_get_title,
        .update = window_update,
        .resize = window_resize,
        .size = window_get_size,
        .native_window_ptr = window_native_window_ptr,
        .native_display_ptr = window_native_display_ptr
};

static struct cpu_api_v0 cpu_api = {
        .count = cpu_count
};

static struct object_api_v0 object_api = {
        .load  = load_object,
        .unload  = unload_object,
        .load_function  = load_function
};

static struct time_api_v0 time_api = {
        .ticks =get_ticks,
        .perf_counter =get_perf_counter,
        .perf_freq =get_perf_freq
};

static struct path_v0 path_api = {
        .list = dir_list,
        .list_free = dir_list_free,
        .make_path = dir_make_path,
        .filename = path_filename,
        .basename = path_basename,
        .dir = path_dir,
        .extension = path_extension,
        .join = path_join,
        .file_mtime = file_mtime
};

static struct vio_api_v0 vio_api = {
        .from_file = vio_from_file,
        .close = vio_close,
        .seek = vio_seek,
        .seek_to_end = vio_seek_to_end,
        .skip = vio_skip,
        .position = vio_position,
        .size = vio_size,
        .read = vio_read,
        .write = vio_write
};

struct process_api_v0 process_api = {
        .exec = exec
};

struct hash_api_v0 hash_api = {
        .id64_from_str = stringid64_from_string,
        .hash_murmur2_64 = hash_murmur2_64
};

void os_register_api(struct api_v0 *api) {
    api->register_api("thread_api_v0", &thread_api);
    api->register_api("window_api_v0", &window_api);
    api->register_api("cpu_api_v0", &cpu_api);
    api->register_api("object_api_v0", &object_api);
    api->register_api("time_api_v0", &time_api);
    api->register_api("path_v0", &path_api);
    api->register_api("vio_api_v0", &vio_api);
    api->register_api("process_api_v0", &process_api);
    api->register_api("hash_api_v0", &hash_api);
}

static void _init(struct api_v0 *api) {
    GET_API(api, log_api_v0);
}

void *sdl_get_module_api(int api) {
    switch (api) {
        case PLUGIN_EXPORT_API_ID: {
            static struct module_api_v0 module = {0};
            module.init = _init;
            //module.init_api = _init_api;
            return &module;
        }

        default:
            return NULL;
    }
}