//
// Created by lekhacvuong on 12/14/24.
//

#ifndef LIBS_STORAGE_V_LINK_LIST_H
#define LIBS_STORAGE_V_LINK_LIST_H
#include "stdint.h"

typedef void v_link_list_t;

typedef int32_t(*v_link_list_cmp_fn_t)(void* first, void* second);

v_link_list_t* v_link_list_create_default(uint32_t _item_size);

v_link_list_t* v_link_list_create(uint32_t _item_size);

int32_t v_link_list_push_back(v_link_list_t* _this, void* _item);

int32_t v_link_list_push_font(v_link_list_t* _this, void* _item);

int32_t v_link_list_get_size(v_link_list_t* _this);

void* v_link_list_get_item(v_link_list_t* _this, uint32_t _index);

void* v_link_list_get_last(v_link_list_t* _this);

void* v_link_list_get_first(v_link_list_t* _this);

int32_t v_link_list_remove_item(v_link_list_t* _this, void* _item);

int32_t v_link_list_remove_item_at(v_link_list_t* _this, uint32_t _index);

int32_t v_link_list_reverse(v_link_list_t* _this);

int32_t v_link_list_sort_asc(v_link_list_t* _this, v_link_list_cmp_fn_t _cmp_fn);

int32_t v_link_list_sort_desc(v_link_list_t* _this, v_link_list_cmp_fn_t _cmp_fn);

#endif //LIBS_STORAGE_V_LINK_LIST_H
