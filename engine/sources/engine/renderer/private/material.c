//==============================================================================
// Include
//==============================================================================

#include <bgfx/c99/bgfx.h>

#include <celib/stringid/stringid.h>
#include <celib/os/path.h>
#include <engine/renderer/material.h>
#include <celib/string/string.h>
#include <celib/math/types.h>
#include "celib/containers/map.h"
#include "celib/os/vio.h"

#include "engine/core/memory_system.h"
#include "engine/core/resource_manager.h"
#include "engine/develop/resource_compiler.h"
#include "bgfx_texture_resource.h"
#include "bgfx_shader_resource.h"


//==============================================================================
// Structs
//==============================================================================

ARRAY_PROTOTYPE(bgfx_program_handle_t)

ARRAY_PROTOTYPE(stringid64_t)

MAP_PROTOTYPE(bgfx_program_handle_t)

typedef struct material_resource {
    stringid64_t shader_name;
    u32 uniforms_count;
    u32 texture_count;
    u32 vec4f_count;
    u32 mat33f_count;
    u32 mat44f_count;
    // material_resource + 1             | char[32]     uniform_names [uniform_count]
    // uniform_names     + uniform_count | stringid64_t texture_names [texture_count]
    // texture_names     + texture_count | vec4f        vec4f_value [vec4f_count]
    // vec4f_value       + vec4f_count   | mat44f       mat33f_value [mat33f_count]
    // mat33f_count      + mat33f_count  | mat33f       mat44f_value [mat44f_count]
} material_resource_t;

#define material_resource_uniform_names(r) ((char*) ((r)+1))
#define material_resource_uniform_texture(r) ((stringid64_t*) ((material_resource_uniform_names(r)+(32*((r)->uniforms_count)))))
#define material_resource_uniform_vec4f(r) ((vec4f_t*) ((material_resource_uniform_texture(r)+((r)->texture_count))))
#define material_resource_uniform_mat33f(r) ((mat33f_t*) ((material_resource_uniform_vec4f(r)+((r)->vec4f_count))))
#define material_resource_uniform_mat44f(r) ((mat44f_t*) ((material_resource_uniform_mat33f(r)+((r)->mat33f_count))))

#define material_resource_uniform_bgfx(r) ((bgfx_uniform_handle_t*) ((material_resource_uniform_vec4f(r)+((r)->vec4f_count))))


#define _get_resorce(idx) (_G.material_instance_data.data[_G.material_instance_offset.data[(idx)]])

#define LOG_WHERE "material"

ARRAY_PROTOTYPE(material_resource_t)


//==============================================================================
// GLobals
//==============================================================================

ARRAY_PROTOTYPE(bgfx_uniform_handle_t)

#define _G MaterialGlobals
struct G {
    MAP_T(u32) material_instace_map;
    ARRAY_T(u32) material_instance_offset;
    ARRAY_T(u8) material_instance_data;
    ARRAY_T(u32) material_instance_uniform_data;

    struct handlerid material_handler;
    stringid64_t type;
} _G = {0};


//==============================================================================
// Compiler private
//==============================================================================

struct material_compile_output {
    ARRAY_T(char) uniform_names;
    ARRAY_T(u8) data;
    u32 texture_count;
    u32 vec4f_count;
    u32 mat33f_count;
    u32 mat44f_count;
};

static void preprocess(const char *filename,
                       yaml_node_t root,
                       struct compilator_api *capi) {
    yaml_node_t parent_node = yaml_get_node(root, "parent");

    if (yaml_is_valid(parent_node)) {
        char prefab_file[256] = {0};
        char prefab_str[256] = {0};
        yaml_as_string(parent_node, prefab_str, CE_ARRAY_LEN(prefab_str));
        snprintf(prefab_file, CE_ARRAY_LEN(prefab_file), "%s.material", prefab_str);

        capi->add_dependency(filename, prefab_file);

        char full_path[256] = {0};
        const char *source_dir = resource_compiler_get_source_dir();
        os_path_join(full_path, CE_ARRAY_LEN(full_path), source_dir, prefab_file);

        log_debug("material", "Loading parent from: %s", full_path);

        struct vio *prefab_vio = vio_from_file(full_path, VIO_OPEN_READ, memsys_main_allocator());

        char prefab_data[vio_size(prefab_vio) + 1];
        memory_set(prefab_data, 0, vio_size(prefab_vio) + 1);
        vio_read(prefab_vio, prefab_data, sizeof(char), vio_size(prefab_vio));

        yaml_document_t h;
        yaml_node_t prefab_root = yaml_load_str(prefab_data, &h);

        preprocess(filename, prefab_root, capi);
        yaml_merge(root, prefab_root);

        vio_close(prefab_vio);
    }
}

void forach_texture_clb(yaml_node_t key,
                        yaml_node_t value,
                        void *_data) {
    struct material_compile_output *output = _data;

    output->texture_count += 1;

    char tmp_buffer[512] = {0};
    char uniform_name[32] = {0};

    yaml_as_string(key, uniform_name, CE_ARRAY_LEN(uniform_name) - 1);

    yaml_as_string(value, tmp_buffer, CE_ARRAY_LEN(tmp_buffer));
    stringid64_t texture_name = stringid64_from_string(tmp_buffer);

    ARRAY_PUSH(char, &output->uniform_names, uniform_name, CE_ARRAY_LEN(uniform_name));
    ARRAY_PUSH(u8, &output->data, (u8 *) &texture_name, sizeof(stringid64_t));
}

void forach_vec4fs_clb(yaml_node_t key,
                       yaml_node_t value,
                       void *_data) {
    struct material_compile_output *output = _data;

    output->vec4f_count += 1;

    char uniform_name[32] = {0};
    yaml_as_string(key, uniform_name, CE_ARRAY_LEN(uniform_name) - 1);

    vec4f_t v = yaml_as_vec4f_t(value);

    ARRAY_PUSH(char, &output->uniform_names, uniform_name, CE_ARRAY_LEN(uniform_name));
    ARRAY_PUSH(u8, &output->data, (u8 *) &v, sizeof(vec4f_t));
}

void forach_mat44f_clb(yaml_node_t key,
                       yaml_node_t value,
                       void *_data) {
    struct material_compile_output *output = _data;

    output->mat44f_count += 1;

    char uniform_name[32] = {0};
    yaml_as_string(key, uniform_name, CE_ARRAY_LEN(uniform_name) - 1);

    mat44f_t m = yaml_as_mat44f_t(value);

    ARRAY_PUSH(char, &output->uniform_names, uniform_name, CE_ARRAY_LEN(uniform_name));
    ARRAY_PUSH(u8, &output->data, (u8 *) &m, sizeof(mat44f_t));
}

void forach_mat33f_clb(yaml_node_t key,
                       yaml_node_t value,
                       void *_data) {
    struct material_compile_output *output = _data;

    output->vec4f_count += 1;

    char uniform_name[32] = {0};
    yaml_as_string(key, uniform_name, CE_ARRAY_LEN(uniform_name) - 1);

    mat44f_t m = yaml_as_mat44f_t(value);

    ARRAY_PUSH(char, &output->uniform_names, uniform_name, CE_ARRAY_LEN(uniform_name));
    ARRAY_PUSH(u8, &output->data, (u8 *) &m, sizeof(mat44f_t));
}

int _material_resource_compiler(const char *filename,
                                struct vio *source_vio,
                                struct vio *build_vio,
                                struct compilator_api *compilator_api) {

    char source_data[vio_size(source_vio) + 1];
    memory_set(source_data, 0, vio_size(source_vio) + 1);
    vio_read(source_vio, source_data, sizeof(char), vio_size(source_vio));

    yaml_document_t h;
    yaml_node_t root = yaml_load_str(source_data, &h);

    preprocess(filename, root, compilator_api);

    yaml_node_t shader_node = yaml_get_node(root, "shader");
    CE_ASSERT("material", yaml_is_valid(shader_node));

    char tmp_buffer[256] = {0};
    yaml_as_string(shader_node, tmp_buffer, CE_ARRAY_LEN(tmp_buffer));

    struct material_compile_output output = {0};
    ARRAY_INIT(char, &output.uniform_names, memsys_main_allocator());
    ARRAY_INIT(u8, &output.data, memsys_main_allocator());

    yaml_node_t textures = yaml_get_node(root, "textures");
    if (yaml_is_valid(textures)) {
        yaml_node_foreach_dict(textures, forach_texture_clb, &output);
    }

    yaml_node_t vec4 = yaml_get_node(root, "vec4f");
    if (yaml_is_valid(vec4)) {
        yaml_node_foreach_dict(vec4, forach_vec4fs_clb, &output);
    }

    yaml_node_t mat44 = yaml_get_node(root, "mat44f");
    if (yaml_is_valid(mat44)) {
        yaml_node_foreach_dict(mat44, forach_mat44f_clb, &output);
    }

    struct material_resource resource = {
            .shader_name = stringid64_from_string(tmp_buffer),
            .texture_count =output.texture_count,
            .vec4f_count = output.vec4f_count,
            .uniforms_count = ARRAY_SIZE(&output.uniform_names) / 32,
    };

    vio_write(build_vio, &resource, sizeof(resource), 1);
    vio_write(build_vio, output.uniform_names.data, sizeof(char), ARRAY_SIZE(&output.uniform_names));
    vio_write(build_vio, output.data.data, sizeof(u8), ARRAY_SIZE(&output.data));

    ARRAY_DESTROY(char, &output.uniform_names);
    ARRAY_DESTROY(u8, &output.data);
    return 1;
}

//==============================================================================
// Resource
//==============================================================================

void *material_resource_loader(struct vio *input,
                               struct allocator *allocator) {
    const i64 size = vio_size(input);
    char *data = CE_ALLOCATE(allocator, char, size);
    vio_read(input, data, 1, size);

    return data;
}

void material_resource_unloader(void *new_data,
                                struct allocator *allocator) {
    CE_DEALLOCATE(allocator, new_data);
}

void material_resource_online(stringid64_t name,
                              void *data) {
}

static const bgfx_program_handle_t null_program = {0};

void material_resource_offline(stringid64_t name,
                               void *data) {
}

void *material_resource_reloader(stringid64_t name,
                                 void *old_data,
                                 void *new_data,
                                 struct allocator *allocator) {

    material_resource_offline(name, old_data);
    material_resource_online(name, new_data);

    CE_DEALLOCATE(allocator, old_data);

    return new_data;
}

static const resource_callbacks_t material_resource_callback = {
        .loader = material_resource_loader,
        .unloader =material_resource_unloader,
        .online =material_resource_online,
        .offline =material_resource_offline,
        .reloader = material_resource_reloader
};

//==============================================================================
// Interface
//==============================================================================

int material_resource_init() {
    _G = (struct G) {0};

    _G.type = stringid64_from_string("material");

    handlerid_init(&_G.material_handler, memsys_main_allocator());

    MAP_INIT(u32, &_G.material_instace_map, memsys_main_allocator());
    ARRAY_INIT(u32, &_G.material_instance_offset, memsys_main_allocator());
    ARRAY_INIT(u8, &_G.material_instance_data, memsys_main_allocator());

    resource_compiler_register(_G.type, _material_resource_compiler);
    resource_register_type(_G.type, material_resource_callback);

    return 1;
}

void material_resource_shutdown() {
    handlerid_destroy(&_G.material_handler);

    MAP_DESTROY(u32, &_G.material_instace_map);
    ARRAY_DESTROY(u32, &_G.material_instance_offset);
    ARRAY_DESTROY(u8, &_G.material_instance_data);

    _G = (struct G) {0};
}

static const material_t null_material = {0};

material_t material_resource_create(stringid64_t name) {
    struct material_resource *resource = resource_get(_G.type, name);

    u32 size = sizeof(struct material_resource) +
               (resource->uniforms_count * sizeof(char) * 32) +
               (resource->texture_count * sizeof(stringid64_t)) +
               (resource->vec4f_count * sizeof(vec4f_t));

    handler_t h = handlerid_handler_create(&_G.material_handler);

    u32 idx = (u32) ARRAY_SIZE(&_G.material_instance_offset);

    MAP_SET(u32, &_G.material_instace_map, h.h, idx);

    u32 offset = ARRAY_SIZE(&_G.material_instance_data);
    ARRAY_PUSH(u8, &_G.material_instance_data, (u8 *) resource, size);
    ARRAY_PUSH_BACK(u32, &_G.material_instance_offset, offset);

    // write bgfx uniform handlers
    bgfx_uniform_handle_t bgfx_uniforms[resource->uniforms_count];
    const char *u_names = (const char *) (resource + 1);

    u32 off = 0;
    u32 tmp_off = 0;
    off += resource->texture_count;
    for (int i = 0; i < resource->texture_count; ++i) {
        bgfx_uniforms[i] = bgfx_create_uniform(&u_names[i * 32], BGFX_UNIFORM_TYPE_INT1, 1);
    }

    tmp_off = off;
    off += resource->vec4f_count;
    for (int i = tmp_off; i < off; ++i) {
        bgfx_uniforms[i] = bgfx_create_uniform(&u_names[i * 32], BGFX_UNIFORM_TYPE_VEC4, 1);
    }

    tmp_off = off;
    off += resource->mat33f_count;
    for (int i = tmp_off; i < off; ++i) {
        bgfx_uniforms[i] = bgfx_create_uniform(&u_names[i * 32], BGFX_UNIFORM_TYPE_MAT3, 1);
    }

    tmp_off = off;
    off += resource->vec4f_count;
    for (int i = tmp_off; i < off; ++i) {
        bgfx_uniforms[i] = bgfx_create_uniform(&u_names[i * 32], BGFX_UNIFORM_TYPE_MAT4, 1);
    }

    ARRAY_PUSH(u8, &_G.material_instance_data, (u8 *) bgfx_uniforms,
               sizeof(bgfx_uniform_handle_t) * resource->uniforms_count);

    return (material_t) {.h=h};
}


u32 material_get_texture_count(material_t material) {
    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return 0;
    }

    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);

    return resource->texture_count;
}

u32 _material_find_slot(struct material_resource *resource,
                        const char *name) {
    const char *u_names = (const char *) (resource + 1);
    for (u32 i = 0; i < resource->uniforms_count; ++i) {
        if (str_compare(&u_names[i * 32], name) != 0) {
            continue;
        }

        return i;
    }

    return UINT32_MAX;
}

void material_set_texture(material_t material,
                          const char *slot,
                          stringid64_t texture) {

    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);


    stringid64_t *u_texture = material_resource_uniform_texture(resource);

    int slot_idx = _material_find_slot(resource, slot);

    u_texture[slot_idx] = texture;
}

void material_set_vec4f(material_t material,
                        const char *slot,
                        vec4f_t v) {

    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);

    vec4f_t *u_vec4f = material_resource_uniform_vec4f(resource);

    int slot_idx = _material_find_slot(resource, slot);

    u_vec4f[slot_idx - (resource->texture_count)] = v;
}

void material_set_mat33f(material_t material,
                         const char *slot,
                         mat33f_t v) {

    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);

    mat33f_t *u_mat33f = material_resource_uniform_mat33f(resource);

    int slot_idx = _material_find_slot(resource, slot);

    u_mat33f[slot_idx - (resource->texture_count + resource->vec4f_count)] = v;
}

void material_set_mat44f(material_t material,
                         const char *slot,
                         mat44f_t v) {

    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);

    mat44f_t *u_mat44f = material_resource_uniform_mat44f(resource);

    int slot_idx = _material_find_slot(resource, slot);

    u_mat44f[slot_idx - (resource->texture_count + resource->vec4f_count + resource->mat33f_count)] = v;
}


void material_use(material_t material) {
    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);

    if (idx == UINT32_MAX) {
        return;
    }

    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);

    stringid64_t *u_texture = material_resource_uniform_texture(resource);
    vec4f_t *u_vec4f = material_resource_uniform_vec4f(resource);
    mat33f_t *u_mat33f = material_resource_uniform_mat33f(resource);
    mat44f_t *u_mat44f = material_resource_uniform_mat44f(resource);

    bgfx_uniform_handle_t *u_handler = material_resource_uniform_bgfx(resource);

    u32 offset = 0;
    for (int i = 0; i < resource->texture_count; ++i) {
        bgfx_set_texture(i, u_handler[offset + i], texture_resource_get(u_texture[i]), 0);
    }
    offset += resource->texture_count;


    for (int i = 0; i < resource->vec4f_count; ++i) {
        bgfx_set_uniform(u_handler[offset + i], &u_vec4f[i], 1);
    }
    offset += resource->vec4f_count;


    for (int i = 0; i < resource->mat33f_count; ++i) {
        bgfx_set_uniform(u_handler[offset + i], &u_mat33f[i], 1);
    }
    offset += resource->mat33f_count;

    for (int i = 0; i < resource->mat33f_count; ++i) {
        bgfx_set_uniform(u_handler[offset + i], &u_mat44f[i], 1);
    }
    offset += resource->mat44f_count;


    u64 state = 0 |
                BGFX_STATE_RGB_WRITE |
                BGFX_STATE_ALPHA_WRITE |
                BGFX_STATE_DEPTH_WRITE |
                BGFX_STATE_DEPTH_TEST_LESS |
                BGFX_STATE_CULL_CW |
                BGFX_STATE_MSAA;

    bgfx_set_state(state, 0);
}

void material_submit(material_t material) {
    u32 idx = MAP_GET(u32, &_G.material_instace_map, material.idx, UINT32_MAX);
    CE_ASSERT(LOG_WHERE, idx != UINT32_MAX);


    struct material_resource *resource = (struct material_resource *) &_get_resorce(idx);
    bgfx_submit(0, shader_resource_get(resource->shader_name), 0, 0);
}