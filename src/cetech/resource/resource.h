#ifndef CETECH_RESOURCE_H
#define CETECH_RESOURCE_H


//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <corelib/module.inl>

#define RESOURCE_I_NAME "ct_resource_i0"

#define PROP_RESOURECE_DATA \
    CT_ID64_0("data", 0x8fd0d44d20650b68ULL)

#define RESOURCE_I \
    CT_ID64_0("ct_resource_i0", 0x3e0127963a0db5b9ULL)


struct ct_vio;
struct ct_alloc;
struct ct_config_a0;

//==============================================================================
// Typedefs
//==============================================================================

typedef void (*ct_resource_compilator_t)(const char *filename,
                                         char **output);


//==============================================================================
// Structs
//==============================================================================

struct ct_resource_id {
    union {
        struct {
            uint64_t name;
            uint64_t type;
        };
        uint64_t i128[2];
    };
};

//! Resource callbacks
struct ct_resource_i0 {
    uint64_t (*cdb_type)();

    void *(*get_interface)(uint64_t name_hash);

    void (*online)(uint64_t name,
                   struct ct_vio *input,
                   uint64_t obj);

    void (*offline)(uint64_t name,
                    uint64_t obj);

    void (*compilator)(const char *filename,
                       char **output);
};



//==============================================================================
// Api
//==============================================================================


struct ct_resource_a0 {
    struct ct_resource_i0 *(*get_interface)(uint64_t type);

    void (*set_autoload)(bool enable);

    void (*load)(uint64_t type,
                 uint64_t *names,
                 size_t count,
                 int force);

    void (*load_now)(uint64_t type,
                     uint64_t *names,
                     size_t count);

    void (*unload)(uint64_t type,
                   uint64_t *names,
                   size_t count);

    void (*reload)(uint64_t type,
                   uint64_t *names,
                   size_t count);


    void (*reload_all)();


    int (*can_get)(uint64_t type,
                   uint64_t names);

    int (*can_get_all)(uint64_t type,
                       uint64_t *names,
                       size_t count);

    uint64_t (*get)(struct ct_resource_id resource_id);

    int (*type_name_string)(char *str,
                            size_t max_len,
                            struct ct_resource_id resourceid);

    void (*compiler_compile_all)();

    void (*compile_and_reload)(const char *filename);

    int (*compiler_get_filename)(char *filename,
                                 size_t max_ken,
                                 struct ct_resource_id resourceid);

    char *(*compiler_get_tmp_dir)(struct ct_alloc *a,
                                  const char *platform);

    char *(*compiler_external_join)(struct ct_alloc *a,
                                    const char *name);

    void (*type_name_from_filename)(const char *fullname,
                                    struct ct_resource_id *resource_id,
                                    char *short_name);


};

CT_MODULE(ct_resource_a0);

#endif //CETECH_RESOURCE_H
