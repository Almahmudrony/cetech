#ifndef CETECH_PROPERTY_INSPECTOR_H
#define CETECH_PROPERTY_INSPECTOR_H



//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>

//==============================================================================
// Typedefs
//==============================================================================
typedef void (*ct_pi_on_debugui)();

//==============================================================================
// Api
//==============================================================================

struct ct_property_editor_i0 {
    void (*draw)();
};

struct ct_property_editor_a0 {
    void (*set_active)(ct_pi_on_debugui on_debugui);
};

CT_MODULE(ct_property_editor_a0);

#endif //CETECH_PROPERTY_INSPECTOR_H
