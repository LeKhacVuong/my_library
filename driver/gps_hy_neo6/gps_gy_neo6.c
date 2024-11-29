//
// Created by vuonglk on 28/11/2024.
//
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gps_gy_neo6.h"
#include "v_modem.h"
#include "sm_logger.h"
#include "ctype.h"

#define TAG "gps_gy_driver"

#define GPS_BUFFER_LENGTH 512

/*********** Global Positioning System *************/

enum {
    GPRMC = 0,
    GPVTG,
    GPGGA,
    GPGSA,
    GPGSV,
    GPGLL
};

const char* NMEA[] = {
        "$GPRMC",
        "$GPVTG",
        "$GPGGA",
        "$GPGSA",
        "$GPGSV",
        "$GPGLL"
};

typedef struct{
    v_modem_t* m_modem;
    char m_buffer[GPS_BUFFER_LENGTH];

    float           m_lat;
    float           m_lon;

    gps_date_time_t m_time;

}gps_gy_driver_impl_t;

static gps_gy_driver_impl_t g_instance;

static void GPRMC_decode(gps_gy_driver_impl_t* this, char* _raw_data){
    //$GPRMC,155105.00,A,2102.59668,N,10546.60809,E,0.260,,291124,,,A*78

    char* token;
    char *check;

    token = strsep(&_raw_data, ",");  //1, name
    if(strcmp(token, NMEA[GPRMC]) != 0){
        LOG_ERR(TAG, "Invalid GPRMC packet!!");
        return;
    }

    token = strsep(&_raw_data,","); //2, time
    if(token && strlen(token)){
        uint32_t time_raw = atoi(token);
        if(time_raw){

            LOG_INF(TAG, "Time now is: string - %s, int - %d", token, time_raw);
        }
    }

    token = strsep(&_raw_data,","); //3, position status
    bool is_pos_valid = *token == 'A';

    LOG_INF(TAG, "Position status is %s", is_pos_valid?"Valid":"Invalid");


    token = strsep(&_raw_data,","); //4, Latitude
    if(token && strlen(token)){
        float raw_lat = strtof(token, &check);
        if(*check == 0){
            LOG_INF(TAG, "Lat is: string - %s, float - %f", token, raw_lat);
        }
    }
    token = strsep(&_raw_data,","); //5, Latitude direction (N/S)

    token = strsep(&_raw_data,","); //6, Longitude
    if(token && strlen(token)){
        float raw_lon = strtof(token, &check);
        if(*check == 0){
            LOG_INF(TAG, "Lon is: string - %s, float - %f", token, raw_lon);
        }
    }
    token = strsep(&_raw_data,","); //7, Latitude direction (E/W)
    token = strsep(&_raw_data,","); //8, Speed over ground
    token = strsep(&_raw_data,","); //9, Track made good

    token = strsep(&_raw_data,","); //10, Date

    if(token && strlen(token)){
        uint32_t data_raw = atoi(token);
        if(data_raw){
            LOG_INF(TAG, "Date now is: string - %s, int - %d", token, data_raw);
        }
    }

    token = strsep(&_raw_data,","); //11, Magnetic variation, degrees
    token = strsep(&_raw_data,","); //12, Magnetic variation direction E/W

    token = strsep(&_raw_data,"*"); //13, Positioning system mode indicator

    token = strsep(&_raw_data,"\r"); //14, Checksum
    if(token && strlen(token)) {
        uint32_t checksum = strtol(token, &check, 16);
        if (*check == 0) {
            LOG_INF(TAG, "checksum is: string - %s, hex - 0x%X", token, checksum);
        }
    }
}

int32_t gps_gy_create_driver(v_serial_t* _serial){
    gps_gy_driver_impl_t* this = &g_instance;

    this->m_modem = modem_create_default(1024, _serial);

    if(!this->m_modem){
        LOG_ERR(TAG, "Create modem FAILED");
        return -1;
    }

    _serial->flush(_serial, SERIAL_TX_RX);

    memset(this->m_buffer, 0, GPS_BUFFER_LENGTH);
    return 0;
}

int32_t gps_gy_process(){
    gps_gy_driver_impl_t* this = &g_instance;

    memset(this->m_buffer, 0, GPS_BUFFER_LENGTH);
    int len = modem_read_until_string(this->m_modem, "\r\n", this->m_buffer, GPS_BUFFER_LENGTH, 1000);
    if(len > 0){
        char* p_GPRMC = strstr(this->m_buffer, NMEA[GPRMC]);
        if(p_GPRMC){
            LOG_INF(TAG, "1, Find GPRMC packet: %s", p_GPRMC);
            GPRMC_decode(this, p_GPRMC);
        }

//        char* p_GPVTG = strstr(this->m_buffer, NMEA[GPVTG]);
//        if(p_GPVTG){
//            LOG_INF(TAG, "2, Find GPVTG packet: %s", p_GPVTG);
//        }
//
//        char* p_GPGGA = strstr(this->m_buffer, NMEA[GPGGA]);
//        if(p_GPGGA){
//            LOG_INF(TAG, "3, Find GPGGA packet: %s", p_GPGGA);
//        }
//
//        char* p_GPGSA = strstr(this->m_buffer, NMEA[GPGSA]);
//        if(p_GPGSA){
//            LOG_INF(TAG, "4, Find GPGSA packet: %s", p_GPGSA);
//        }
//
//        char* p_GPGSV = strstr(this->m_buffer, NMEA[GPGSV]);
//        if(p_GPGSV){
//            LOG_INF(TAG, "5, Find GPGSV packet: %s", p_GPGSV);
//        }
//
//        char* p_GPGLL= strstr(this->m_buffer, NMEA[GPGLL]);
//        if(p_GPGLL){
//            LOG_INF(TAG, "6, Find GPGLL packet: %s", p_GPGLL);
//        }
    }

    return 0;
}