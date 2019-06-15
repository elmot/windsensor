//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <math.h>

bool AngleCorrectScreen::isChanged() {
    return localSettings.windAngleCorrection != naviSettings->windAngleCorrection;
}

static const uint8_t CORRECT_ANGLE_BACKGROUND[] = {
#include "bitmaps/angle-corr.txt"
};

void AngleCorrectScreen::updatePicture() {
    Display::copyPict(CORRECT_ANGLE_BACKGROUND);
    unsigned int char1, char2, char3;

    char1 = localSettings.windAngleCorrection / 100;
    char2 = localSettings.windAngleCorrection / 10 % 10;
    char3 = localSettings.windAngleCorrection % 10;

    bigCharOutput(char1, 0, 50, position == 3);
    bigCharOutput(char2, 5, 50, position == 4);
    bigCharOutput(char3, 10, 50, position == 5);

    if (state.windAngle >= 0) {
        char1 = state.windAngleNonCorrected / 100;
        char2 = state.windAngleNonCorrected / 10 % 10;
        char3 = state.windAngleNonCorrected % 10;
        charOutput(char1, 18, 54);
        charOutput(char2, 22, 54);
        charOutput(char3, 26, 54);
    } else {
        charOutput(CHAR_CONN_FAIL, 22, 54);
    }
    SettingsScreen::drawButtons();
}

void AngleCorrectScreen::changeValue(int delta) {
    SettingsScreen::changeValue(delta, &localSettings.windAngleCorrection, 360, 0, position - LAST_STD_BUTTON - 1);
}

void AngleCorrectScreen::gotoNextScreen() {
    nextScreen(&mainScreen);
}

int AngleCorrectScreen::maxPosition() {
    return LAST_STD_BUTTON + 3;
}

