#include "Battery/battery.h"
#include "main.h"

#define BATTERY_ADC_TIMEOUT       (100000U)
#define BATTERY_ADC_SAMPLES       (4U)
#define BATTERY_DIVIDER_TOP       (33U)
#define BATTERY_DIVIDER_BOTTOM    (10U)

static uint8_t Battery_ReadAdc(uint16_t *sample)
{
    uint32_t timeout = BATTERY_ADC_TIMEOUT;

    LL_ADC_ClearFlag_EOC(ADC1);
    LL_ADC_ClearFlag_EOS(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1) && --timeout != 0U)
    {
    }
    if (timeout == 0U)
        return 0U;

    *sample = LL_ADC_REG_ReadConversionData12(ADC1);
    return 1U;
}

uint8_t Battery_ReadMillivolts(uint16_t *millivolts)
{
    LL_GPIO_InitTypeDef gpio = {0};
    uint32_t timeout, battery_sum = 0U, vdda_mv, battery_mv;
    uint16_t sample, vref_sample = 0U;
    uint8_t valid = 0U;

    if (millivolts == 0)
        return 0U;

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    /* PA1 sits near VBAT while Q4 is off. Turn on the divider before
       connecting PA1 to the ADC's analog input. */
    GPIOA->BRR = VBAT_EN_PIN;
    gpio.Pin = VBAT_EN_PIN;
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(VIN_EN_PORT, &gpio);
    GPIOA->BSRR = VBAT_EN_PIN;

    /* R29 || R19 and C46 settle in about 0.77 ms; allow over 5 time constants. */
    LL_mDelay(5U);
    gpio.Pin = VBAT_AIN_PIN;
    gpio.Mode = LL_GPIO_MODE_ANALOG;
    LL_GPIO_Init(VIN_PORT, &gpio);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_ADC1);

    LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_LOW);
    LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV4);
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_160CYCLES_5);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT);
    LL_SYSCFG_VREFINT_EnableADC();
    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_1);
    LL_ADC_EnableInternalRegulator(ADC1);
    LL_mDelay(4U); /* VREFINT startup and ADC regulator stabilization. */

    LL_ADC_StartCalibration(ADC1);
    timeout = BATTERY_ADC_TIMEOUT;
    while (LL_ADC_IsCalibrationOnGoing(ADC1) && --timeout != 0U)
    {
    }
    if (timeout == 0U)
        goto cleanup;

    LL_mDelay(1U); /* Calibration-to-enable delay. */
    LL_ADC_ClearFlag_ADRDY(ADC1);
    LL_ADC_Enable(ADC1);
    timeout = BATTERY_ADC_TIMEOUT;
    while (!LL_ADC_IsActiveFlag_ADRDY(ADC1) && --timeout != 0U)
    {
    }
    if (timeout == 0U)
        goto cleanup;

    for (uint8_t i = 0U; i < BATTERY_ADC_SAMPLES; ++i)
    {
        if (!Battery_ReadAdc(&sample))
            goto cleanup;
        battery_sum += sample;
    }

    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_VREFINT);
    if (!Battery_ReadAdc(&vref_sample) || vref_sample == 0U)
        goto cleanup;

    vdda_mv = __LL_ADC_CALC_VREFANALOG_VOLTAGE(vref_sample, LL_ADC_RESOLUTION_12B);
    /* PA1 is a standard ADC channel, specified from VDDA = 1.75 V. */
    if (vdda_mv < 1750U || vdda_mv > 2000U)
        goto cleanup;

    /* ADC counts * VDDA * (R29 + R19) / (4095 * R19). */
    battery_mv = ((battery_sum / BATTERY_ADC_SAMPLES) * vdda_mv *
                  (BATTERY_DIVIDER_TOP + BATTERY_DIVIDER_BOTTOM) +
                  (4095U * BATTERY_DIVIDER_BOTTOM / 2U)) /
                 (4095U * BATTERY_DIVIDER_BOTTOM);
    if (battery_mv < 1800U || battery_mv > 4500U)
        goto cleanup;

    *millivolts = (uint16_t)battery_mv;
    valid = 1U;

cleanup:
    /* Reset disconnects PA1 from the ADC before the divider is switched off. */
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_ADC1);
    LL_SYSCFG_VREFINT_DisableADC();
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);
    GPIOA->BRR = VBAT_EN_PIN;
    return valid;
}
