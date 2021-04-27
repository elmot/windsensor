/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

#include "stm32l4xx_ll_iwdg.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_dma.h"

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

#define NRF_SPI SPI2

#include "../../../sensor-signature.h"
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
#define NRF_IRQ_Pin LL_GPIO_PIN_10
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_CE_Pin LL_GPIO_PIN_11
#define NRF_CE_GPIO_Port GPIOB
#define NRF_CSN_Pin LL_GPIO_PIN_12
#define NRF_CSN_GPIO_Port GPIOB
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
/* USER CODE BEGIN Private defines */
void initLcd(void);

void findSettings(void);

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

#define WIND_TABLE_LEN 7

void mainLoop()__attribute__((noreturn));

enum DeviceState {
    CONN_FAIL,
    CONN_TIMEOUT,
    DATA_ERROR,
    DATA_NO_FIX,
    OK
};

enum LightsState {
    OFF,
    ANCHOR,
    NAVI
};

#define CONSISTENCY_SIGN_VALUE 0xAA5533CC18819669ul

struct NaviState {
//public:
    uint8_t keyUp;
    uint8_t keyDown;
    uint8_t keyOk;
    uint8_t keyCancel;

    enum DeviceState gpsState;
    enum DeviceState anemState;

    enum LightsState lights;

    /***
     * @brief value 0-100
     *
     */
    int8_t backLightPwm;

    float lat;
    float lon;
    float speedK;
    float course;

    int windTics;
    float windSpdMps;
    int windAngle;
    int windAngleNonCorrected;

    uint_fast64_t lastPosTS;
    uint_fast64_t lastWindTS;
    uint_fast64_t lastScreenTS;
    bool waitUntilKeysReleased;

    float windTable[WIND_TABLE_LEN][2];
} ;

extern struct NaviState state;


enum DeviceState radioCheck();

void processKeyboard();

#define SCREEN_WIDTH 240
#define SCREEN_WIDTH_BYTES (SCREEN_WIDTH / 8)
#define SCREEN_HEIGHT 128

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
