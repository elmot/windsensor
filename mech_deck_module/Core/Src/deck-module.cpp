//
// Created by Ilia.Motornyi on 15/04/2019.
//
#include "deck-module.hpp"

static void adjustBackLight() {
    if (state.keyUp & (KEY_PRESS_E | KEY_REPEAT_E)) {
        state.backLightPwm += 10;
        if (state.backLightPwm > 99) {
            state.backLightPwm = 99;
        }
    } else if (state.keyDown & (KEY_PRESS_E | KEY_REPEAT_E)) {
        state.backLightPwm -= 10;
        if (state.backLightPwm < 0) {
            state.backLightPwm = 0;
        }
    }
    DISP_BG_TIM_SETPULSE(DISP_BG_TIM, 99 - state.backLightPwm);
}

void mainLoop() {
    /* Enable output channel 1 */
    LL_TIM_CC_EnableChannel(DISP_BG_TIM, DISP_BG_TIM_CHANNEL);
    /* Enable counter */
    LL_TIM_EnableCounter(DISP_BG_TIM);
    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(DISP_BG_TIM);

    LL_InitTick(SystemCoreClock, 200);
    LL_SYSTICK_EnableIT();

    initLcd();
    radioCheck();
    radioInit();
    splashLcd();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if (!LL_GPIO_IsInputPinSet(KEY_L_ANCHOR_GPIO_Port, KEY_L_ANCHOR_Pin)) {
            state.lights = ANCHOR;
        } else if (!LL_GPIO_IsInputPinSet(KEY_L_NAVI_GPIO_Port, KEY_L_NAVI_Pin)) {
            state.lights = NAVI;
        } else {
            state.lights = OFF;
        }
        radioLoop();
        __disable_irq();
        uint64_t ts = sysTicks();
        __enable_irq();
        updateKeyboard(ts);
        adjustBackLight();
        updateLcd(ts);

        msDelay(10);
        LL_IWDG_ReloadCounter(IWDG);
    }
#pragma clang diagnostic pop

}

