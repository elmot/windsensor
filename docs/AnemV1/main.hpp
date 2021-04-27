/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_DEMO_H
#define __STM32F4_DISCOVERY_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_dbgmcu.h"
#include <stdio.h>
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Voltage constants */
const float VREF = 3.0f;
#define BAT_ADC_PORT	GPIOC
#define BAT_ADC_PIN		GPIO_Pin_5
#define BAT_ADC_CLOCK	RCC_AHB1Periph_GPIOC
#define BAT_ADC_CHANNEL ADC_Channel_15

/* Keyboard */
#define  FRONT_LED_PORT	GPIOD
#define  FRONT_LED_PIN	GPIO_Pin_14
#define  KEYB_CLOCK1	RCC_AHB1Periph_GPIOD
#define  KEYB_CLOCK2	RCC_AHB1Periph_GPIOC

#define  BTN_UP_PORT	GPIOC
#define  BTN_UP_PIN		GPIO_Pin_4
#define  BTN_OK_PORT	GPIOD
#define  BTN_OK_PIN		GPIO_Pin_3
#define  BTN_DOWN_PORT	GPIOD
#define  BTN_DOWN_PIN		GPIO_Pin_4

/* Exported macro ------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x
#define MAX(a,b)       (a < b) ? (b) : a
/* Exported functions ------------------------------------------------------- */

#endif /* __STM32F4_DISCOVERY_DEMO_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
