//
// Created by Ilia.Motornyi on 15/04/2019.
//
#include <cmath>
#include "deck-module.hpp"

const NaviSettings defaultSettings;
const NaviSettings *naviSettings;

void findSettings() {
    if (flashSettings.CONSISTENCY_SIGN == CONSISTENCY_SIGN_VALUE) {
        naviSettings = &flashSettings;
    } else {
        naviSettings = &defaultSettings;
    }
    state.fixedTics = naviSettings->fixedTics;
    state.fixedWindMs = naviSettings->fixedWindMs;
}

void mainLoop() {

    LL_InitTick(SystemCoreClock, 200);
    LL_SYSTICK_EnableIT();

    findSettings();
    initLcd();
    radioCheck();
    radioInit();
    Display::bgBrightness(state.backLightPwm);
    Screen::activeScreen->enter();
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
        processKeyboard();
        updateLcd(ts);
        outputNmea();

        msDelay(10);
        LL_IWDG_ReloadCounter(IWDG);
    }
#pragma clang diagnostic pop

}


