//
//                      **Dynamic growing array**
// # Description
//
// ****************************************************
// *
// * header            pointer    size  capacity
// *    |                 |         |       |
// *    v                 v         v       v
// *    +-----------------+---+---+---+---+---+
// *    | size | capacity | O | 1 | 2 |   |   |
// *    +-----------------+---+---+---+---+---+
// *
// *
// ****************************************************
//
// # Example
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// char* buffer = NULL;
//
// ct_array_push(buffer, 'h', ct_memory_a0->system);
// ct_array_push(buffer, 'e', ct_memory_a0->system);
// ct_array_push(buffer, 'l', ct_memory_a0->system);
// ct_array_push(buffer, 'l', ct_memory_a0->system);
// ct_array_push(buffer, 'o', ct_memory_a0->system);
// ct_array_push(buffer, '\0', ct_memory_a0->system);
//
// printf("%s\n", buffer);
//
// ct_array_free(buffer, ct_memory_a0->system);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// # API

#ifndef CETECH_ARRAY_H
#define CETECH_ARRAY_H

#include <stdint.h>
#include <string.h>
#include <memory.h>

#include <corelib/macros.h>
#include "allocator.h"

// Array header (pointer preambule)
struct ct_array_header_t {
    uint32_t size;
    uint32_t capacity;
};

// Free array.
#define ct_array_free(a, alloc) \
    ((a) ?  CT_FREE(alloc, ct_array_header(a)) : 0, a = NULL )

// Get array [header ](#ct_array_header_t).
#define ct_array_header(a) \
    ((a) ? (struct ct_array_header_t *)((char *)(a) - sizeof(struct ct_array_header_t)): NULL)

// Get array size.
#define ct_array_size(a) \
    ((a) ? ct_array_header(a)->size : 0)

// Get array capacity.
#define ct_array_capacity(a) \
     ((a) ? ct_array_header(a)->capacity : 0)

// Clean array (only set size = 0)
#define ct_array_clean(a) \
     ((a) ? ct_array_header(a)->size = 0 : 0)

// Set array capacity
#define ct_array_set_capacity(a, c, alloc) \
     ((a) = (__typeof(a)) ct_array_grow(a, c, sizeof(*(a)), CT_ALIGNOF(*(a)), alloc, __FILE__, __LINE__))

// Resize array
#define ct_array_resize(a, c, alloc) \
     (ct_array_set_capacity(a, c, alloc), ct_array_header(a)->size = (c))

// Is array emty?
#define ct_array_empty(a) \
    (ct_array_size(a) == 0)

// Has array any element?
#define ct_array_any(a) \
    (ct_array_size(a) > 0)

// Is array full?
#define ct_array_full(a) \
    ct_array_full_n(a, 1)

// Is array full for n elemenets?
#define ct_array_full_n(a, n) \
     ((a) ? (ct_array_size(a) + (n)) >= ct_array_capacity(a) : 1)

// Push value to array
#define ct_array_push(a, item, alloc) \
    (ct_array_full(a) ? (a) = (__typeof(a))ct_array_grow(a, ct_array_size(a) + 1, sizeof(*(a)), CT_ALIGNOF(*(a)), alloc, __FILE__, __LINE__) : 0, \
    (a)[ct_array_header(a)->size++] = item)

// Push n values from items to array
#define ct_array_push_n(a, items, n, alloc) \
    (ct_array_full_n(a, n) ? (a) =  (__typeof(a))ct_array_grow(a, ct_array_size(a) + (n), sizeof(*(a)), CT_ALIGNOF(*(a)), alloc, __FILE__, __LINE__) : 0, \
    memcpy((a)+ct_array_header(a)->size, (items), sizeof(*(a)) * (n)), \
    ct_array_header(a)->size += (n))

// Pop element from front
#define ct_array_pop_front(a) \
    (ct_array_any(a) ? memmove(a, ((a)+1), sizeof(*(a)) * (ct_array_header(a)->size--)) : 0)

// Pop element from back
#define ct_array_pop_back(a) \
    (ct_array_any(a) ? ct_array_header(a)->size-- : 0)

// Insert element before element at *idx*
#define ct_array_insert(a, idx, v, alloc) \
    do {\
        ct_array_resize(a, ct_array_size(a) + 1, alloc);\
        memcpy(a+idx+1,a+idx, (ct_array_size(a)-idx)*sizeof(*(a)));\
        a[idx] = v;\
    } while(0)

// Get front element
#define ct_array_front(a) \
    a[0]

// Get back element
#define ct_array_back(a) \
    a[ct_array_size(a)-1]

// Grow array if need.
static inline void *ct_array_grow(void *array,
                                  uint32_t capacity,
                                  size_t type_size,
                                  size_t type_align,
                                  const struct ct_alloc *alloc,
                                  const char *filename,
                                  uint32_t line) {
    if (capacity < ct_array_capacity(array)) {
        return array;
    }

    uint32_t new_capacity = (ct_array_capacity(array) * 2) + 8;
    if (new_capacity < capacity) {
        new_capacity = capacity;
    }

    const uint32_t orig_size = ct_array_size(array);

    const uint32_t size = sizeof(struct ct_array_header_t) + (new_capacity
                                                              * type_size);

    void *new_data = alloc->call->reallocate(alloc, ct_array_header(array),
                                             size, type_align, filename, line);

    char *new_array = (char *) new_data + sizeof(struct ct_array_header_t);

    *((struct ct_array_header_t *) new_data) = (struct ct_array_header_t) {
            .size = orig_size,
            .capacity = new_capacity
    };

    return new_array;
}

#endif //CETECH_ARRAY_H
