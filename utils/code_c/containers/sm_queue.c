//
// Created by vuonglk on 09/05/2024.
//

#include "sm_queue.h"

#include <string.h>

sm_queue_t* sm_queue_create(uint32_t _capacity, size_t _item_size)
{
    sm_queue_t* queue = malloc(sizeof(sm_queue_t));

    if (queue == NULL)
    {
        printf("cant create vector instance");
        return NULL;
    }
    queue->max_capacity = _capacity;
    queue->item_size = _item_size;
    queue->memory = malloc(_capacity * _item_size);
    if (queue->memory == NULL)
    {
        printf("cant create vector memory");
        free(queue);
        return NULL;
    }
    queue->item_count = 0;
    return queue;
}

int32_t sm_queue_push_back(sm_queue_t* _queue, void* _item)
{
    if (_queue == NULL) return -1;
    if (_queue->item_count >= _queue->max_capacity) return -1;

    memcpy(_queue->memory + (_queue->item_count * _queue->item_size), _item, _queue->item_size);
    _queue->item_count++;
    return 0;
}

int32_t sm_queue_get_size(sm_queue_t* _queue)
{
    if (_queue == NULL) return -1;
    return _queue->item_count;
}

void* sm_queue_get_font(sm_queue_t* _queue)
{
    if (_queue == NULL) return NULL;
    if (_queue->item_size == 0) return NULL;
    unsigned char* ptr;
    ptr = _queue->memory;
    return ptr;
}

void* sm_queue_get_back(sm_queue_t* _queue)
{
    if (_queue == NULL) return NULL;
    if (_queue->item_size == 0) return NULL;
    unsigned char* ptr;
    ptr = _queue->memory + (_queue->item_count - 1) * _queue->item_size;
    return ptr;
}

int32_t sm_queue_pop(sm_queue_t* _queue, void* _buff)
{
    if (_queue == NULL || _buff == NULL) return -1;
    if (_queue->item_size == 0) return -1;
    memcpy(_buff, _queue->memory, _queue->item_size);
    _queue->item_count--;
    memmove(_queue->memory, _queue->memory + _queue->item_size, _queue->item_count * _queue->item_size);
    return 0;
}

int32_t sm_vector_destroy(sm_queue_t* _queue)
{
    if (_queue == NULL) return -1;
    free(_queue->memory);
    free(_queue);
    return 0;
}
