#include "Task/task.h"
#include "EPD/epd_api.h"
#include "EPD/epd_driver.h"
#include "Graphics/GUI_Paint.h"
#include "stm32l0xx_ll_utils.h"

#include <stdio.h>

void task_sensor()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint8_t * img = NULL;
    img = EPD_GetVRAM();

    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIOB->BRR = LL_GPIO_PIN_2;

    GPIOB->BSRR = LL_GPIO_PIN_2;
    EPD_Init();
    GPIOB->BRR = LL_GPIO_PIN_2;
    GPIOB->BSRR = LL_GPIO_PIN_2;
    EPD_Clear();
    GPIOB->BRR = LL_GPIO_PIN_2;

    LL_mDelay(500);

    GPIOB->BSRR = LL_GPIO_PIN_2;

    /** Paint black part */
    Paint_NewImage(img, EPD_WIDTH, EPD_HEIGHT, 90, WHITE);
    Paint_SelectImage(img);
    Paint_Clear(WHITE);
    Paint_DrawPoint(5, 70, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 80, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawLine(20, 70, 50, 100, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(50, 70, 20, 100, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(60, 70, 90, 100, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(125, 85, 15, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    EPD_UpdateBlack(img);

    GPIOB->BRR = LL_GPIO_PIN_2;
    GPIOB->BSRR = LL_GPIO_PIN_2;

    /** Paint red part */
    Paint_Clear(WHITE);
    Paint_DrawPoint(5, 90, RED, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 100, RED, DOT_PIXEL_4X4, DOT_STYLE_DFT);
    Paint_DrawLine(125, 70, 125, 100, RED, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(110, 85, 140, 85, RED, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawRectangle(20, 70, 50, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(165, 85, 15, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(5, 0, "waveshare Electronics", &Font12, BLACK, WHITE);
    Paint_DrawNum(5, 50, 987654321, &Font16, WHITE, RED);
    EPD_UpdateRed(img);

    GPIOB->BRR = LL_GPIO_PIN_2;

    /** Update display */
    EPD_TurnOnDisplay();
}
