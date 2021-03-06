#include <unistd.h>

#include <corelib/platform.h>

#if CT_PLATFORM_LINUX
#include <sys/inotify.h>
#elif CT_PLATFORM_OSX

#include <CoreServices/CoreServices.h>

#else
#warning Watchdog is unsupported
#endif

#include <corelib/macros.h>


#include <corelib/os.h>
#include <corelib/api_system.h>
#include <corelib/module.h>
#include <corelib/config.h>
#include <corelib/log.h>
#include <corelib/memory.h>
#include <corelib/hashlib.h>
#include <corelib/allocator.h>

#define LOG_WHERE_WATCHDOG "watchdog"
#define _G watchdog_globals

struct _G {

} _G;


struct watchdog_instance {
    struct ct_alloc *alloc;

#if CT_PLATFORM_LINUX
    int inotify;
#endif

#if CT_PLATFORM_OSX

#endif

};

void add_dir(ct_watchdog_instance_t *inst,
             const char *path,
             bool recursive) {
//    watchdog_instance *wi = static_cast<watchdog_instance *>(inst);



#if CT_PLATFORM_LINUX
    //    int wd = 0;
    //    wd = inotify_add_watch(wi->inotify, path, IN_ALL_EVENTS);
    //    ct_log_a0->debug(LOG_WHERE_WATCHDOG, "New watch -> %s", path);
    //
    //    if (-1 == wd) {
    //        ct_log_a0->error(LOG_WHERE_WATCHDOG, "Could not add watch -> %s",
    //                        strerror(errno));
    //        return;
    //    }
#endif

//    char *path_dup = ct_memory_a0->str_dup(path, wi->alloc);
//    uint64_t path_hash = CT_ID64_0(path_dup);

//    celib::map::set(wi->dir2wd, path_hash, wd);
//    celib::map::set(wi->wd2dir, wd, path_dup);

    struct ct_alloc *allocator = ct_memory_a0->system;

    if (recursive) {
        char **files;
        uint32_t files_count;

        ct_os_a0->path->list(path, (const char*[]){"*"}, 1, 1, 1,
                             &files, &files_count, allocator);

        for (uint32_t i = 0; i < files_count; ++i) {
            add_dir(inst, files[i], false);
        }

        ct_os_a0->path->list_free(files, files_count, allocator);
    }
}

struct ct_watchdog_ev_header *event_begin(ct_watchdog_instance_t *inst) {
//    watchdog_instance *wi = static_cast<watchdog_instance *>(inst);

//    return celib::eventstream::begin<ct_watchdog_ev_header>(
//            wi->event_stream);
    return NULL;
}

struct ct_watchdog_ev_header *event_end(ct_watchdog_instance_t *inst) {
//    watchdog_instance *wi = static_cast<watchdog_instance *>(inst);

//    return celib::eventstream::end<ct_watchdog_ev_header>(wi->event_stream);
    return NULL;
}

struct ct_watchdog_ev_header *event_next(ct_watchdog_instance_t *inst,
                                         struct ct_watchdog_ev_header *header) {

    CT_UNUSED(inst);

//    return celib::eventstream::next<ct_watchdog_ev_header>(header);
    return NULL;
}


void clean_events(struct watchdog_instance *wi) {
//    auto *wd_it = celib::eventstream::begin<ct_watchdog_ev_header>(
//            wi->event_stream);
//    const auto *wd_end = celib::eventstream::end<ct_watchdog_ev_header>(
//            wi->event_stream);
//
//    while (wd_it != wd_end) {
//        if (wd_it->type == CT_WATCHDOG_EVENT_FILE_MODIFIED) {
//            ct_wd_ev_file_write_end *ev = reinterpret_cast<ct_wd_ev_file_write_end *>(wd_it);
//            CT_FREE(wi->alloc, ev->filename);
//        }
//
//        wd_it = celib::eventstream::next<ct_watchdog_ev_header>(wd_it);
//    }
//
//
//    celib::eventstream::clear(wi->event_stream);
}

void fetch_events(ct_watchdog_instance_t *inst) {
    struct watchdog_instance *wi = inst;

    clean_events(wi);

#if CT_PLATFORM_LINUX
#define BUF_LEN 1024

    char buf[BUF_LEN] __attribute__ ((aligned(8)));

    ssize_t numRead = read(wi->inotify, buf, BUF_LEN);
    if (numRead <= 0) {
        return;
    }

    struct inotify_event *event;

    for (char *p = buf;
         p < buf + numRead; p += sizeof(struct inotify_event) +
                                 event->len) {
        event = (struct inotify_event *) p;

        if (event->mask & IN_ISDIR) {

        } else {
            if (event->mask & (IN_CLOSE_WRITE | IN_MOVE)) {
//                ct_wd_ev_file_write_end ev = {};

//                char *path = celib::map::get<char *>(
//                        wi->wd2dir,
//                        static_cast<uint64_t>(event->wd),
//                        NULL);

//                ev.dir = path;
//                ev.filename = ct_memory_a0->str_dup(event->name, wi->alloc);

//                celib::eventstream::push<ct_watchdog_ev_header>(
//                        wi->event_stream,
//                        CT_WATCHDOG_EVENT_FILE_MODIFIED,
//                        ev);

                continue;
            }
        }
    }
#endif

}

struct ct_watchdog *create(struct ct_alloc *alloc) {
    struct ct_watchdog *watchdog = CT_ALLOC(alloc,
                                            struct ct_watchdog,
                                            sizeof(struct ct_watchdog));

    struct watchdog_instance *watchdog_inst = CT_ALLOC(alloc,
                                                       struct watchdog_instance,
                                                       sizeof(struct watchdog_instance));

//    watchdog_inst->event_stream.init(alloc);
//    watchdog_inst->wd2dir.init(alloc);
//    watchdog_inst->dir2wd.init(alloc);

    if (NULL == watchdog) {
        return NULL;
    }

#if CT_PLATFORM_LINUX
    int inotify = inotify_init1(IN_NONBLOCK);
    if (-1 == inotify) {
        ct_log_a0->error(LOG_WHERE_WATCHDOG, "Could not init inotify");
        return NULL;
    }
    watchdog_inst->inotify = inotify;
#endif

    watchdog_inst->alloc = alloc;

    *watchdog = (struct ct_watchdog) {
            .inst = watchdog_inst,
            .add_dir = add_dir,
            .fetch_events = fetch_events,
            .event_begin = event_begin,
            .event_end = event_end,
            .event_next = event_next
    };

    return watchdog;
}

void destroy(struct ct_watchdog *watchdog) {
    struct watchdog_instance *wi = (watchdog->inst);
    clean_events(wi);
//    wi->event_stream.destroy();

    struct ct_alloc *alloc = wi->alloc;

////    auto ct_it = celib::map::begin(wi->wd2dir);
////    auto ct_end = celib::map::end(wi->wd2dir);
//
//    while (ct_it != ct_end) {
//        CT_FREE(alloc, ct_it->value);
//
//#if CT_PLATFORM_LINUX
//        inotify_rm_watch(wi->inotify, static_cast<int>(ct_it->key));
//#endif
//
//        ++ct_it;
//    }
//
//#if CT_PLATFORM_LINUX
//    close(wi->inotify);
//#endif
//
//    wi->wd2dir.destroy();
//    wi->dir2wd.destroy();


    CT_FREE(alloc, watchdog);
}

struct ct_os_watchdog_a0 wathdog_api = {
        .create = create,
        .destroy = destroy,
};


struct ct_os_watchdog_a0 *ct_watchdog_a0 = &wathdog_api;
