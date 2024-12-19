//
// Created by lekhacvuong on 12/19/24.
//

char* g_url = "http://s3.ap-southeast-1.amazonaws.com/selex-bss-v2-log/ota_bss%2FBMS_V1.0.17.rc05%20.hex";

char* g_git_link = "https://raw.githubusercontent.com/LeKhacVuong/http_file_storage/refs/heads/master/self_info.txt";

/*=====================================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};
CURL *curl_handle;

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{

    char *ct;
    /* ask for the content-type */
    int res = curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ct);

    if((CURLE_OK == res) && ct)
        printf("We received Content-Type: %s\n", ct);

    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(void)
{
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, g_git_link);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    char buff_range[20] = "0-15";

    curl_easy_setopt(curl_handle, CURLOPT_RANGE, buff_range);

    memset(buff_range, 0, 20);

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    else {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *
         * Do something nice with it!
         */

        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    free(chunk.memory);

    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();

    return 0;
}

