#include "SHTC3/SHTC3_api.h"

static int16_t tempx10_array[SHTC3_MAX_DATA_RECORD_LEN];
static int16_t T_max, T_min;
static int32_t T_numerator;
static uint8_t humi_array[SHTC3_MAX_DATA_RECORD_LEN];
static int16_t H_max, H_min;
static int32_t H_numerator;

void SHTC3_Init()
{
    IIC_Init();
}



const int16_t * GetTemperatureArray()
{
    return tempx10_array;
}

const uint8_t * GetHumidityArray()
{
    return humi_array;
}