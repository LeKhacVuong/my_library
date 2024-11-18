//
// Created by vuonglk on 18/11/2024.
//

#include "sm_http_client.h"
#include "sm_logger.h"
#include "TimeUtils.h"

#define TAG "http_client_exam"

void log_put(const char* _log){
    printf("%s\n", _log);
}

sm_http_client_t* g_http = NULL;

char* g_url = "http://s3.ap-southeast-1.amazonaws.com/selex-bss-v2-log/ota_bss%2FBMS_V1.0.17.rc05%20.hex";

int main(){
    sm_logger_init(log_put, LOG_LEVEL_DEBUG);
    LOG_INF(TAG, "Http client test");

    
    
    INIT:
    g_http = sm_http_client_create(NULL);
    if(!g_http){
        LOG_ERR(TAG, "Create HTTP client FAILED!!!");
        delay(1000);
        goto INIT;
    }



    return 0;
}
