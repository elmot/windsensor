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
    LL_TIM_OC_SetCompareCH1(TIM22, 99 - state.backLightPwm);
}

void mainLoop() {
    /* Enable output channel 1 */
    LL_TIM_CC_EnableChannel(TIM22, LL_TIM_CHANNEL_CH1);
    /* Enable counter */
    LL_TIM_EnableCounter(TIM22);
    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(TIM22);

    radioCheck();
    radioInit();
    initLcd();
    LL_InitTick(SystemCoreClock, 200);
    LL_SYSTICK_EnableIT();
    splashLcd();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        radioLoop();
        __disable_irq();
        uint64_t ts = sysTicks;
        __enable_irq();
        updateKeyboard(ts);
        adjustBackLight();
        updateLcd(ts);

        LL_mDelay(2);

    }
#pragma clang diagnostic pop

}

