#include "SHTC3/SHTC3_api.h"

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

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

shtc3_ret_e SHTC3_StartWrite()
{
    IIC_Start();
    return IIC_SendByte(SHTC3_ADDR << 1);
}

shtc3_ret_e SHTC3_StartRead()
{
    IIC_Start();
    return IIC_SendByte((SHTC3_ADDR << 1) | 0x01);
}

void SHTC3_Stop()
{
    IIC_Stop();
}

shtc3_ret_e SHTC3_WriteCommand(uint16_t cmd)
{
    shtc3_ret_e ret;

    ret = IIC_SendByte(cmd >> 8);
    ret |= IIC_SendByte(cmd & 0xFF);

    return ret;
}

shtc3_ret_e SHTC3_Read2BytesAndCRC(uint16_t *pdata)
{
    shtc3_ret_e ret;    // error code
    uint8_t bytes[2]; // read data array
    uint8_t checksum; // checksum byte

    // read two data bytes and one checksum byte
    bytes[0] = IIC_ReadByte(ACK);
    bytes[1] = IIC_ReadByte(ACK);
    checksum = IIC_ReadByte(ACK);

    // verify checksum
    ret = SHTC3_CheckCRC(bytes, 2, checksum);

    // combine the two bytes to a 16-bit value
    *pdata = ((uint16_t)(bytes[0] << 8)) | bytes[1];

    return ret;
}

shtc3_ret_e SHTC3_CheckCRC(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
{
    uint8_t bit;        // bit mask
    uint8_t crc = 0xFF; // calculated checksum
    uint8_t byteCtr;    // byte counter

    // calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
    {
        crc ^= (data[byteCtr]);
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }

    // verify checksum
    if (crc != checksum)
    {
        return SHTC3_CRC_ERR;
    }
    else
    {
        return SHTC3_OK;
    }
}

inline float SHTC3_CalcTemperature(uint16_t rawValue)
{
    // calculate temperature [°C]
    // T = -45 + 175 * rawValue / 2^16
    return 175 * (float)rawValue / 65536.0f - 45.0f;
}

inline float SHTC3_CalcHumidity(uint16_t rawValue)
{
    // calculate relative humidity [%RH]
    // RH = rawValue / 2^16 * 100
    return 100 * (float)rawValue / 65536.0f;
}

shtc3_ret_e SHTC3_GetID(uint16_t *pid)
{
    shtc3_ret_e ret = SHTC3_StartWrite();

    // write ID read command
    ret |= SHTC3_WriteCommand(READ_ID);

    // read ID
    if (ret == SHTC3_OK)
    {
        SHTC3_StartRead();
        ret |= SHTC3_Read2BytesAndCRC(pid);
    }

    SHTC3_Stop();

    return ret;
}

shtc3_ret_e SHTC3_Sleep(void)
{
    shtc3_ret_e ret = SHTC3_StartWrite();

    if (ret == SHTC3_OK)
    {
        ret |= SHTC3_WriteCommand(SLEEP);
    }

    SHTC3_Stop();

    return ret;
}

shtc3_ret_e SHTC3_WakeUp(void)
{
    shtc3_ret_e ret = SHTC3_StartWrite();

    if (ret == SHTC3_OK)
    {
        ret |= SHTC3_WriteCommand(WAKEUP);
    }

    SHTC3_Stop();

    LL_mDelay(1); // wait 100 us

    return ret;
}

shtc3_ret_e SHTC3_SoftReset(void)
{
  shtc3_ret_e ret = SHTC3_StartWrite();

  // write reset command
  ret |= SHTC3_WriteCommand(SOFT_RESET);

  SHTC3_Stop();

  return ret;
}

shtc3_ret_e SHTC3_GetTempAndHumi(float *temp, float *humi)
{
    shtc3_ret_e  ret;        // error code
    uint16_t rawValueTemp; // temperature raw value from sensor
    uint16_t rawValueHumi; // humidity raw value from sensor

    ret = SHTC3_StartWrite();

    // measure, read temperature first, clock streching enabled
    ret |= SHTC3_WriteCommand(MEAS_T_RH_CLOCKSTR);

    // if no error, read temperature and humidity raw values
    if (ret == SHTC3_OK)
    {
        ret |= SHTC3_StartRead();
        ret |= SHTC3_Read2BytesAndCRC(&rawValueTemp);
        ret |= SHTC3_Read2BytesAndCRC(&rawValueHumi);
    }

    SHTC3_Stop();

    // if no error, calculate temperature in °C and humidity in %RH
    if (ret == SHTC3_OK)
    {
        *temp = SHTC3_CalcTemperature(rawValueTemp);
        *humi = SHTC3_CalcHumidity(rawValueHumi);
    }

    return ret;
}

shtc3_ret_e SHTC3_GetTempAndHumiPolling(float *temp, float *humi)
{
    shtc3_ret_e  ret;           // error code
    uint8_t  maxPolling = 20; // max. retries to read the measurement (polling)
    uint16_t rawValueTemp;    // temperature raw value from sensor
    uint16_t rawValueHumi;    // humidity raw value from sensor

    ret = SHTC3_StartWriteAccess();

    // measure, read temperature first, clock streching disabled (polling)
    ret |= SHTC3_WriteCommand(MEAS_T_RH_POLLING);

    // if no error, ...
    if (ret == SHTC3_OK)
    {
        // poll every 1ms for measurement ready
        while (maxPolling--)
        {
            // check if the measurement has finished
            ret = SHTC3_StartReadAccess();

            // if measurement has finished -> exit loop
            if (ret == SHTC3_OK)
                break;

            // delay 1ms
            DelayMicroSeconds(1000);
        }

        // if no error, read temperature and humidity raw values
        if (ret == SHTC3_OK)
        {
            ret |= SHTC3_Read2BytesAndCrc(&rawValueTemp);
            ret |= SHTC3_Read2BytesAndCrc(&rawValueHumi);
        }
    }

    SHTC3_StopAccess();

    // if no error, calculate temperature in °C and humidity in %RH
    if (ret == SHTC3_OK)
    {
        *temp = SHTC3_CalcTemperature(rawValueTemp);
        *humi = SHTC3_CalcHumidity(rawValueHumi);
    }

    return ret;
}

const int16_t * GetTemperatureArray()
{
    return tempx10_array;
}

const uint8_t * GetHumidityArray()
{
    return humi_array;
}