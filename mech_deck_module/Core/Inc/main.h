/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l4xx_ll_iwdg.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx.h"
#include "stm32l4xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define DISP_BG_TIM TIM3
#define DISP_BG_TIM_SETPULSE LL_TIM_OC_SetCompareCH1
#define DISP_BG_TIM_CHANNEL LL_TIM_CHANNEL_CH1

#define NRF_SPI SPI2

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void radioInit(void);
void radioLoop(void);

//delay in milliseconds
inline void msDelay(int delay) {
    LL_mDelay(delay / 5); //we have 200Hz system timer
}

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin LL_GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define DISP_D0_Pin LL_GPIO_PIN_0
#define DISP_D0_GPIO_Port GPIOC
#define DISP_D1_Pin LL_GPIO_PIN_1
#define DISP_D1_GPIO_Port GPIOC
#define DISP_D2_Pin LL_GPIO_PIN_2
#define DISP_D2_GPIO_Port GPIOC
#define DISP_D3_Pin LL_GPIO_PIN_3
#define DISP_D3_GPIO_Port GPIOC
#define KEY_L_NAVI_Pin LL_GPIO_PIN_0
#define KEY_L_NAVI_GPIO_Port GPIOA
#define KEY_L_ANCHOR_Pin LL_GPIO_PIN_1
#define KEY_L_ANCHOR_GPIO_Port GPIOA
#define USART_TX_Pin LL_GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin LL_GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin LL_GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define DISP_D4_Pin LL_GPIO_PIN_4
#define DISP_D4_GPIO_Port GPIOC
#define DISP_D5_Pin LL_GPIO_PIN_5
#define DISP_D5_GPIO_Port GPIOC
#define NRF_IRQ_Pin LL_GPIO_PIN_10
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_CE_Pin LL_GPIO_PIN_11
#define NRF_CE_GPIO_Port GPIOB
#define NRF_CSN_Pin LL_GPIO_PIN_12
#define NRF_CSN_GPIO_Port GPIOB
#define DISP_D6_Pin LL_GPIO_PIN_6
#define DISP_D6_GPIO_Port GPIOC
#define DISP_D7_Pin LL_GPIO_PIN_7
#define DISP_D7_GPIO_Port GPIOC
#define DISP_RESET_Pin LL_GPIO_PIN_8
#define DISP_RESET_GPIO_Port GPIOA
#define DISP_CD_Pin LL_GPIO_PIN_9
#define DISP_CD_GPIO_Port GPIOA
#define DISP_RD_Pin LL_GPIO_PIN_10
#define DISP_RD_GPIO_Port GPIOA
#define DISP_WR_Pin LL_GPIO_PIN_11
#define DISP_WR_GPIO_Port GPIOA
#define TMS_Pin LL_GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin LL_GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define KEY_CANCEL_Pin LL_GPIO_PIN_2
#define KEY_CANCEL_GPIO_Port GPIOD
#define KEY_UP_Pin LL_GPIO_PIN_3
#define KEY_UP_GPIO_Port GPIOB
#define KEY_DOWN_Pin LL_GPIO_PIN_4
#define KEY_DOWN_GPIO_Port GPIOB
#define KEY_OK_Pin LL_GPIO_PIN_5
#define KEY_OK_GPIO_Port GPIOB
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
void initLcd(void);
void splashLcd(void);

void updateLcd(uint_fast64_t timestamp);
void outputNmea(void);

extern volatile uint_fast64_t _sysTicks;
static inline void _sysTimerRoutine() {
    _sysTicks++;
}
static inline uint_fast64_t sysTicks() {
    __disable_irq();
    uint_fast64_t result = _sysTicks;
    __enable_irq();
    return result;
}

void updateKeyboard(uint_fast64_t timeStamp);

void mainLoop()__attribute__((noreturn));


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
