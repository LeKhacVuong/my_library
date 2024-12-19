//
// Created by vnbk on 18/09/2024.
//
#include <curl/curl.h>
#include <string.h>

#include "sm_http_client.h"
#include "sm_logger.h"

#define TAG "HTTP_CLIENT_CURL"

#define _impl(x)    ((sm_http_curl_t*)(x))

typedef struct{
    CURL* m_curl;
    struct curl_slist* m_headers;

    sm_http_client_response_fn_t m_response;
    void* m_arg;
}sm_http_curl_t;

static sm_http_curl_t g_http_client = {
        .m_curl = NULL,
        .m_headers = NULL,
        .m_response = NULL,
        .m_arg = NULL
};

static size_t http_write_callback(void *data, size_t size, size_t nmemb, void *_arg){
    sm_http_curl_t* http_client = (sm_http_curl_t*)_arg;

    return 0;
}

sm_http_client_t* sm_http_client_create(void* _net){
    (void)_net;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    g_http_client.m_curl = curl_easy_init();
    if(!g_http_client.m_curl){
        LOG_ERR(TAG, "CURL handle NULL");
        return NULL;
    }

    return &g_http_client;
}

int32_t sm_http_client_free(sm_http_client_t* _this){
    if(!_this){
        return -1;
    }

    curl_global_cleanup();

    return 0;
}

int32_t sm_http_client_add_header_default(sm_http_client_t* _this){
    if(!_this){
        return -1;
    }



    return 0;
}

int32_t sm_http_client_add_header(sm_http_client_t* _this, const char* _type, const char* _content){
    if(!_this || !_type || !_content){
        return -1;
    }




    return 0;
}

int32_t sm_http_client_add_headers(sm_http_client_t* _this, const sm_http_header_t* _headers, uint32_t _len){
    return 0;
}

int32_t sm_http_client_set_range_header(sm_http_client_t* _this, uint32_t _start, uint32_t _length){
    if(!_this){
        return -1;
    }
    sm_http_curl_t* this = _this;

    char range[128] = {0,};
    sprintf(range, "%d-%d", _start, _start + _length - 1);

    curl_easy_setopt(this->m_curl, CURLOPT_RANGE, range);
    return 0;
}

int32_t sm_http_client_set_url(sm_http_client_t* _this, const char* _url){
    if(!_this || !_url){
        return -1;
    }
    sm_http_curl_t* this = _this;

    curl_easy_setopt(this->m_curl, CURLOPT_URL, "https://example.com");

    return 0;
}

int32_t sm_http_client_get_method(sm_http_client_t* _this,
                                  const char* _url,
                                  sm_http_client_response_fn_t _cb,
                                  void* _arg){
    if(!_this){
        return -1;
    }


    int result = 0;




    return result;
}

int32_t sm_http_client_post_method(sm_http_client_t* _this,
                                   const char* _url,
                                   const char* _request_payload,
                                   sm_http_client_response_fn_t _cb,
                                   void* _arg){



    return -1;
}
