/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_crc.h"
#include "stm32l0xx_ll_lptim.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BATT_IN_Pin LL_GPIO_PIN_0
#define BATT_IN_GPIO_Port GPIOA
#define EPD_PWR_Pin LL_GPIO_PIN_10
#define EPD_PWR_GPIO_Port GPIOA
#define EPD_RST_Pin LL_GPIO_PIN_3
#define EPD_RST_GPIO_Port GPIOA
#define EPD_CS_Pin LL_GPIO_PIN_4
#define EPD_CS_GPIO_Port GPIOA
#define EPD_CLK_Pin LL_GPIO_PIN_5
#define EPD_CLK_GPIO_Port GPIOA
#define EPD_DC_Pin LL_GPIO_PIN_6
#define EPD_DC_GPIO_Port GPIOA
#define EPD_DIN_Pin LL_GPIO_PIN_7
#define EPD_DIN_GPIO_Port GPIOA
#define EPD_BUSY_Pin LL_GPIO_PIN_0
#define EPD_BUSY_GPIO_Port GPIOB
#define KEY_B_Pin LL_GPIO_PIN_8
#define KEY_B_GPIO_Port GPIOA
#define KEY_A_Pin LL_GPIO_PIN_9
#define KEY_A_GPIO_Port GPIOA
#define SHTC3_SCL_Pin LL_GPIO_PIN_6
#define SHTC3_SCL_GPIO_Port GPIOB
#define SHTC3_SDA_Pin LL_GPIO_PIN_7
#define SHTC3_SDA_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
