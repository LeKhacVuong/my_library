#include <assert.h>
#include <stdio.h>
#include "v_link_list.h"
#include "sm_logger.h"

#define TAG "main"

void printf_link_list(v_link_list_t* linkList){
    uint32_t size = v_link_list_get_size(linkList);
    int* value = NULL;

    LOG_INF(TAG, "Link list contain %d node, value bellow", v_link_list_get_size(linkList));
    for(int i = 0; i < size; i++){
        value = v_link_list_get_item(linkList, i);
        if(value){
            LOG_INF(TAG, "Node id %2d value: %d", i, *value);
        }
    }
}

static int32_t int_cmp_fn_t(void* first, void* second){
    int32_t* a = first, *b = second;
    return *a - *b;
}

int main() {
    int test[11] = {11, 22, 44, 12, 62, 55, 23, 34, 88, 45, 23};
    v_link_list_t* linkList = v_link_list_create_default(sizeof(int));
    if(!linkList){
        LOG_ERR(TAG, "Create link list FAILED");
        return -1;
    }
    LOG_INF(TAG, "Now list have %d item", v_link_list_get_size(linkList));

    for(int i = 0; i < 11; i++){
        v_link_list_push_back(linkList, test + i);
        LOG_INF(TAG, "Now list have %d item", v_link_list_get_size(linkList));
    }


    printf_link_list(linkList);

    v_link_list_sort_desc(linkList, int_cmp_fn_t);

    printf_link_list(linkList);

    v_link_list_reverse(linkList);

    printf_link_list(linkList);

    return 0;
}
