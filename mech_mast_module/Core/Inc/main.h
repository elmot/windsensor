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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "../../../sensor-signature.h"
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
void radioCheck(void);
void radioInit(void);
void radioLoop(void);
void resetI2C(void);
void sensorLoop(void);

extern volatile char *reply;
extern volatile int wind_ticks;
extern volatile int wind_ticks_skipped;

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DIAG_Pin GPIO_PIN_13
#define DIAG_GPIO_Port GPIOC
#define VOLTS_Pin GPIO_PIN_0
#define VOLTS_GPIO_Port GPIOA
#define NRF_CSN_Pin GPIO_PIN_4
#define NRF_CSN_GPIO_Port GPIOA
#define NRF_CE_Pin GPIO_PIN_0
#define NRF_CE_GPIO_Port GPIOB
#define NRF_IRQ_Pin GPIO_PIN_1
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_IRQ_EXTI_IRQn EXTI1_IRQn
#define ANCHOR_Pin GPIO_PIN_13
#define ANCHOR_GPIO_Port GPIOB
#define LIGHT_ENA_Pin GPIO_PIN_14
#define LIGHT_ENA_GPIO_Port GPIOB
#define NAVI_Pin GPIO_PIN_15
#define NAVI_GPIO_Port GPIOB
#define WIND_TICK_Pin GPIO_PIN_8
#define WIND_TICK_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

void Error_Indication(int ledTimeOn, int ledTimeOff);

extern volatile bool sensorPhase;

extern const char signature[];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
