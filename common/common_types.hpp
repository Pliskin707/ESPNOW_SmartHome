#ifndef __COMMON_TYPES__
#define __COMMON_TYPES__

#include <cmath>

typedef enum e_msgType
{
    unknown = 0,
    temperatureHumidity
} e_msgType;

typedef int16_t float16;

float16 toFloat16 (const float value);
float fromFloat16 (const float16 value);

typedef struct t_tempHumidData
{
    float temperature;
    float humidity;
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

#endif