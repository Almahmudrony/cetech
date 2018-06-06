#include <cstdio>

#include <cetech/engine/debugui/debugui.h>
#include <cetech/kernel/fs/fs.h>
#include <cetech/kernel/hashlib/hashlib.h>
#include <cetech/kernel/config/config.h>
#include <cetech/kernel/memory/memory.h>
#include <cetech/kernel/api/api_system.h>
#include <cetech/kernel/module/module.h>
#include <cetech/kernel/cdb/cdb.h>
#include <cetech/engine/ecs/ecs.h>

#include <cetech/playground/asset_browser.h>
#include <cetech/playground/explorer.h>
#include <cetech/kernel/yaml/yng.h>
#include <cetech/kernel/yaml/ydb.h>
#include <cetech/playground/playground.h>
#include <cetech/engine/debugui/private/ocornut-imgui/imgui.h>
#include <cetech/engine/resource/resource.h>
#include <cetech/kernel/ebus/ebus.h>

CETECH_DECL_API(ct_memory_a0);
CETECH_DECL_API(ct_hashlib_a0);
CETECH_DECL_API(ct_debugui_a0);
CETECH_DECL_API(ct_cdb_a0);
CETECH_DECL_API(ct_resource_a0);
CETECH_DECL_API(ct_ebus_a0);


#define WINDOW_NAME "Explorer"
#define PLAYGROUND_MODULE_NAME CT_ID64_0("explorer")

#define _G explorer_globals
static struct _G {
    bool visible;

    struct ct_cdb_obj_t* selected_obj;
    uint32_t ent_name;
    struct ct_entity entity;
    struct ct_world world;

    const char *path;
    ct_alloc *allocator;
} _G;


void set_level(struct ct_world world,
               struct ct_entity level,
               uint64_t name,
               uint64_t root,
               const char *path) {

    CT_UNUSED(root);

    if (_G.ent_name == name) {
        return;
    }

    _G.ent_name = name;
    _G.entity = level;
    _G.world = world;
    _G.path = path;
}

static ct_explorer_a0 level_inspector_api = {
        .set_level = set_level,
};


static void ui_entity_item_end() {
    ct_debugui_a0.TreePop();
}

static void ui_entity_item_begin(ct_cdb_obj_t *obj, uint32_t id) {

    ImGuiTreeNodeFlags flags = DebugUITreeNodeFlags_OpenOnArrow |
                               DebugUITreeNodeFlags_OpenOnDoubleClick;

    bool selected = _G.selected_obj == obj;
    if (selected) {
        flags |= DebugUITreeNodeFlags_Selected;
    }

    ct_cdb_obj_t** children = ct_cdb_a0.children(obj);
    if (ct_array_size(children) == 0) {
        flags |= DebugUITreeNodeFlags_Leaf;
    }


    char name[128] = {0};
    uint64_t uid = ct_cdb_a0.read_uint64(obj, CT_ID64_0("uid"), 0);
    const char* ent_name = ct_cdb_a0.read_str(obj, CT_ID64_0("name"), NULL);
    if(ent_name) {
        strcpy(name, ent_name);
    } else {
        snprintf(name, CT_ARRAY_LEN(name), "%llu", uid);
    }

    char label[128] = {0};
    snprintf(label, CT_ARRAY_LEN(label), "%s", name);

    bool open = ct_debugui_a0.TreeNodeEx(label, flags);
    if (ct_debugui_a0.IsItemClicked(0)) {
        ct_ent_selected_ev ev = {
                .world  = _G.world,
                .entity = _G.entity,
                .filename = _G.path,
                .obj = obj,
        };

        ct_ebus_a0.broadcast(EXPLORER_EBUS,
                             EXPLORER_ENTITY_SELECT_EVENT, &ev, sizeof(ev));

        _G.selected_obj = obj;
    }

    if (open) {
        for (uint32_t i = 0; i < ct_array_size(children); ++i) {
            ui_entity_item_begin(children[i], rand());
        }
        ui_entity_item_end();
    }
}


static void on_debugui(void *event) {
    if (ct_debugui_a0.BeginDock(WINDOW_NAME, &_G.visible,
                                DebugUIWindowFlags_(0))) {

        ct_debugui_a0.LabelText("Entity", "%u", _G.ent_name);

        if (_G.path) {
            struct ct_resource_id rid = (struct ct_resource_id){
                    .type = CT_ID32_0("entity"),
                    .name = _G.ent_name,
            };

            ct_cdb_obj_t* obj = ct_resource_a0.get_obj(rid);

#define PROP_ENT_OBJ (CT_ID64_0("ent_obj") << 32)
            obj = ct_cdb_a0.read_ref(obj, PROP_ENT_OBJ, NULL);
            ui_entity_item_begin(obj, rand());
        }
    }

    ct_debugui_a0.EndDock();
}

static void on_menu_window(void *event) {
    CT_UNUSED(event);

    ct_debugui_a0.MenuItem2(WINDOW_NAME, NULL, &_G.visible, true);
}

static void _init(ct_api_a0 *api) {
    _G = {
            .allocator = ct_memory_a0.main_allocator(),
            .visible = true
    };

    api->register_api("ct_explorer_a0", &level_inspector_api);

    ct_ebus_a0.connect(PLAYGROUND_EBUS, PLAYGROUND_UI_EVENT, on_debugui, 0);
    ct_ebus_a0.connect(PLAYGROUND_EBUS, PLAYGROUND_UI_MAINMENU_EVENT, on_menu_window, 0);

    ct_ebus_a0.create_ebus(EXPLORER_EBUS_NAME, EXPLORER_EBUS);
}

static void _shutdown() {
    ct_ebus_a0.disconnect(PLAYGROUND_EBUS, PLAYGROUND_UI_EVENT, on_debugui);
    ct_ebus_a0.disconnect(PLAYGROUND_EBUS, PLAYGROUND_UI_MAINMENU_EVENT, on_menu_window);

    _G = {};
}

CETECH_MODULE_DEF(
        level_inspector,
        {
            CETECH_GET_API(api, ct_memory_a0);
            CETECH_GET_API(api, ct_hashlib_a0);
            CETECH_GET_API(api, ct_debugui_a0);
            CETECH_GET_API(api, ct_cdb_a0);
            CETECH_GET_API(api, ct_ebus_a0);
            CETECH_GET_API(api, ct_resource_a0);
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