//
// Created by Ilia.Motornyi on 15/04/2019.
//
#include <cmath>
#include "deck-module.hpp"

const NaviSettings defaultSettings;
const NaviSettings *naviSettings;

/* qsort int comparison function */
int float_array_head_cmp(const void *a, const void *b) {
    const auto da = (const float *) a; // casting pointer types
    const auto db = (const float *) b;
    return __signbitd(db[0] - da[0]);
}


void normalizetWindTable(float (*dest)[2], const NaviSettings *actualSettings) {
    int n = 0;
    for (int i = 0; i < WIND_TABLE_LEN; i++) {
        if (!actualSettings->windLineEnabled[i]) continue;
        bool q = false;
        for (int j = 0; (j < n) && !q; j++) {
            q = actualSettings->windTpMtoMs[i][0] == dest[j][0];
        }
        if (q) continue;
        dest[n][0] = actualSettings->windTpMtoMs[i][0];
        dest[n][1] = actualSettings->windTpMtoMs[i][1];
        n++;
    }
    qsort(dest, n, sizeof(float) * 2, &float_array_head_cmp);
    for (; n < WIND_TABLE_LEN; n++) {
        dest[n][0] = 0;
    }

}

void findSettings() {
    if (flashSettings.CONSISTENCY_SIGN == CONSISTENCY_SIGN_VALUE) {
        naviSettings = &flashSettings;
    } else {
        naviSettings = &defaultSettings;
    }
    normalizetWindTable(state.windTable, naviSettings);
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


