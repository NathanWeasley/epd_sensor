#pragma once

#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_bus.h"

#define SHTC3_MAX_DATA_RECORD_LEN   (100)

typedef enum
{
    ACK = 0,
    NACK = 1
} iic_ack_e;

void IIC_Init();

void IIC_Start();
void IIC_Stop();
uint8_t IIC_SendByte(uint8_t byte);
uint8_t IIC_ReadByte(uint8_t ack);
