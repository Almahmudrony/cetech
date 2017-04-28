#include <celib/array.inl>
#include <celib/yaml.h>
#include <celib/map.inl>
#include <celib/stringid.h>

#include <cetech/entity/entity.h>
#include <cetech/component/component.h>
#include <cetech/renderer/renderer.h>
#include <bgfx/c99/bgfx.h>
#include <cetech/transform/transform.h>
#include <cetech/renderer/private/scene/scene.h>
#include <cetech/scenegraph/scenegraph.h>
#include <celib/math_mat44f.inl>

#include <cetech/memory/memory.h>
#include <cetech/module/module.h>


IMPORT_API(MemSysApi, 0);
IMPORT_API(SceneGprahApi, 0);
IMPORT_API(TransformApi, 0);
IMPORT_API(ComponentSystemApi, 0);
IMPORT_API(MaterialApi, 0);
IMPORT_API(MeshRendererApi, 0);
IMPORT_API(ResourceApi, 0);


#define LOG_WHERE "mesh_renderer"

ARRAY_PROTOTYPE(stringid64_t)

ARRAY_PROTOTYPE(material_t)

struct mesh_data {
    stringid64_t scene;
    stringid64_t mesh;
    stringid64_t node;
    stringid64_t material;
};

typedef struct {
    MAP_T(uint32_t) ent_idx_map;

    ARRAY_T(stringid64_t) scene;
    ARRAY_T(stringid64_t) mesh;
    ARRAY_T(stringid64_t) node;
    ARRAY_T(stringid64_t) material;

    ARRAY_T(material_t) material_instance;
} world_data_t;

ARRAY_PROTOTYPE(world_data_t)

MAP_PROTOTYPE(world_data_t)

mesh_renderer_t mesh_get(world_t world,
                         entity_t entity);

#define _G meshGlobal
static struct G {
    stringid64_t type;

    MAP_T(world_data_t) world;
} _G = {0};

static void _new_world(world_t world) {
    world_data_t data = {0};

    MAP_INIT(uint32_t, &data.ent_idx_map, MemSysApiV0.main_allocator());

    ARRAY_INIT(stringid64_t, &data.scene, MemSysApiV0.main_allocator());
    ARRAY_INIT(stringid64_t, &data.mesh, MemSysApiV0.main_allocator());
    ARRAY_INIT(stringid64_t, &data.node, MemSysApiV0.main_allocator());
    ARRAY_INIT(stringid64_t, &data.material, MemSysApiV0.main_allocator());
    ARRAY_INIT(material_t, &data.material_instance,
               MemSysApiV0.main_allocator());

    MAP_SET(world_data_t, &_G.world, world.h.h, data);
}

static world_data_t *_get_world_data(world_t world) {
    return MAP_GET_PTR(world_data_t, &_G.world, world.h.h);
}

static void _destroy_world(world_t world) {
    world_data_t *data = _get_world_data(world);

    MAP_DESTROY(uint32_t, &data->ent_idx_map);

    ARRAY_DESTROY(stringid64_t, &data->scene);
    ARRAY_DESTROY(stringid64_t, &data->mesh);
    ARRAY_DESTROY(stringid64_t, &data->node);
    ARRAY_DESTROY(stringid64_t, &data->material);
    ARRAY_DESTROY(material_t, &data->material_instance);

}

int _mesh_component_compiler(yaml_node_t body,
                             ARRAY_T(uint8_t) *data) {

    struct mesh_data t_data;

    char tmp_buffer[64] = {0};

    YAML_NODE_SCOPE(scene, body, "scene",
                    yaml_as_string(scene, tmp_buffer,
                                   CEL_ARRAY_LEN(tmp_buffer));
                            t_data.scene = stringid64_from_string(tmp_buffer);
    );
    YAML_NODE_SCOPE(mesh, body, "mesh",
                    yaml_as_string(mesh, tmp_buffer, CEL_ARRAY_LEN(tmp_buffer));
                            t_data.mesh = stringid64_from_string(tmp_buffer);
    );

    YAML_NODE_SCOPE(material, body, "material",
                    yaml_as_string(material, tmp_buffer,
                                   CEL_ARRAY_LEN(tmp_buffer));
                            t_data.material = stringid64_from_string(
                                    tmp_buffer);
    );

    YAML_NODE_SCOPE(node, body, "node",
                    if (yaml_is_valid(node)) {
                        yaml_as_string(node, tmp_buffer,
                                       CEL_ARRAY_LEN(tmp_buffer));
                        t_data.node = stringid64_from_string(tmp_buffer);
                    }
    );

    ARRAY_PUSH(uint8_t, data, (uint8_t *) &t_data, sizeof(t_data));

    return 1;
}

static void _on_world_create(world_t world) {
    _new_world(world);
}

static void _on_world_destroy(world_t world) {
    _destroy_world(world);
}

static void _destroyer(world_t world,
                       entity_t *ents,
                       size_t ent_count) {
    world_data_t *world_data = _get_world_data(world);

    // TODO: remove from arrays, swap idx -> last AND change size
    for (int i = 0; i < ent_count; ++i) {
        if (MAP_HAS(uint32_t, &world_data->ent_idx_map, ents[i].idx)) {
            MAP_REMOVE(uint32_t, &world_data->ent_idx_map, ents[i].idx);
        }

        //CEL_ASSERT("mesh_renderer", MAP_HAS(uint32_t, &world_data->ent_idx_map, ents[i].idx));
    }
}


static void _spawner(world_t world,
                     entity_t *ents,
                     uint32_t *cents,
                     uint32_t *ents_parent,
                     size_t ent_count,
                     void *data) {
    struct mesh_data *tdata = data;

    for (int i = 0; i < ent_count; ++i) {
        MeshRendererApiV0.create(world,
                                 ents[cents[i]],
                                 tdata[i].scene,
                                 tdata[i].mesh,
                                 tdata[i].node,
                                 tdata[i].material);
    }
}



static void _shutdown() {
    MAP_DESTROY(world_data_t, &_G.world);

    _G = (struct G) {0};
}


int mesh_is_valid(mesh_renderer_t mesh) {
    return mesh.idx != UINT32_MAX;
}

int mesh_has(world_t world,
             entity_t entity) {
    world_data_t *world_data = _get_world_data(world);
    return MAP_HAS(uint32_t, &world_data->ent_idx_map, entity.h.h);
}

mesh_renderer_t mesh_get(world_t world,
                         entity_t entity) {

    world_data_t *world_data = _get_world_data(world);
    uint32_t idx = MAP_GET(uint32_t, &world_data->ent_idx_map, entity.h.h, UINT32_MAX);
    return (mesh_renderer_t) {.idx = idx};
}

mesh_renderer_t mesh_create(world_t world,
                            entity_t entity,
                            stringid64_t scene,
                            stringid64_t mesh,
                            stringid64_t node,
                            stringid64_t material) {

    world_data_t *data = _get_world_data(world);

    scene_create_graph(world, entity, scene);

    material_t material_instance = MaterialApiV0.resource_create(material);

    uint32_t idx = (uint32_t) ARRAY_SIZE(&data->material);

    MAP_SET(uint32_t, &data->ent_idx_map, entity.h.h, idx);

    if (node.id == 0) {
        node = scene_get_mesh_node(scene, mesh);
    }

    ARRAY_PUSH_BACK(stringid64_t, &data->scene, scene);
    ARRAY_PUSH_BACK(stringid64_t, &data->mesh, mesh);
    ARRAY_PUSH_BACK(stringid64_t, &data->node, node);
    ARRAY_PUSH_BACK(stringid64_t, &data->material, material);
    ARRAY_PUSH_BACK(material_t, &data->material_instance, material_instance);

    return (mesh_renderer_t) {.idx = idx};
}

void mesh_render_all(world_t world) {
    world_data_t *data = _get_world_data(world);

    const MAP_ENTRY_T(uint32_t) *ce_it = MAP_BEGIN(uint32_t, &data->ent_idx_map);
    const MAP_ENTRY_T(uint32_t) *ce_end = MAP_END(uint32_t, &data->ent_idx_map);
    while (ce_it != ce_end) {
        material_t material = ARRAY_AT(&data->material_instance, ce_it->value);
        stringid64_t scene = ARRAY_AT(&data->scene, ce_it->value);
        stringid64_t geom = ARRAY_AT(&data->mesh, ce_it->value);

        MaterialApiV0.use(material);

        entity_t ent = {.idx = ce_it->key};

        transform_t t = TransformApiV0.get(world, ent);
        cel_mat44f_t t_w = *TransformApiV0.get_world_matrix(world, t);
        //cel_mat44f_t t_w = MAT44F_INIT_IDENTITY;//*transform_get_world_matrix(world, t);
        cel_mat44f_t node_w = MAT44F_INIT_IDENTITY;
        cel_mat44f_t final_w = MAT44F_INIT_IDENTITY;


        if (SceneGprahApiV0.has(world, ent)) {
            stringid64_t name = scene_get_mesh_node(scene, geom);
            if (name.id != 0) {
                scene_node_t n = SceneGprahApiV0.node_by_name(world, ent, name);
                node_w = *SceneGprahApiV0.get_world_matrix(world, n);
            }
        }

        cel_mat44f_mul(&final_w, &node_w, &t_w);

        bgfx_set_transform(&final_w, 1);

        scene_submit(scene, geom);

        MaterialApiV0.submit(material);

        ++ce_it;
    }
}

material_t mesh_get_material(world_t world,
                             mesh_renderer_t mesh) {
    CEL_ASSERT(LOG_WHERE, mesh.idx != UINT32_MAX);
    world_data_t *data = _get_world_data(world);

    return ARRAY_AT(&data->material_instance, mesh.idx);
}

void mesh_set_material(world_t world,
                       mesh_renderer_t mesh,
                       stringid64_t material) {
    world_data_t *data = _get_world_data(world);

    material_t material_instance = MaterialApiV0.resource_create(material);
    ARRAY_AT(&data->material_instance, mesh.idx) = material_instance;
    ARRAY_AT(&data->material, mesh.idx) = material;
}


static void _set_property(world_t world,
                          entity_t entity,
                          stringid64_t key,
                          struct property_value value) {

    stringid64_t scene = stringid64_from_string("scene");
    stringid64_t mesh = stringid64_from_string("mesh");
    stringid64_t node = stringid64_from_string("node");
    stringid64_t material = stringid64_from_string("material");

    mesh_renderer_t mesh_renderer = mesh_get(world, entity);


    if (key.id == material.id) {
        mesh_set_material(world, mesh_renderer, stringid64_from_string(value.value.str));
    }
}

static struct property_value _get_property(world_t world,
                                           entity_t entity,
                                           stringid64_t key) {
    stringid64_t scene = stringid64_from_string("scene");
    stringid64_t mesh = stringid64_from_string("mesh");
    stringid64_t node = stringid64_from_string("node");
    stringid64_t material = stringid64_from_string("material");

    mesh_renderer_t mesh_r = mesh_get(world, entity);
    world_data_t *data = _get_world_data(world);

    char name_buff[256] = {0};

//    if (key.id == scene.id) {
//        ResourceApiV0.get_filename(name_buff, CEL_ARRAY_LEN(name_buff), scene, ARRAY_AT(&data->scene, mesh_r.idx));
//        char* name = cel_strdup(name_buff, MemSysApiV0.main_scratch_allocator());
//
//        return (struct property_value) {
//                .type= PROPERTY_STRING,
//                .value.str = name
//        };
//    } else if (key.id == mesh.id) {
//        ResourceApiV0.compiler_get_filename(name_buff, CEL_ARRAY_LEN(name_buff), scene, ARRAY_AT(&data->mesh, mesh_r.idx));
//        char* name = cel_strdup(name_buff, MemSysApiV0.main_scratch_allocator());
//
//        return (struct property_value) {
//                .type= PROPERTY_STRING,
//                .value.str = name
//        };
//    } else if (key.id == node.id) {
//        ResourceApiV0.compiler_get_filename(name_buff, CEL_ARRAY_LEN(name_buff), scene, ARRAY_AT(&data->node, mesh_r.idx));
//        char* name = cel_strdup(name_buff, MemSysApiV0.main_scratch_allocator());
//
//        return (struct property_value) {
//                .type= PROPERTY_STRING,
//                .value.str = name
//        };
//    } else if (key.id == material.id) {
//        ResourceApiV0.compiler_get_filename(name_buff, CEL_ARRAY_LEN(name_buff), scene, ARRAY_AT(&data->scene, mesh_r.idx));
//        char* name = cel_strdup(name_buff, MemSysApiV0.main_scratch_allocator());
//
//        return (struct property_value) {
//                .type= PROPERTY_STRING,
//                .value.str = name
//        };
//    }

    return (struct property_value) {.type= PROPERTY_INVALID};
}


static void _init(get_api_fce_t get_engine_api) {
    INIT_API(ComponentSystemApi, COMPONENT_API_ID, 0);
    INIT_API(MemSysApi, MEMORY_API_ID, 0);
    INIT_API(MaterialApi, MATERIAL_API_ID, 0);
    INIT_API(MeshRendererApi, MESH_API_ID, 0);
    INIT_API(SceneGprahApi, SCENEGRAPH_API_ID, 0);
    INIT_API(TransformApi, TRANSFORM_API_ID, 0);
    INIT_API(ResourceApi, RESOURCE_API_ID, 0);

    _G = (struct G) {0};

    MAP_INIT(world_data_t, &_G.world, MemSysApiV0.main_allocator());

    _G.type = stringid64_from_string("mesh_renderer");

    ComponentSystemApiV0.component_register_compiler(_G.type,
                                                     _mesh_component_compiler,
                                                     10);

    ComponentSystemApiV0.component_register_type(_G.type,
                                                 (struct component_clb) {
                                                         .spawner=_spawner,
                                                         .destroyer=_destroyer,
                                                         .on_world_create=_on_world_create,
                                                         .on_world_destroy=_on_world_destroy,
                                                         .set_property=_set_property, .get_property=_get_property
                                                 });
}


void *mesh_get_module_api(int api,
                          int version) {

    switch (api) {
        case PLUGIN_EXPORT_API_ID:
            switch (version) {
                case 0: {
                    static struct module_api_v0 module = {0};

                    module.init = _init;
                    module.shutdown = _shutdown;

                    return &module;
                }

                default:
                    return NULL;
            };
        case MESH_API_ID:
            switch (version) {
                case 0: {
                    static struct MeshRendererApiV0 api = {0};

                    api.is_valid = mesh_is_valid;
                    api.has = mesh_has;
                    api.get = mesh_get;
                    api.create = mesh_create;
                    api.get_material = mesh_get_material;
                    api.set_material = mesh_set_material;
                    api.render_all = mesh_render_all;



                    return &api;
                }

                default:
                    return NULL;
            };

        default:
            return NULL;
    }
}
