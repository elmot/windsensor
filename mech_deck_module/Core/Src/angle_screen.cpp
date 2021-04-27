//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <cmath>

bool AngleCorrectScreen::isChanged() {
    return localSettings.windAngleCorrection != naviSettings->windAngleCorrection;
}

static const uint8_t CORRECT_ANGLE_BACKGROUND[] = {
#include "bitmaps/angle-corr.txt"
};

void AngleCorrectScreen::updatePicture() {
    Display::copyPict(CORRECT_ANGLE_BACKGROUND);

    draw3digits(true, localSettings.windAngleCorrection, 0, 50, position - LAST_STD_BUTTON - 1);

    if (state.windAngle >= 0) {
        draw3digits(false, state.windAngleNonCorrected, 18, 54, -1);
    } else {
        charOutput(CHAR_CONN_FAIL, 22, 54);
    }
    SettingsScreen::drawButtons();
}

void AngleCorrectScreen::changeValue(int delta) {
    SettingsScreen::changeValue(delta, &localSettings.windAngleCorrection, 360, 0, position - LAST_STD_BUTTON - 1);
}

void AngleCorrectScreen::gotoNextScreen() {
    nextScreen(&speedCorrectScreen);
}

int AngleCorrectScreen::maxPosition() {
    return LAST_STD_BUTTON + 3;
}

AngleCorrectScreen angleCorrectScreen = AngleCorrectScreen();
