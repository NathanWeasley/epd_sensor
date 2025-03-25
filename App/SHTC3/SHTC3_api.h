#pragma once

#include "SHTC3/SHTC3_driver.h"

void SHTC3_Init();

void SHTC3_Measure(uint8_t times);

const int16_t * GetTemperatureArray();
const uint8_t * GetHumidityArray();
