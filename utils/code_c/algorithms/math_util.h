//
// Created by vuonglk on 28/11/2024.
//

#ifndef LIBS_STORAGE_MATH_UTIL_H
#define LIBS_STORAGE_MATH_UTIL_H
#include "stdint.h"


inline uint32_t math_get_larger_uint32(uint32_t a, uint32_t b){
    return (a > b) ? a : b;
}

inline uint32_t math_get_smaller_uint32(uint32_t a, uint32_t b){
    return (a < b) ? a : b;
}

#endif //LIBS_STORAGE_MATH_UTIL_H
