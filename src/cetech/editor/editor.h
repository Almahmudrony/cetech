#ifndef CETECH_PLAYGROUND_H
#define CETECH_PLAYGROUND_H

//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>
#include <stdint.h>

#define EDITOR_MODULE_INTERFACE_NAME \
    "ct_editor_module_i0"

#define EDITOR_MODULE_INTERFACE \
    CT_ID64_0("ct_editor_module_i0", 0x761dbf8cf91061a1ULL)

struct ct_editor_module_i0 {
    bool (*init)();

    bool (*shutdown)();

    void (*update)(float dt);

    void (*render)();
};

#endif //CETECH_PLAYGROUND_H
