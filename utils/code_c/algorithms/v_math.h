//
// Created by vuonglk on 28/11/2024.
//

#ifndef LIBS_STORAGE_V_MATH_H
#define LIBS_STORAGE_V_MATH_H
#include "stdint.h"


inline uint32_t v_max_off(uint32_t a, uint32_t b){
    return (a > b) ? a : b;
}

inline uint32_t v_min_off(uint32_t a, uint32_t b){
    return (a < b) ? a : b;
}

#endif //LIBS_STORAGE_V_MATH_H
