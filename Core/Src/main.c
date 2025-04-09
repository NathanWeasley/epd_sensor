/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// static uint8_t count = 0;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriority(SysTick_IRQn, 3);

  SystemClock_Config();

  MX_RTC_Init();

  while (1)
  {
    MX_GPIO_Init();
    //MX_ADC_Init();
    MX_SPI1_Init();

    Task_Init();

    GPIOA->BSRR = LL_GPIO_PIN_15;
    LL_mDelay(1000);
    GPIOA->BRR = LL_GPIO_PIN_15;

    Task_UpdateMeasurement();
    Task_Display();
    Task_PrepareForSleep();

    LPM_StopUntilRTC();

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    NVIC_SetPriority(SysTick_IRQn, 3);
    SystemClock_Config();

    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    // GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
    // GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    // GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    // GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    // GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    // LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // if (count == 0)
    // {
    //   GPIOA->BSRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BSRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BRR = LL_GPIO_PIN_15;

    //   count++;
    // }
    // else
    // {
    //   GPIOA->BSRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BSRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BSRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BSRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    //   GPIOA->BRR = LL_GPIO_PIN_15;
    //   LL_mDelay(100);
    // }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() != 0)
  {
  }
  // LL_RCC_HSI_EnableDivider();
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_PWR_EnableBkUpAccess();
  if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
  }
  LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
  LL_RCC_LSE_Enable();

   /* Wait till LSE is ready */
  while(LL_RCC_LSE_IsReady() != 1)
  {

  }
  if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE)
  {
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
  }
  LL_RCC_EnableRTC();
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }

  LL_Init1msTick(8000000);

  LL_SetSystemCoreClock(8000000);
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  GPIO_InitStruct.Pin = VBAT_IN_Pin | VBUS_IN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(VBAT_IN_GPIO_Port, &GPIO_InitStruct);

  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_1);
  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_2);
  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_17);

  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
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

  /* Enable ADC internal voltage regulator */
  LL_ADC_EnableInternalRegulator(ADC1);
  /* Delay for ADC internal voltage regulator stabilization. */
  /* Compute number of CPU cycles to wait for, from delay in us. */
  /* Note: Variable divided by 2 to compensate partially */
  /* CPU processing cycles (depends on compilation optimization). */
  /* Note: If system core clock frequency is below 200kHz, wait time */
  /* is only a few CPU processing cycles. */
  uint32_t wait_loop_index;
  wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
  while (wait_loop_index != 0)
  {
    wait_loop_index--;
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{
  LL_RTC_InitTypeDef RTC_InitStruct = {0};

  /* Peripheral clock enable */
  // LL_RCC_EnableRTC();
  // LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

  /* RTC interrupt Init */
  NVIC_SetPriority(RTC_IRQn, 0);
  NVIC_EnableIRQ(RTC_IRQn);

  /** Initialize RTC and set the Time and Date
  */
  RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
  RTC_InitStruct.AsynchPrescaler = 127;
  RTC_InitStruct.SynchPrescaler = 255;
  LL_RTC_Init(RTC, &RTC_InitStruct);

  /** Disable WP */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /** Disable Wakeup timer and wait for clock sync */
  RTC->CR &=~ RTC_CR_WUTE;
  while ((RTC->ISR & RTC_ISR_WUTWF) != RTC_ISR_WUTWF);

  /** Clear WUTF flag */
  LL_RTC_ClearFlag_WUT(RTC);

  /** Select wakeup clock source */
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

  /** Set auto reload value */
  LL_RTC_WAKEUP_SetAutoReload(RTC, 60 - 1);

  /** Re-enable Wakeup timer and enable interrupt */
  RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

  /** Enable WP */
  RTC->WPR = 0xFE;
  RTC->WPR = 0x64;

  /** Enable RTC wakeup interrupt through EXTI */
  EXTI->IMR |= EXTI_IMR_IM20;
  EXTI->RTSR |= EXTI_RTSR_RT20;
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = EPD_CLK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(EPD_CLK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = EPD_DIN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(EPD_DIN_GPIO_Port, &GPIO_InitStruct);

  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_Enable(SPI1);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  // LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

  /* EPD nRST */
  LL_GPIO_SetOutputPin(EPD_RST_GPIO_Port, EPD_RST_Pin);

  /* EPD nCS Pin */
  LL_GPIO_SetOutputPin(EPD_CS_GPIO_Port, EPD_CS_Pin);

  /* EPD DC# Pin */
  LL_GPIO_SetOutputPin(EPD_DC_GPIO_Port, EPD_DC_Pin);

  LL_GPIO_SetOutputPin(EPD_PWR_GPIO_Port, EPD_PWR_Pin);
  LL_GPIO_ResetOutputPin(EPD_DEBUG_GPIO_Port, EPD_DEBUG_Pin);

  /**/
  GPIO_InitStruct.Pin = EPD_PWR_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(EPD_PWR_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = EPD_RST_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(EPD_RST_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = EPD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(EPD_CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = EPD_DC_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(EPD_DC_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = EPD_BUSY_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(EPD_BUSY_GPIO_Port, &GPIO_InitStruct);

  /** Init EPD enable debug pin */
  GPIO_InitStruct.Pin = EPD_DEBUG_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Pull = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(EPD_DEBUG_GPIO_Port, &GPIO_InitStruct);

  // /**/
  // LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE8);

  // /**/
  // LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE9);

  // /**/
  // LL_GPIO_SetPinPull(KEY_B_GPIO_Port, KEY_B_Pin, LL_GPIO_PULL_NO);

  // /**/
  // LL_GPIO_SetPinPull(KEY_A_GPIO_Port, KEY_A_Pin, LL_GPIO_PULL_NO);

  // /**/
  // LL_GPIO_SetPinMode(KEY_B_GPIO_Port, KEY_B_Pin, LL_GPIO_MODE_INPUT);

  // /**/
  // LL_GPIO_SetPinMode(KEY_A_GPIO_Port, KEY_A_Pin, LL_GPIO_MODE_INPUT);

  // /**/
  // EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_8;
  // EXTI_InitStruct.LineCommand = ENABLE;
  // EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  // EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  // LL_EXTI_Init(&EXTI_InitStruct);

  // /**/
  // EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_9;
  // EXTI_InitStruct.LineCommand = ENABLE;
  // EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  // EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  // LL_EXTI_Init(&EXTI_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
