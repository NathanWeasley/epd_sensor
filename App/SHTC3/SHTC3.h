#pragma once

#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_bus.h"

#define SHTC3_SCL_Pin LL_GPIO_PIN_6
#define SHTC3_SDA_Pin LL_GPIO_PIN_7
#define SHTC3_GPIO_Port GPIOB

#define SHTC3_ADDR      (0x70)

void IIC_Init();

uint8_t IIC_SendByte(uint8_t byte);
uint8_t IIC_ReadByte(uint8_t ack);

void SHTC3_Init();
