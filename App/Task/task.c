#include "Task/task.h"
#include "EPD/epd_api.h"
#include "EPD/epd_driver.h"
#include "Graphics/GUI_Paint.h"
#include "Graphics/icon.h"
#include "SHTC3/SHTC3_api.h"
#include "stm32l0xx_ll_utils.h"

#include <stdio.h>

#define GUI_UPPER_YBEGIN        (0)
#define GUI_UPPER_HEIGHT        (64)
#define GUI_UPPER_YEND          (GUI_UPPER_YBEGIN + GUI_UPPER_HEIGHT)
#define GUI_LOWER_YBEGIN        (GUI_UPPER_HEIGHT + 2)
#define GUI_LOWER_HEIGHT        (EPD_WIDTH - GUI_LOWER_YBEGIN)
#define GUI_LOWER_YEND          (GUI_LOWER_YBEGIN + GUI_LOWER_HEIGHT)

#define GUI_YAXIS_WIDTH         (24)
#define GUI_LYAXIS_XBEGIN       (0)
#define GUI_LYAXIS_XEND         (GUI_LYAXIS_XBEGIN + GUI_YAXIS_WIDTH)

#define GUI_RYAXIS_XBEGIN       (EPD_HEIGHT - GUI_YAXIS_WIDTH)
#define GUI_RYAXIS_XEND         (EPD_HEIGHT)

#define GUI_XAXIS_HEIGHT        (8)
#define GUI_XAXIS_YBEGIN        (EPD_WIDTH - GUI_XAXIS_HEIGHT)
#define GUI_XAXIS_YEND          (GUI_XAXIS_YBEGIN + GUI_XAXIS_HEIGHT)

#define GUI_PLOT_XBEGIN         (GUI_LYAXIS_XBEGIN + GUI_YAXIS_WIDTH)
#define GUI_PLOT_WIDTH          (EPD_HEIGHT - 2*GUI_YAXIS_WIDTH)
#define GUI_PLOT_XEND           (GUI_PLOT_XBEGIN + GUI_PLOT_WIDTH - 1)
#define GUI_PLOT_AREA           (200)
#define GUI_PLOT_GAP            (GUI_PLOT_AREA / STORAGE_SIZE)
#define GUI_PLOT_YBEGIN         (GUI_LOWER_YBEGIN + 2)
#define GUI_PLOT_HEIGHT         (GUI_LOWER_HEIGHT - GUI_XAXIS_HEIGHT - 4)

void Task_Init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint16_t shtc3_id = 0;
    float temp, humi;

    /** Debugging */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIOB->BRR = LL_GPIO_PIN_2;

    GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIOA->BSRR = LL_GPIO_PIN_1;

    /** SHTC3 init */
    SHTC3_Init();

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
    int16_t temperature = -143, humidity = 370;
    int16_t T_max = 25, T_min = -10, H_max = 44, H_min = 7;
    float temp, humi;
    uint8_t * img = NULL;

    SHTC3_WakeUp();
    SHTC3_GetTempAndHumi(&temp, &humi);
    SHTC3_Sleep();
    temperature = (int16_t)(temp*10);
    humidity = (int16_t)(humi)*10;

    GPIOB->BSRR = LL_GPIO_PIN_2;

    img = EPD_GetVRAM();

    /** Paint black part */
    Paint_NewImage(img, EPD_WIDTH, EPD_HEIGHT, 90, WHITE);
    Paint_SelectImage(img);
    Paint_Clear(WHITE);

    Paint_DrawRectangle(GUI_LYAXIS_XBEGIN, GUI_LOWER_YBEGIN, GUI_LYAXIS_XEND, GUI_LOWER_YEND, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(GUI_PLOT_XBEGIN, GUI_XAXIS_YBEGIN, GUI_PLOT_XEND, GUI_XAXIS_YEND, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
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
    Paint_DrawNum(GUI_YAXIS_WIDTH - width, GUI_LOWER_YBEGIN, T_max, &Font12, WHITE, BLACK);
    width = Paint_FindNumberWidth(T_min, &Font12);
    Paint_DrawNum(GUI_YAXIS_WIDTH - width, GUI_LOWER_YEND+1 - GUI_XAXIS_HEIGHT - Font12.Height, T_min, &Font12, WHITE, BLACK);
    
    ///< Draw graph
    // for (i = 0; i < STORAGE_SIZE; ++i)
    // {
    //     width = GUI_PLOT_XBEGIN + GUI_PLOT_GAP * i;
    //     height = (uint16_t)((T_num - temp_array[i]*GUI_PLOT_HEIGHT) / (float)(T_max - T_min));
    //     Paint_DrawPoint(width, height, BLACK, DOT_PIXEL_2X2, DOT_PIXEL_DFT);
    // }

    Paint_DrawRectangle(
        EPD_HEIGHT/2 - icon_table[ICON_NOBAT].width/2,
        GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-icon_table[ICON_NOBAT].height)/2 - 4,
        EPD_HEIGHT/2 + icon_table[ICON_NOBAT].width/2,
        GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-icon_table[ICON_NOBAT].height)/2 + icon_table[ICON_NOBAT].height - 4,
        WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    if (GPIOA->IDR & LL_GPIO_PIN_0)
    {
        EPD_UpdateBlack(img);
    }

    /** Paint red part */
    Paint_Clear(WHITE);

    Paint_DrawRectangle(GUI_RYAXIS_XBEGIN, GUI_LOWER_YBEGIN, GUI_RYAXIS_XEND, GUI_LOWER_YEND, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawNum(GUI_RYAXIS_XBEGIN, GUI_LOWER_YBEGIN, H_max, &Font12, WHITE, RED);
    Paint_DrawNum(GUI_RYAXIS_XBEGIN, GUI_XAXIS_YEND+1 - GUI_XAXIS_HEIGHT - Font12.Height, H_min, &Font12, WHITE, RED);
    width = icon_table[ICON_PRH].width;
    height = icon_table[ICON_PRH].height;
    width += Paint_FindAFBNumberWidth(humidity);
    width = Paint_DrawIcon(EPD_HEIGHT - width, GUI_UPPER_YBEGIN + GUI_UPPER_HEIGHT - height, &(icon_table[ICON_PRH]), RED, WHITE);
    Paint_DrawAFBNumber(width, 0, humidity, RED, WHITE);
    
    ///< Draw graph
    // for (i = 0; i < STORAGE_SIZE; ++i)
    // {
    //     width = GUI_PLOT_XBEGIN + GUI_PLOT_GAP * i;
    //     height = (uint16_t)((H_num - humi_array[i]*GUI_PLOT_HEIGHT) / (float)(H_max - H_min));
    //     Paint_DrawPoint(width, height, RED, DOT_PIXEL_1X1, DOT_PIXEL_DFT);
    // }

    Paint_DrawRectangle(
        EPD_HEIGHT/2 - icon_table[ICON_NOBAT].width/2,
        GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-icon_table[ICON_NOBAT].height)/2 - 4,
        EPD_HEIGHT/2 + icon_table[ICON_NOBAT].width/2,
        GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-icon_table[ICON_NOBAT].height)/2 + icon_table[ICON_NOBAT].height - 4,
        WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawIcon(
        EPD_HEIGHT/2 - icon_table[ICON_NOBAT].width/2,
        GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-icon_table[ICON_NOBAT].height)/2 - 4,
        &(icon_table[ICON_NOBAT]), RED, WHITE);

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
