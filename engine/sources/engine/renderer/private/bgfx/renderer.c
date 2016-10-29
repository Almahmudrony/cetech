//==============================================================================
// includes
//==============================================================================

#include <celib/math/types.h>
#include <engine/world/camera.h>
#include <celib/string/stringid.h>
#include <engine/renderer/mesh_renderer.h>
#include <engine/develop/console_server.h>
#include <mpack/mpack.h>
#include <engine/application/application.h>
#include "celib/window/window.h"
#include "engine/renderer/renderer.h"

#include "bgfx/c99/bgfxplatform.h"
#include "texture.h"
#include "shader.h"
#include "engine/renderer/material.h"
#include "scene.h"

//==============================================================================
// GLobals
//==============================================================================

#define _G RendererGlobals
static struct G {
    stringid64_t type;
    u32 size_width;
    u32 size_height;
    int capture;
    int vsync;
    int need_reset;
} _G = {0};


//==============================================================================
// Private
//==============================================================================

static u32 _get_reset_flags() {
    return (_G.capture ? BGFX_RESET_CAPTURE : 0) |
           (_G.vsync ? BGFX_RESET_VSYNC : 0);
}

//==============================================================================
// Interface
//==============================================================================

static int _cmd_resize(mpack_node_t args,
                       mpack_writer_t *writer) {
    mpack_node_t width = mpack_node_map_cstr(args, "width");
    mpack_node_t height = mpack_node_map_cstr(args, "height");

    _G.size_width = (u32) mpack_node_int(width);
    _G.size_height = (u32) mpack_node_int(height);
    _G.need_reset = 1;

    return 0;
}

int renderer_init(int stage) {
    if (stage == 0) {
        return 1;
    }

    _G = (struct G) {0};

    texture_resource_init();
    shader_resource_init();
    material_resource_init();
    scene_resource_init();

    consolesrv_register_command("renderer.resize", _cmd_resize);

    return 1;
}

void renderer_shutdown() {
    texture_resource_shutdown();
    shader_resource_shutdown();
    material_resource_shutdown();
    scene_resource_shutdown();

    bgfx_shutdown();

    _G = (struct G) {0};
}

void renderer_create(window_t window) {
    bgfx_platform_data_t pd = {0};
    pd.nwh = window_native_window_ptr(window);
    pd.ndt = window_native_display_ptr(window);
    bgfx_set_platform_data(&pd);

    // TODO: from config
    bgfx_init(BGFX_RENDERER_TYPE_OPENGL, 0, 0, NULL, NULL);

    window_get_size(window, &_G.size_width, &_G.size_height);

    _G.need_reset = 1;
}

void renderer_set_debug(int debug) {
    if (debug) {
        bgfx_set_debug(BGFX_DEBUG_STATS);
    } else {
        bgfx_set_debug(BGFX_DEBUG_NONE);
    }
}

void renderer_render_world(world_t world,
                           camera_t camera,
                           viewport_t viewport) {
    if (CE_UNLIKELY(_G.need_reset)) {
        bgfx_reset(_G.size_width, _G.size_height, _get_reset_flags());
    }

    bgfx_set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x66CCFFff, 1.0f, 0);

    mat44f_t view_matrix;
    mat44f_t proj_matrix;

    camera_get_project_view(world, camera, &proj_matrix, &view_matrix);
    bgfx_set_view_transform(0, view_matrix.f, proj_matrix.f);

    bgfx_set_view_rect(0, 0, 0, (uint16_t) _G.size_width, (uint16_t) _G.size_height);

    bgfx_touch(0);
    bgfx_dbg_text_clear(0, 0);

    mesh_render_all(world);

    bgfx_frame(0);
    window_update(application_get_main_window());
}

vec2f_t renderer_get_size() {
    vec2f_t result;

    result.x = _G.size_width;
    result.y = _G.size_height;

    return result;
}
