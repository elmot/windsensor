//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <math.h>

/* LCM commands */
MainScreen mainScreen = MainScreen();

static const uint8_t BACKGROUND[] = {
#include "bitmaps/background.txt"
};

void MainScreen::drawScreenData() {
    if (state.windAngle >= 0) {
        int angle;
        int direction;
        if (state.windAngle > 180) {
            angle = 360 - state.windAngle;
            direction = 11;
        } else {
            angle = state.windAngle;
            direction = 12;
        }
        if (angle < naviSettings->tooCloseAngle || angle > naviSettings->tooFreeAngle) {
            bigCharOutput(13, 25, 32);
        }
        bigCharOutput(direction, 20, 32);
        bigCharOutput(angle / 100, 15, 80);
        bigCharOutput((angle / 10) % 10, 20, 80);
        bigCharOutput(angle % 10, 25, 80);
    }

    charOutput(((int) state.windSpdMps / 10) % 10, 17, 0);
    charOutput((int) state.windSpdMps % 10, 21, 0);
    if (state.backLightPwm != Display::BG_MIN) {
        charOutput(CHAR_BACK_LIGHT, 0, 0);
    }
    switch (state.lights) {
        case ANCHOR:
            charOutput(CHAR_ANCHOR, 6, 38);
            break;
        case NAVI:
            charOutput(CHAR_NAVI, 6, 38);
            break;
        default:;
    }
}

void MainScreen::processKeyboard() {
    if ((state.keyUp & KEY_L_PRESSED) && (state.keyDown & KEY_L_PRESSED)) {
        nextScreen(&alarmCorrectScreen);
    }
        if ((state.keyUp & (KEY_PRESS_E | KEY_REPEAT_E)) && (state.keyDown == 0)) {
            state.backLightPwm += 10;
            if (state.backLightPwm > Display::BG_MAX) {
                state.backLightPwm = Display::BG_MAX;
            }
        } else if ((state.keyDown & (KEY_PRESS_E | KEY_REPEAT_E)) && (state.keyUp == 0)) {
            state.backLightPwm -= 10;
            if (state.backLightPwm < Display::BG_MIN) {
                state.backLightPwm = Display::BG_MIN;
            }
        }
    Display::bgBrightness(state.backLightPwm);
}

void MainScreen::updatePicture() {
    Display::copyPict(BACKGROUND);
    switch (state.anemState) {
        case OK:
            affineScreen.reset();
            affineScreen.rotate((int) -state.windAngle / 180.0 * M_PI, 63, 63);
            affineScreen.line(63, 127, 63, 63, 3, "1");
            drawScreenData();
            break;
        case CONN_TIMEOUT:
        case CONN_FAIL:
            Display::copyPict(6, 38, 4, 32, 0xFF, &FONT[2 * 32 * 4]);
            break;
        default:
            charOutput(CHAR_CONN_FAIL, 6, 38);
            break;
    }
}


