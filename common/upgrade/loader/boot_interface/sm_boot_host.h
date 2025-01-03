//
// Created by vuonglk on 22/03/2023.
//

#ifndef SELEX_HOST_SM_BOOT_HOST_H
#define SELEX_HOST_SM_BOOT_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sm_boot_if.h"
#include "sm_host.h"

sm_boot_if_t* sm_boot_host_create(sm_host_t* _host);
sm_boot_if_t* sm_boot_host_create_default(sm_host_t* _host);

#ifdef __cplusplus
};
#endif
#endif //SELEX_HOST_SM_BOOT_HOST_H
