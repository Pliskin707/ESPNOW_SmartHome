#ifndef __COMMON_TYPES__
#define __COMMON_TYPES__

#include <cmath>

typedef enum e_msgType
{
    unknown = 0,
    temperatureHumidity
} e_msgType;

typedef int16_t fixed16_t;
typedef uint8_t mac_t[6];

typedef struct t_tempHumidData
{
    fixed16_t temperature;
    fixed16_t humidity;
} t_tempHumidData;

typedef union u_msgDataUnion
{
    t_tempHumidData tempHumid;
} u_msgDataUnion;

typedef struct __attribute__((packed)) t_msgData
{
    e_msgType type : 8;
    u_msgDataUnion data;
} t_msgData;

fixed16_t toFixed16 (const float value) {return (int16_t) (value * 100.0);}
float fromFixed16 (const fixed16_t value) {return ((float) value) / 100.0;}
String toString (const mac_t mac) 
{
    char buf[18]; // FF:FF:FF:FF:FF:FF
    sprintf_P(buf, PSTR("%02X:%02X:%02X:%02X:%02X"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}

#endif