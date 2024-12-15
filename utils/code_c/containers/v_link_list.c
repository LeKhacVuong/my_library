//
// Created by lekhacvuong on 12/14/24.
//


#include <string.h>
#include <stdio.h>
#include "v_link_list.h"
#include "stdlib.h"

typedef struct link_node link_node_t;

struct link_node{
    void* value;
    link_node_t* next;
};

typedef struct{
    link_node_t* first;
    uint32_t item_size;
    uint32_t cur_size;
}v_link_list_impl_t;

static v_link_list_impl_t g_list;

static int32_t swap_link_node(link_node_t* _first, link_node_t* _second, uint32_t _size){
    if(!_first ||  !_second){
        return -1;
    }
    void* temp = malloc(_size);

    memcpy(temp, _first->value, _size);
    memcpy(_first->value, _second->value, _size);
    memcpy(_second->value, temp, _size);

    free(temp);
    return 0;
}

static link_node_t* v_link_list_get_link_node(v_link_list_t* _this, uint32_t _index){
    if(!_this){
        return NULL;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* node = this->first;
    for(int i = 0; i < _index; i++){
        node = node->next;
        if(!node){
            break;
        }
    }

    if(node && node->value){
        return node;
    }

    return NULL;
}

v_link_list_t* v_link_list_create_default(uint32_t _item_size){
    v_link_list_impl_t* this = &g_list;
    this->item_size = _item_size;
    this->first = NULL;
    this->cur_size = 0;
    return this;
}

v_link_list_t* v_link_list_create(uint32_t _item_size){
    v_link_list_impl_t* this = malloc(sizeof(v_link_list_impl_t));
    if(!this){
        return NULL;
    }
    this->item_size = _item_size;
    this->first = NULL;
    this->cur_size = 0;
    return this;
}

int32_t v_link_list_push_back(v_link_list_t* _this, void* _item){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* new_node = malloc(sizeof(link_node_t));
    if(!new_node){
        return -1;
    }
    new_node->value = malloc(this->item_size);
    if(!new_node->value){
        free(new_node);
        return -1;
    }
    new_node->next = NULL;
    memcpy(new_node->value, _item, this->item_size);
    link_node_t* node = this->first;

    if(!node){
        this->first = new_node;
    } else{
        while (node){
            if(!node->next){
                node->next = new_node;
                break;
            }
            node = node->next;
        }
    }
    return 0;
}

int32_t v_link_list_push_font(v_link_list_t* _this, void* _item){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* new_node = malloc(sizeof(link_node_t));
    if(!new_node){
        return -1;
    }
    new_node->value = malloc(this->item_size);
    if(!new_node->value){
        free(new_node);
        return -1;
    }
    new_node->next = this->first;
    memcpy(new_node->value, _item, this->item_size);
    this->first = new_node;
    return 0;
}


int32_t v_link_list_get_size(v_link_list_t* _this){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* node = this->first;
    int32_t count = 0;

    while (node){
        count++;
        if(!node->next){
            break;
        }
        node = node->next;
    }
    return count;
}

void* v_link_list_get_item(v_link_list_t* _this, uint32_t _index){
    if(!_this){
        return NULL;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* node = v_link_list_get_link_node(_this, _index);

    return node? node->value : NULL;
}

void* v_link_list_get_last(v_link_list_t* _this){
    if(!_this){
        return NULL;
    }
    v_link_list_impl_t* this = _this;

    if(!this->first){
        return NULL;
    }

    link_node_t* node = this->first;

    while(node->next){
        node = node->next;
    }

    return node->value;
}

void* v_link_list_get_first(v_link_list_t* _this){
    if(!_this){
        return NULL;
    }
    v_link_list_impl_t* this = _this;

    if(!this->first){
        return NULL;
    }
    return this->first->value;
}

int32_t v_link_list_remove_item(v_link_list_t* _this, void* _item){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* node = this->first;
    link_node_t* pre_node = NULL;

    while (node){
        if(!memcmp(node->value, _item, this->item_size)){
            if(pre_node){
                pre_node->next = node->next;
            }
            free(node->value);
            free(node);
            return 0;
        }
        if(!node->next){
            break;
        }
        pre_node = node;
        node = node->next;
    }

    return -1;
}

int32_t v_link_list_remove_item_at(v_link_list_t* _this, uint32_t _index){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    link_node_t* node = this->first;
    link_node_t* pre_node = NULL;

    for(int i = 0; i < _index; i++){
        pre_node = node;
        node = node->next;
        if(!node){
            break;
        }
    }

    if(node){
        if(pre_node){
            pre_node->next = node->next;
        }
        free(node->value);
        free(node);
        return 0;
    }
    return -1;
}

int32_t v_link_list_reverse(v_link_list_t* _this){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    int32_t max_node = v_link_list_get_size(this) - 1;
    if(max_node <= 0){
        return -1;
    }

    for(int i = 0; i <= (max_node / 2) ; i++){
        link_node_t* node_first = v_link_list_get_link_node(this, i);
        link_node_t* node_last  = v_link_list_get_link_node(this, max_node - i);
        swap_link_node(node_first, node_last, this->item_size);
    }

    return 0;
}

int32_t v_link_list_sort_asc(v_link_list_t* _this, v_link_list_cmp_fn_t _cmp_fn){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    int32_t max_node = v_link_list_get_size(this) - 1;
    if(max_node <= 0){
        return -1;
    }

    for(int i = max_node; i >= 0; i--){
        for(int j = 0; j <= i; j++){
            link_node_t* node_first = v_link_list_get_link_node(this, j);
            link_node_t* node_last  = v_link_list_get_link_node(this, i);
            if(_cmp_fn(node_first->value, node_last->value) > 0){
                swap_link_node(node_first, node_last, this->item_size);
            }
        }
    }
    return 0;
}

int32_t v_link_list_sort_desc(v_link_list_t* _this, v_link_list_cmp_fn_t _cmp_fn){
    if(!_this){
        return -1;
    }
    v_link_list_impl_t* this = _this;

    int32_t max_node = v_link_list_get_size(this) - 1;
    if(max_node <= 0){
        return -1;
    }

    for(int i = max_node; i >= 0; i--){
        for(int j = 0; j <= i; j++){
            link_node_t* node_first = v_link_list_get_link_node(this, j);
            link_node_t* node_last  = v_link_list_get_link_node(this, i);
            if(_cmp_fn(node_first->value, node_last->value) < 0){
                swap_link_node(node_first, node_last, this->item_size);
            }
        }
    }
    return 0;
}
