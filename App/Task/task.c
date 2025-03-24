#include "Task/task.h"
#include "EPD/epd_api.h"
#include "EPD/epd_driver.h"
#include "Graphics/GUI_Paint.h"
#include "Graphics/icon.h"
#include "stm32l0xx_ll_utils.h"

#include <stdio.h>

#define GUI_GRAPH_BLOCK_WIDTH   (24)

void Task_Init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /** Debugging */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIOB->BRR = LL_GPIO_PIN_2;

    /** SHTC3 init */


    /** EPD init */
    if (GPIOA->IDR & LL_GPIO_PIN_0)
    {
        EPD_Init();
        EPD_Clear();

        LL_mDelay(500);
    }
}

void Task_UpdateMeasurement()
{
    ;
}

void Task_Test()
{
    uint16_t width, height;
    int16_t temperature = 143, humidity = -12;
    int16_t T_max = 25, T_min = -10, H_max = 44, H_min = 7;
    uint8_t * img = NULL;

    GPIOB->BSRR = LL_GPIO_PIN_2;

    img = EPD_GetVRAM();

    /** Paint black part */
    Paint_NewImage(img, EPD_WIDTH, EPD_HEIGHT, 90, WHITE);
    Paint_SelectImage(img);
    Paint_Clear(WHITE);

    Paint_DrawRectangle(0, 66, 23, 121, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(24, 115, 225, 121, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(24, 66, 225, 66, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(24, 76, 225, 76, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(24, 86, 225, 86, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(24, 96, 225, 96, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(24, 106, 225, 106, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(44, 66, 44, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(64, 66, 64, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(84, 66, 84, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(104, 66, 104, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(124, 66, 124, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(144, 66, 144, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(164, 66, 164, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(184, 66, 184, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(204, 66, 204, 114, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    width = Paint_DrawAFBNumber(0, 0, temperature, BLACK, WHITE);
    Paint_DrawIcon(width, 0, &(icon_table[ICON_DEGC]), BLACK, WHITE);
    width = Paint_FindNumberWidth(T_max, &Font12);
    Paint_DrawNum(GUI_GRAPH_BLOCK_WIDTH - width, 66, T_max, &Font12, WHITE, BLACK);
    width = Paint_FindNumberWidth(T_min, &Font12);
    Paint_DrawNum(GUI_GRAPH_BLOCK_WIDTH - width, EPD_WIDTH - Font12.Height - 7, T_min, &Font12, WHITE, BLACK);
    
    if (GPIOA->IDR & LL_GPIO_PIN_0)
    {
        EPD_UpdateBlack(img);
    }

    /** Paint red part */
    Paint_Clear(WHITE);

    Paint_DrawRectangle(226, 66, 249, 121, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawNum(226, 66, H_max, &Font12, WHITE, RED);
    Paint_DrawNum(226, EPD_WIDTH - Font12.Height, H_min, &Font12, WHITE, RED);
    width = icon_table[ICON_PRH].width;
    height = icon_table[ICON_PRH].height;
    width += Paint_FindNumberWidth(humidity, NULL);
    width = Paint_DrawIcon(EPD_HEIGHT - width, EPD_WIDTH - height - 7, &(icon_table[ICON_PRH]), RED, WHITE);
    Paint_DrawAFBNumber(width, 0, humidity, RED, WHITE);
    Paint_DrawIcon(EPD_HEIGHT/2 - icon_table[ICON_NOBAT].width/2, 66 + (56-icon_table[ICON_NOBAT].height)/2, &(icon_table[ICON_NOBAT]), RED, WHITE);
    
    if (GPIOA->IDR & LL_GPIO_PIN_0)
    {
        EPD_UpdateRed(img);
    }

    GPIOB->BRR = LL_GPIO_PIN_2;

    /** Update display */
    if (GPIOA->IDR & LL_GPIO_PIN_0)
    {
        EPD_TurnOnDisplay();
    }
}

void Task_PrepareForSleep()
{
    /** EPD Power down */
    EPD_SetPower(0);

    /** SHTC3 in sleep mode */
    ;

    // __WFI();
}
