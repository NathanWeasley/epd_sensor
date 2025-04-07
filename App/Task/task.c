#include "Task/task.h"
#include "EPD/epd_api.h"
#include "EPD/epd_driver.h"
#include "Graphics/GUI_Paint.h"
#include "Graphics/icon.h"
#include "SHTC3/SHTC3_api.h"
#include "Battery/battery.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_gpio.h"

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
#define GUI_PLOT_GAP            (GUI_PLOT_AREA / SHTC3_MAX_DATA_RECORD_LEN)
#define GUI_PLOT_YBEGIN         (GUI_LOWER_YBEGIN + 2)
#define GUI_PLOT_HEIGHT         (GUI_LOWER_HEIGHT - GUI_XAXIS_HEIGHT - 4)

#define SHTC3_TEMP_MIN_GAP      (50)
#define SHTC3_HUMI_MIN_GAP      (10)
#define SHTC3_MAX_DATA_RECORD_LEN   (100)
static int16_t tempx10_array[SHTC3_MAX_DATA_RECORD_LEN] = {0};
static int16_t T_max = INT16_MIN, T_min = INT16_MAX;
static int32_t T_numerator;
static uint8_t humi_array[SHTC3_MAX_DATA_RECORD_LEN] = {0};
static uint8_t H_max = 0, H_min = UINT8_MAX;
static int32_t H_numerator;
uint8_t array_idx = 0;

typedef enum
{
    BATTERY_NORMAL = 0,
    BATTERY_CRITICL = 1,
    BATTERY_CHARGING = 2
} battery_state_e;
battery_state_e battery_flag = BATTERY_NORMAL;
uint16_t vbat_val, vbus_val, vref_val;

void Task_Init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /** Debugging */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15 | LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /** EPD init */
    if (EPD_GetSwitch())
    {
        EPD_Init();
    }
}

void Task_UpdateMeasurement()
{
    float temp, humi;
    int16_t temp_buf, T_delta;
    uint8_t humi_buf, H_delta;

    /** SHTC3 init (only inits IIC emulation GPIO) */
    SHTC3_Init();

    /** Wakep sensor and read */
    SHTC3_WakeUp();
    SHTC3_GetTempAndHumi(&temp, &humi);
    SHTC3_Sleep();

    /** Sensor data processing */
    temp_buf = (int16_t)(temp*10);
    humi_buf = (uint8_t)humi;

    /** Update extrema */
    if (temp_buf > T_max)
        T_max = temp_buf;
    if (temp_buf < T_min)
        T_min = temp_buf;
    if (humi_buf > H_max)
        H_max = humi_buf;
    if (humi_buf < H_min)
        H_min = humi_buf;

    /** Limit extrema */
    if (T_max - T_min < SHTC3_TEMP_MIN_GAP)
    {
        T_delta = T_max - T_min;
        T_max += (SHTC3_TEMP_MIN_GAP - T_delta)/2;
        T_min -= (SHTC3_TEMP_MIN_GAP - T_delta)/2;
    }
    if (H_max - H_min < SHTC3_HUMI_MIN_GAP)
    {
        H_delta = H_max - H_min;
        H_max += (SHTC3_HUMI_MIN_GAP - H_delta)/2;
        H_min -= (SHTC3_HUMI_MIN_GAP - H_delta)/2;
    }

    /** Update precalculated bias and gain for display */
    T_delta = T_max - T_min;
    T_numerator = GUI_PLOT_YBEGIN*T_delta + T_max*GUI_PLOT_HEIGHT;
    H_delta = H_max - H_min;
    H_numerator = GUI_PLOT_YBEGIN*H_delta + H_max*GUI_PLOT_HEIGHT;

    /** Write into array */
    tempx10_array[array_idx] = temp_buf;
    humi_array[array_idx] = humi_buf;
    if (++array_idx == SHTC3_MAX_DATA_RECORD_LEN)
        array_idx = 0;
}

void Task_UpdateBattery()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_ADC_InitTypeDef ADC_InitStruct = {0};

    /** Enable GPIO clock */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /** Init MOSFET switch for voltage divider */
    VIN_EN_PORT->BSRR = VBAT_EN_PIN | VBUS_EN_PIN;
    GPIO_InitStruct.Pin = VBAT_EN_PIN | VBUS_EN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(VIN_EN_PORT, &GPIO_InitStruct);

    /** ADC clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    /** ADC pin config */
    GPIO_InitStruct.Pin = VBAT_AIN_PIN | VBUS_AIN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(VIN_PORT, &GPIO_InitStruct);

    /** ADC Regular Channel */
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_1);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_2);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_17);    ///< VREF_INT

    /** ADC Common config */
    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_1RANK;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
    LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_HIGH);
    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);
    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);
    LL_ADC_Enable(ADC1);

    /** Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);
    /* Compute number of CPU cycles to wait for, from delay in us. */
    /* Note: Variable divided by 2 to compensate partially */
    /* CPU processing cycles (depends on compilation optimization). */
    /**
     * Delay for ADC internal voltage regulator stabilization.
     * Note: If system core clock frequency is below 200kHz, wait time
     * is only a few CPU processing cycles.
     */
    uint32_t wait_loop_index;
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /** Enable voltage divider */
    VIN_EN_PORT->BSRR = VBAT_EN_PIN | VBUS_EN_PIN;
    LL_mDelay(1);       ///< Wait for capacitor to stablize

    /** Start ADC conversion */
    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1));
    vbat_val = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1));
    vbus_val = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1));
    vref_val = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_ClearFlag_EOS(ADC1);

    /** Disable voltage divider */
    VIN_EN_PORT->BRR = VBAT_EN_PIN | VBUS_EN_PIN;

    /** Disable ADC */
    LL_ADC_DeInit(ADC1);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);

    /** Process values */
    ///< TODO
}

void Task_Display()
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

    uint16_t width, height, width_prev, height_prev, delta;
    uint8_t idx = array_idx == 0 ? SHTC3_MAX_DATA_RECORD_LEN - 1 : array_idx - 1;
    int16_t temperature = tempx10_array[idx], humidity = humi_array[idx]*10;
    uint8_t * img = NULL;
    uint8_t i, j;
    const icon_t * picon = NULL;

    /** Preprocess */
    img = EPD_GetVRAM();
    Paint_NewImage(img, EPD_WIDTH, EPD_HEIGHT, 90, WHITE);
    Paint_SelectImage(img);
    if (battery_flag == BATTERY_CRITICL)
    {
        picon = &(icon_table[ICON_NOBAT]);
    }
    else if (battery_flag == BATTERY_CHARGING)
    {
        picon = &(icon_table[ICON_CHRG]);
    }

    /** Paint black part */
    Paint_Clear(WHITE);     ///< TODO: Replace with background pouring
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
    width = Paint_FindNumberWidth(T_max/10, &Font12);
    Paint_DrawNum(GUI_YAXIS_WIDTH - width, GUI_LOWER_YBEGIN, T_max/10, &Font12, WHITE, BLACK);
    width = Paint_FindNumberWidth(T_min/10, &Font12);
    Paint_DrawNum(GUI_YAXIS_WIDTH - width, GUI_LOWER_YEND+1 - GUI_XAXIS_HEIGHT - Font12.Height, T_min/10, &Font12, WHITE, BLACK);
    
    ///< Draw graph
    delta = T_max - T_min;
    width_prev = 0;
    height_prev = (uint16_t)((T_numerator - tempx10_array[array_idx]*GUI_PLOT_HEIGHT) / delta);
    for (j = 1, i = array_idx+1; i < SHTC3_MAX_DATA_RECORD_LEN; ++i, ++j)
    {
        width = GUI_PLOT_XBEGIN + GUI_PLOT_GAP * j;
        height = (uint16_t)((T_numerator - tempx10_array[i]*GUI_PLOT_HEIGHT) / delta);
        Paint_DrawLine(width_prev, height_prev, width, height, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        width_prev = width;
        height_prev = height;
    }
    for (i = 0; i < array_idx; ++i, ++j)
    {
        width = GUI_PLOT_XBEGIN + GUI_PLOT_GAP * j;
        height = (uint16_t)((T_numerator - tempx10_array[i]*GUI_PLOT_HEIGHT) / delta);
        Paint_DrawLine(width_prev, height_prev, width, height, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        width_prev = width;
        height_prev = height;
    }

    if (picon)
    {
        Paint_DrawRectangle(
            EPD_HEIGHT/2 - picon->width/2,
            GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-picon->height)/2 - 4,
            EPD_HEIGHT/2 + picon->width/2,
            GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-picon->height)/2 + picon->height - 4,
            WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }

    if (EPD_GetSwitch())
    {
        /** Init EPD */
        EPD_Init();

        EPD_UpdateBlack(img);
    }

    /** Paint red part */
    Paint_Clear(WHITE);     ///< TODO: Replace with background pouring

    Paint_DrawRectangle(GUI_RYAXIS_XBEGIN, GUI_LOWER_YBEGIN, GUI_RYAXIS_XEND, GUI_LOWER_YEND, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawNum(GUI_RYAXIS_XBEGIN, GUI_LOWER_YBEGIN, H_max, &Font12, WHITE, RED);
    Paint_DrawNum(GUI_RYAXIS_XBEGIN, GUI_XAXIS_YEND+1 - GUI_XAXIS_HEIGHT - Font12.Height, H_min, &Font12, WHITE, RED);
    width = icon_table[ICON_PRH].width;
    height = icon_table[ICON_PRH].height;
    width += Paint_FindAFBNumberWidth(humidity);
    width = Paint_DrawIcon(EPD_HEIGHT - width, GUI_UPPER_YBEGIN + GUI_UPPER_HEIGHT - height, &(icon_table[ICON_PRH]), RED, WHITE);
    Paint_DrawAFBNumber(width, 0, humidity, RED, WHITE);
    
    ///< Draw graph
    delta = H_max - H_min;
    width_prev = 0;
    height_prev = (uint16_t)((H_numerator - humi_array[array_idx]*GUI_PLOT_HEIGHT) / delta);
    for (j = 1, i = array_idx+1; i < SHTC3_MAX_DATA_RECORD_LEN; ++i, ++j)
    {
        width = GUI_PLOT_XBEGIN + GUI_PLOT_GAP * j;
        height = (uint16_t)((H_numerator - humi_array[i]*GUI_PLOT_HEIGHT) / delta);
        Paint_DrawLine(width_prev, height_prev, width, height, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        width_prev = width;
        height_prev = height;
    }
    for (i = 0; i < array_idx; ++i, ++j)
    {
        width = GUI_PLOT_XBEGIN + GUI_PLOT_GAP * j;
        height = (uint16_t)((H_numerator - humi_array[i]*GUI_PLOT_HEIGHT) / delta);
        Paint_DrawLine(width_prev, height_prev, width, height, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        width_prev = width;
        height_prev = height;
    }

    if (picon)
    {
        Paint_DrawRectangle(
            EPD_HEIGHT/2 - picon->width/2,
            GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-picon->height)/2 - 4,
            EPD_HEIGHT/2 + picon->width/2,
            GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-picon->height)/2 + picon->height - 4,
            WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

        Paint_DrawIcon(
            EPD_HEIGHT/2 - picon->width/2,
            GUI_LOWER_YBEGIN + (GUI_LOWER_HEIGHT-picon->height)/2 - 4,
            &(icon_table[ICON_NOBAT]), RED, WHITE);
    }

    if (EPD_GetSwitch())
    {
        EPD_UpdateRed(img);
    }

    /** Update display */
    if (EPD_GetSwitch())
    {
        EPD_Refresh();

        /** Stop SPI */
        EPD_DeInit();

        /** Set falling edge event on EPD_BUSY */
        LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE0);
        LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);
        LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
        EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
        EXTI_InitStruct.LineCommand = ENABLE;
        EXTI_InitStruct.Mode = LL_EXTI_MODE_EVENT;
        EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
        LL_EXTI_Init(&EXTI_InitStruct);

        /** Enter stop mode and wait for wakeup by BUSY pin falling */
        LPM_StopUntilEvent();

        /** Recover MOSFET management pins to shutdown EPD */
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
        LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_LOW);
        LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);
        LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_PUSHPULL);
    }
}

void Task_PrepareForSleep()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /** EPD Power down */
    EPD_SetPower(0);

    /** Unused GPIOs all into analog input mode */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15 | LL_GPIO_PIN_10 | LL_GPIO_PIN_9 | LL_GPIO_PIN_8 |
                          LL_GPIO_PIN_7 | LL_GPIO_PIN_6 | LL_GPIO_PIN_5 | LL_GPIO_PIN_4 |
                          LL_GPIO_PIN_3 | LL_GPIO_PIN_2 | LL_GPIO_PIN_1 | LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 |
                          LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8 |
                          LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 |
                          LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;
    LL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /** MOSFET switches set to close */
    GPIOA->BRR = LL_GPIO_PIN_11;        ///< VBUS voltage divider
    GPIOA->BRR = LL_GPIO_PIN_12;        ///< VBAT voltage divider
    GPIOB->BSRR = LL_GPIO_PIN_1;        ///< EPD power switch (1=off, 0=on)

    /** Stop GPIO clock */
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /** Stop Peripheral clocks */
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
}

void LPM_StopUntilEvent()
{
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_LOW_POWER);
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
    LL_LPM_EnableDeepSleep();
    __WFE();
}
