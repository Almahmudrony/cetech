#include <corelib/hashlib.h>
#include <corelib/ebus.h>
#include <corelib/os.h>
#include <corelib/macros.h>
#include "corelib/log.h"
#include "corelib/allocator.h"
#include "corelib/api_system.h"

#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_syswm.h"


//==============================================================================
// Private
//==============================================================================

static uint32_t _sdl_pos(const uint32_t pos) {
    switch (pos) {
        case WINDOWPOS_CENTERED:
            return SDL_WINDOWPOS_CENTERED;

        case WINDOWPOS_UNDEFINED:
            return SDL_WINDOWPOS_UNDEFINED;

        default:
            return pos;
    }
}


static struct {
    enum ct_window_flags from;
    SDL_WindowFlags to;
} _flag_to_sdl[] = {
        {.from = WINDOW_NOFLAG, .to =  0},
        {.from = WINDOW_FULLSCREEN, .to = SDL_WINDOW_FULLSCREEN},
        {.from = WINDOW_SHOWN, .to = SDL_WINDOW_SHOWN},
        {.from = WINDOW_HIDDEN, .to = SDL_WINDOW_HIDDEN},
        {.from = WINDOW_BORDERLESS, .to = SDL_WINDOW_BORDERLESS},
        {.from = WINDOW_RESIZABLE, .to = SDL_WINDOW_RESIZABLE},
        {.from = WINDOW_MINIMIZED, .to = SDL_WINDOW_MINIMIZED},
        {.from = WINDOW_MAXIMIZED, .to = SDL_WINDOW_MAXIMIZED},
        {.from = WINDOW_INPUT_GRABBED, .to = SDL_WINDOW_INPUT_GRABBED},
        {.from = WINDOW_INPUT_FOCUS, .to = SDL_WINDOW_INPUT_FOCUS},
        {.from = WINDOW_MOUSE_FOCUS, .to = SDL_WINDOW_MOUSE_FOCUS},
        {.from = WINDOW_FULLSCREEN_DESKTOP, .to =  SDL_WINDOW_FULLSCREEN_DESKTOP},
        {.from = WINDOW_ALLOW_HIGHDPI, .to = SDL_WINDOW_ALLOW_HIGHDPI},
        {.from = WINDOW_MOUSE_CAPTURE, .to = SDL_WINDOW_MOUSE_CAPTURE},
        {.from = WINDOW_ALWAYS_ON_TOP, .to = SDL_WINDOW_ALWAYS_ON_TOP},
        {.from = WINDOW_SKIP_TASKBAR, .to = SDL_WINDOW_SKIP_TASKBAR},
        {.from = WINDOW_UTILITY, .to = SDL_WINDOW_UTILITY},
        {.from = WINDOW_TOOLTIP, .to = SDL_WINDOW_TOOLTIP},
        {.from = WINDOW_POPUP_MENU, .to = SDL_WINDOW_POPUP_MENU},
};

static uint32_t _sdl_flags(uint32_t flags) {
    uint32_t sdl_flags = 0;

    for (uint32_t i = 1; i < CT_ARRAY_LEN(_flag_to_sdl); ++i) {
        if (flags & _flag_to_sdl[i].from) {
            sdl_flags |= _flag_to_sdl[i].to;
        }
    }

    return sdl_flags;
}

//==============================================================================
// Interface
//==============================================================================

void window_set_title(ct_window_ints *w,
                      const char *title) {
    SDL_SetWindowTitle((SDL_Window *) w, title);
}

const char *window_get_title(ct_window_ints *w) {
    return SDL_GetWindowTitle((SDL_Window *) w);
}

void window_resize(ct_window_ints *w,
                   uint32_t width,
                   uint32_t height) {
    SDL_SetWindowSize((SDL_Window *) w, width, height);
}

void window_get_size(ct_window_ints *window,
                     uint32_t *width,
                     uint32_t *height) {
    int w, h;
    w = h = 0;

    SDL_GetWindowSize((SDL_Window *) window, &w, &h);

    *width = (uint32_t) w;
    *height = (uint32_t) h;
}

void *window_native_window_ptr(ct_window_ints *w) {
    SDL_SysWMinfo wmi = {{0}};

    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo((SDL_Window *) w, &wmi)) {
        return 0;
    }

#if defined(CETECH_WINDOWS)
    return (void *) wmi.info.win.window;
#elif CT_PLATFORM_LINUX
    return (void *) wmi.info.x11.window;
#elif CT_PLATFORM_OSX
    return (void *) wmi.info.cocoa.window;
#endif
}

void *window_native_display_ptr(ct_window_ints *w) {
    SDL_SysWMinfo wmi;

    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo((SDL_Window *) w, &wmi)) {
        return 0;
    }

#if defined(CETECH_WINDOWS)
    return (void *) wmi.info.win.hdc;
#elif CT_PLATFORM_LINUX
    return (void *) wmi.info.x11.display;
#elif CT_PLATFORM_OSX
    return (0);
#endif
}

struct ct_window *window_new(struct ct_alloc *alloc,
                             const char *title,
                             enum ct_window_pos x,
                             enum ct_window_pos y,
                             const int32_t width,
                             const int32_t height,
                             uint32_t flags) {

    struct ct_window *window = CT_ALLOC(alloc, struct ct_window,
                                        sizeof(struct ct_window));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *w = SDL_CreateWindow(
            title,
            _sdl_pos(x), _sdl_pos(y),
            width, height,
            _sdl_flags(flags)
    );

    if (w == NULL) {
        ct_log_a0->error("sys", "Could not create window: %s",
                         SDL_GetError());
    }

    *window = (struct ct_window) {
            .inst  = w,
            .set_title = window_set_title,
            .get_title = window_get_title,
            .resize = window_resize,
            .size = window_get_size,
            .native_window_ptr = window_native_window_ptr,
            .native_display_ptr = window_native_display_ptr
    };

    return window;
}

struct ct_window *window_new_from(struct ct_alloc *alloc,
                                  void *hndl) {

    struct ct_window *window = CT_ALLOC(alloc,
                                        struct ct_window,
                                        sizeof(struct ct_window));

    SDL_Window *w = SDL_CreateWindowFrom(hndl);

    if (w == NULL) {
        ct_log_a0->error("sys", "Could not create window: %s",
                         SDL_GetError());
    }

    *window = (struct ct_window) {
            .inst  = w,
            .set_title = window_set_title,
            .get_title = window_get_title,
            .resize = window_resize,
            .size = window_get_size,
            .native_window_ptr = window_native_window_ptr,
            .native_display_ptr = window_native_display_ptr
    };

    return window;
}

void window_destroy(struct ct_alloc *alloc,
                    struct ct_window *w) {
    SDL_DestroyWindow((SDL_Window *) w->inst);
    CT_FREE(alloc, w);
}

struct ct_os_window_a0 window_api = {
        .create = window_new,
        .create_from = window_new_from,
        .destroy = window_destroy,
};


struct ct_os_window_a0 *ct_window_a0 = &window_api;


int sdl_window_init(struct ct_api_a0 *api) {

    api->register_api("ct_os_window_a0", &window_api);
    ct_ebus_a0->create_ebus(WINDOW_EBUS_NAME, WINDOW_EBUS);
    return 1;
}

void sdl_window_shutdown() {

}
