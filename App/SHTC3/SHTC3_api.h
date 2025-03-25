#pragma once

#include "SHTC3/SHTC3_driver.h"

#define SHTC3_ADDR  (0x70)

typedef enum
{
    READ_ID             = 0xEFC8, // command: read ID register
    SOFT_RESET          = 0x805D, // soft reset
    SLEEP               = 0xB098, // sleep
    WAKEUP              = 0x3517, // wakeup
    MEAS_T_RH_POLLING   = 0x7866, // meas. read T first, clock stretching disabled
    MEAS_T_RH_CLOCKSTR  = 0x7CA2, // meas. read T first, clock stretching enabled
    MEAS_RH_T_POLLING   = 0x58E0, // meas. read RH first, clock stretching disabled
    MEAS_RH_T_CLOCKSTR  = 0x5C24  // meas. read RH first, clock stretching enabled
} shtc3_cmd_e;

typedef enum
{
    SHTC3_OK            = 0,
    SHTC3_CRC_ERR       = 1,
    SHTC3_COMM_ERR      = 2
} shtc3_ret_e;

void SHTC3_Init();
shtc3_ret_e SHTC3_WriteCommand(uint16_t cmd);
shtc3_ret_e SHTC3_Read2BytesAndCRC(uint16_t * pdata);
shtc3_ret_e SHTC3_CheckCRC(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
shtc3_ret_e SHTC3_Sleep();
shtc3_ret_e SHTC3_WakeUp();
shtc3_ret_e SHTC3_GetID(uint16_t * pid);
shtc3_ret_e SHTC3_SoftReset();
shtc3_ret_e SHTC3_GetTempAndHumi(float *temp, float *humi);
shtc3_ret_e SHTC3_GetTempAndHumiPolling(float *temp, float *humi);

void SHTC3_Measure(uint8_t times);

const int16_t * GetTemperatureArray();
const uint8_t * GetHumidityArray();
