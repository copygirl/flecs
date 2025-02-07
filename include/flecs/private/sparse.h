/**
 * @file sparse.h
 * @brief Sparse set data structure.
 */

#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H

#include "flecs/private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The number of elements in a single chunk */
#define FLECS_SPARSE_CHUNK_SIZE (4096)

typedef struct ecs_sparse_t {
    ecs_vector_t *dense;        /* Dense array with indices to sparse array. The
                                 * dense array stores both alive and not alive
                                 * sparse indices. The 'count' member keeps
                                 * track of which indices are alive. */

    ecs_vector_t *chunks;       /* Chunks with sparse arrays & data */
    ecs_size_t size;            /* Element size */
    int32_t count;              /* Number of alive entries */
    uint64_t max_id_local;      /* Local max index (if no global is set) */
    uint64_t *max_id;           /* Maximum issued sparse index */
    struct ecs_allocator_t *allocator;
    struct ecs_block_allocator_t *chunk_allocator;
} ecs_sparse_t;

/** Initialize sparse set */
FLECS_DBG_API
void _flecs_sparse_init(
    ecs_sparse_t *sparse,
    struct ecs_allocator_t *allocator,
    struct ecs_block_allocator_t *chunk_allocator,
    ecs_size_t elem_size);

#define flecs_sparse_init(sparse, allocator, chunk_allocator, T)\
    _flecs_sparse_init(sparse, allocator, chunk_allocator, ECS_SIZEOF(T))

/** Create new sparse set */
FLECS_DBG_API
ecs_sparse_t* _flecs_sparse_new(
    struct ecs_allocator_t *allocator,
    struct ecs_block_allocator_t *chunk_allocator,
    ecs_size_t elem_size);

#define flecs_sparse_new(allocator, chunk_allocator, T)\
    _flecs_sparse_new(allocator, chunk_allocator, ECS_SIZEOF(T))

FLECS_DBG_API
void _flecs_sparse_fini(
    ecs_sparse_t *sparse);

#define flecs_sparse_fini(sparse)\
    _flecs_sparse_fini(sparse)

/** Free sparse set */
FLECS_DBG_API
void flecs_sparse_free(
    ecs_sparse_t *sparse);

/** Remove all elements from sparse set */
FLECS_DBG_API
void flecs_sparse_clear(
    ecs_sparse_t *sparse);

/** Set id source. This allows the sparse set to use an external variable for
 * issuing and increasing new ids. */
FLECS_DBG_API
void flecs_sparse_set_id_source(
    ecs_sparse_t *sparse,
    uint64_t *id_source);

/** Add element to sparse set, this generates or recycles an id */
FLECS_DBG_API
void* _flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define flecs_sparse_add(sparse, T)\
    ((T*)_flecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get last issued id. */
FLECS_DBG_API
uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Generate or recycle a new id. */
FLECS_DBG_API
uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse);

/** Generate or recycle new ids in bulk. The returned pointer points directly to
 * the internal dense array vector with sparse ids. Operations on the sparse set
 * can (and likely will) modify the contents of the buffer. */
FLECS_DBG_API
const uint64_t* flecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t count);

/** Remove an element */
FLECS_DBG_API
void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t id);

/** Fast version of remove, no liveliness checking */
FLECS_DBG_API
void* _flecs_sparse_remove_fast(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Remove an element, return pointer to the value in the sparse array */
FLECS_DBG_API
void* _flecs_sparse_remove_get(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);    

#define flecs_sparse_remove_get(sparse, T, index)\
    ((T*)_flecs_sparse_remove_get(sparse, ECS_SIZEOF(T), index))

/** Check whether an id has ever been issued. */
FLECS_DBG_API
bool flecs_sparse_exists(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Check whether an id has ever been issued and is currently alive. */
FLECS_DBG_API
bool flecs_sparse_is_valid(
    const ecs_sparse_t *sparse,
    uint64_t index);

/** Test if id is alive, which requires the generation count to match. */
FLECS_DBG_API
bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Return identifier with current generation set. */
FLECS_DBG_API
uint64_t flecs_sparse_get_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get value from sparse set by dense id. This function is useful in 
 * combination with flecs_sparse_count for iterating all values in the set. */
FLECS_DBG_API
void* _flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define flecs_sparse_get_dense(sparse, T, index)\
    ((T*)_flecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))


/** Get the number of alive elements in the sparse set. */
FLECS_DBG_API
int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Get the number of not alive alive elements in the sparse set. */
FLECS_DBG_API
int32_t flecs_sparse_not_alive_count(
    const ecs_sparse_t *sparse);

/** Return total number of allocated elements in the dense array */
FLECS_DBG_API
int32_t flecs_sparse_size(
    const ecs_sparse_t *sparse);

/** Get element by (sparse) id. The returned pointer is stable for the duration
 * of the sparse set, as it is stored in the sparse array. */
FLECS_DBG_API
void* _flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get(sparse, T, index)\
    ((T*)_flecs_sparse_get(sparse, ECS_SIZEOF(T), index))

/** Like get_sparse, but don't care whether element is alive or not. */
FLECS_DBG_API
void* _flecs_sparse_get_any(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get_any(sparse, T, index)\
    ((T*)_flecs_sparse_get_any(sparse, ECS_SIZEOF(T), index))

/** Get or create element by (sparse) id. */
FLECS_DBG_API
void* _flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_ensure(sparse, T, index)\
    ((T*)_flecs_sparse_ensure(sparse, ECS_SIZEOF(T), index))

/** Fast version of ensure, no liveliness checking */
FLECS_DBG_API
void* _flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_ensure_fast(sparse, T, index)\
    ((T*)_flecs_sparse_ensure_fast(sparse, ECS_SIZEOF(T), index))

/** Set value. */
FLECS_DBG_API
void* _flecs_sparse_set(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id,
    void *value);

#define flecs_sparse_set(sparse, T, index, value)\
    ((T*)_flecs_sparse_set(sparse, ECS_SIZEOF(T), index, value))

/** Get pointer to ids (alive and not alive). Use with count() or size(). */
FLECS_DBG_API
const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse);

/** Set size of the dense array. */
FLECS_DBG_API
void flecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count);

/** Copy sparse set into a new sparse set. */
FLECS_DBG_API
ecs_sparse_t* flecs_sparse_copy(
    const ecs_sparse_t *src);    

/** Restore sparse set into destination sparse set. */
FLECS_DBG_API
void flecs_sparse_restore(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src);

/* Publicly exposed APIs 
 * The flecs_ functions aren't exposed directly as this can cause some
 * optimizers to not consider them for link time optimization. */

FLECS_API
ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size);

#define ecs_sparse_new(T)\
    _ecs_sparse_new(ECS_SIZEOF(T))

FLECS_API
void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define ecs_sparse_add(sparse, T)\
    ((T*)_ecs_sparse_add(sparse, ECS_SIZEOF(T)))

FLECS_API
uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse);

FLECS_API
int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Override the generation count for a specific id */
FLECS_API
void flecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t id);

FLECS_API
void* _ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define ecs_sparse_get_dense(sparse, T, index)\
    ((T*)_ecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

FLECS_API
void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define ecs_sparse_get(sparse, T, index)\
    ((T*)_ecs_sparse_get(sparse, ECS_SIZEOF(T), index))

#ifdef __cplusplus
}
#endif

#endif
