//
// Created by vuonglk on 09/05/2024.
//

#ifndef SM_QUEUE_H
#define SM_QUEUE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct sm_queue {
    void *memory;
    size_t item_size;
    uint32_t max_capacity;
    uint32_t item_count;
} sm_queue_t;

sm_queue_t *sm_queue_create(uint32_t _capacity, size_t _item_size);

int32_t sm_queue_push_back(sm_queue_t *_queue, void *_item);

int32_t sm_queue_get_size(sm_queue_t *_queue);

void *sm_queue_get_font(sm_queue_t *_queue);

void *sm_queue_get_back(sm_queue_t *_queue);

int32_t sm_queue_pop(sm_queue_t *_queue, void *_buff);

int32_t sm_vector_destroy(sm_queue_t *_queue);

#ifdef __cplusplus
}
#endif

#endif //SM_QUEUE_H
