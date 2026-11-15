#include "main.h"
#include "app.h"

void SystemClock_Config(void);
static void MX_RTC_Init(void);

int main(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriority(SysTick_IRQn, 3);

  SystemClock_Config();

  MX_RTC_Init();

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIOA->BSRR = LL_GPIO_PIN_15;
  LL_mDelay(2000);
  GPIOA->BRR = LL_GPIO_PIN_15;

  // LL_DBGMCU_EnableDBGStopMode();

  while (1)
  {
    /** Read the rested battery voltage before powering the display. */
    Task_UpdateBattery();
    Task_Init();

    Task_UpdateMeasurement();

    /** Prepare and update UI */
    Task_Display();

    /** Shutdown all peripherals and prepare GPIO */
    Task_PrepareForSleep();

    LPM_StopUntilRTC();

    SystemClock_Config();

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIOA->BSRR = LL_GPIO_PIN_15;
    LL_mDelay(100);
    GPIOA->BRR = LL_GPIO_PIN_15;
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while (LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0);

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() != 0);

  LL_RCC_HSI_Enable();
  while (LL_RCC_HSI_IsReady() != 1); /* Wait till HSI is ready */
  LL_RCC_HSI_SetCalibTrimming(16);

  LL_PWR_EnableBkUpAccess();
  if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
  }

  LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
  LL_RCC_LSE_Enable();
   /* Wait till LSE is ready */
  while (LL_RCC_LSE_IsReady() != 1);

  if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE)
  {
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
  }
  LL_RCC_EnableRTC();

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI);

  LL_Init1msTick(8000000);
  LL_SetSystemCoreClock(8000000);
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
  // clock already enabled in systemclock_init()

  /* RTC interrupt Init */
  NVIC_SetPriority(RTC_IRQn, 0);
  NVIC_EnableIRQ(RTC_IRQn);

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
  LL_RTC_WAKEUP_SetAutoReload(RTC, 1800 - 1);

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
