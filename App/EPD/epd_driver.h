#pragma once

#include "Utils/utils.h"

/** Below connected to GPIOB */
#define EPD_PWR_PIN         LL_GPIO_PIN_1
#define EPD_PWR_PORT        GPIOB
#define EPD_BUSY_PIN        LL_GPIO_PIN_0
#define EPD_BUSY_PORT       GPIOB

/** Below connected to GPIOA */
#define EPD_RST_PIN         LL_GPIO_PIN_3
#define EPD_RST_PORT        GPIOA
#define EPD_DC_PIN          LL_GPIO_PIN_6
#define EPD_DC_PORT         GPIOA
#define EPD_CS_PIN          LL_GPIO_PIN_4
#define EPD_CS_PORT         GPIOA
#define EPD_CLK_PIN         LL_GPIO_PIN_5
#define EPD_DIN_PIN         LL_GPIO_PIN_7
#define EPD_SPI_PORT        GPIOA

#define EPD_WIDTH           (122)
#define EPD_HEIGHT          (250)
#define EPD_WIDTH_BYTE      ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8 ) : (EPD_WIDTH / 8 + 1))
#define EPD_HEIGHT_BYTE     (EPD_HEIGHT)

void EPD_SPI_Init();
void EPD_GPIO_Init();
void EPD_Transmit(uint8_t byte);
void EPD_Delay_ms(uint16_t ms);

void EPD_Reset();
void EPD_WaitForBusy();
void EPD_SendData(uint8_t data);
void EPD_SendCommand(uint8_t cmd);
