#ifndef CETECH_LEVEL_BLOB_H
#define CETECH_LEVEL_BLOB_H

#include <celib/string/stringid.h>
#include <celib/math/types.h>

typedef struct level_blob {
    u32 units_count;
    // stringid64_t names[units_count];
    // u32 offset[units_count];
    // u8 data[*];
} level_blob_t;

#define level_blob_names(r) ((stringid64_t*) ((r) + 1))
#define level_blob_offset(r) ((u32*) (level_blob_names(r) + ((r)->units_count)))
#define level_blob_data(r) ((u8*) (level_blob_offset(r) + ((r)->units_count)))

#endif // CETECH_LEVEL_BLOB_H