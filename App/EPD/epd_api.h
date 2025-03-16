#pragma once

#include "stdint.h"

void EPD_SetPower(uint8_t onoff);

void EPD_TurnOnDisplay(void);

void EPD_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void EPD_SetCursor(uint16_t Xstart, uint16_t Ystart);

void EPD_Init(void);
void EPD_Clear(void);
void EPD_UpdateBlack(const uint8_t *blackImage);
void EPD_UpdateRed(const uint8_t *redImage);
void EPD_UpdateAll(const uint8_t *blackImage, const uint8_t *redImage);
void EPD_Sleep(void);
