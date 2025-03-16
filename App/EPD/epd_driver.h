#pragma once

#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_utils.h"

#define EPD_WIDTH   122
#define EPD_HEIGHT  250

void SPI_Transmit(uint8_t byte);
void EPD_Delay_ms(uint16_t ms);

void EPD_Reset();
void EPD_WaitForBusy();
void EPD_SendData(uint8_t data);
void EPD_SendCommand(uint8_t cmd);




