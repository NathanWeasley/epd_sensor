#pragma once

#include <stdint.h>

void Task_Init();

void Task_UpdateMeasurement();
void Task_UpdateBattery();


void Task_UpdateDisplay();
void Task_Display();

void Task_PrepareForSleep();
void LPM_StopUntilEvent();
