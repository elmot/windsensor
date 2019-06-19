//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <cmath>

AlarmCorrectScreen alarmCorrectScreen;

bool AlarmCorrectScreen::isChanged() {
    return (localSettings.tooFreeAngle != naviSettings->tooFreeAngle) ||
           (localSettings.tooCloseAngle != naviSettings->tooCloseAngle);
}

static const uint8_t CORRECT_ALARM_BACKGROUND[] = {
#include "bitmaps/alarm-corr.txt"
};

void AlarmCorrectScreen::updatePicture() {
    Display::copyPict(CORRECT_ALARM_BACKGROUND);
    draw3digits(false, localSettings.tooCloseAngle, 18, 71, position - LAST_STD_BUTTON - 1);
    draw3digits(false, localSettings.tooFreeAngle, 18, 38, position - LAST_STD_BUTTON - 4);

    SettingsScreen::drawButtons();
}

void AlarmCorrectScreen::changeValue(int delta) {
    if(position <= LAST_STD_BUTTON+3) {
        SettingsScreen::changeValue(delta, (unsigned int *) &localSettings.tooCloseAngle, 360, 0, position - LAST_STD_BUTTON - 1);
    } else {
        SettingsScreen::changeValue(delta, (unsigned int *) &localSettings.tooFreeAngle, 360, 0, position - LAST_STD_BUTTON - 4);
    }
}

void AlarmCorrectScreen::gotoNextScreen() {
    nextScreen(&angleCorrectScreen);
}

int AlarmCorrectScreen::maxPosition() {
    return LAST_STD_BUTTON + 6;
}

