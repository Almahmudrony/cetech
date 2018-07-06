#ifndef CETECH_SHADER_H
#define CETECH_SHADER_H

#include <corelib/cdb.h>



//==============================================================================
// Includes
//==============================================================================

#include <stdint.h>
#include <cetech/renderer/renderer.h>

//==============================================================================
// Typedefs
//==============================================================================

//==============================================================================
// Api
//==============================================================================

//==============================================================================
// Api
//==============================================================================

//! Shader API V0
struct ct_shader_a0 {
    struct ct_render_shader_handle (*get)(uint64_t shader);
};

CT_MODULE(ct_shader_a0);

#endif //CETECH_SHADER_H