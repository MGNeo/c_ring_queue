#ifndef C_RING_QUEUE_H
#define C_RING_QUEUE_H

#include <stddef.h>

typedef struct s_c_ring_queue c_ring_queue;

c_ring_queue *c_ring_queue_create(const size_t _capacity,
                                  size_t *const _error);

ptrdiff_t c_ring_queue_delete(c_ring_queue *const _ring_queue,
                              void (*const _del_data)(void *const _data));

ptrdiff_t c_ring_queue_clear(c_ring_queue *const _ring_queue,
                             void (*const _del_data)(void *const _data));

ptrdiff_t c_ring_queue_push(c_ring_queue *const _ring_queue,
                            const void *const _data,
                            void (*const _del_data)(void *const _data));

ptrdiff_t c_ring_queue_pop(c_ring_queue *const _ring_queue,
                           void (*const _del_data)(void *const _data));

ptrdiff_t c_ring_queue_for_each(c_ring_queue *const _ring_queue,
                                void (*const _action_data)(void *const _data));

size_t c_ring_queue_count(const c_ring_queue *const _ring_queue,
                          size_t *const _error);

size_t c_ring_queue_capacity(const c_ring_queue *const _ring_queue);

#endif
