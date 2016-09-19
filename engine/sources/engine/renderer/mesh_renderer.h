#ifndef CETECH_MESH_RENDERER_H
#define CETECH_MESH_RENDERER_H

#include <celib/stringid/types.h>
#include "../world_system/types.h"
#include "../entcom/types.h"
#include "material.h"

typedef struct {
    u32 idx;
} mesh_t;

int mesh_init();

void mesh_shutdown();

int mesh_is_valid(mesh_t mesh);

int mesh_has(world_t world,
             entity_t entity);

mesh_t mesh_get(world_t world,
                entity_t entity);

mesh_t mesh_create(world_t world,
                   entity_t entity,
                   stringid64_t scene,
                   stringid64_t mesh,
                   stringid64_t node,
                   stringid64_t material);

material_t mesh_get_material(world_t world,
                             mesh_t mesh);

void mesh_render_all(world_t world);


#endif //CETECH_MESH_RENDERER_H