//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <math.h>

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
    draw3digits(localSettings.tooCloseAngle, 18, 71, position - LAST_STD_BUTTON - 1);
    draw3digits(localSettings.tooFreeAngle, 18, 38, position - LAST_STD_BUTTON - 4);

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

void AlarmCorrectScreen::draw3digits(unsigned int val, unsigned int xBytes, unsigned int y, int activepos) const {
    char char1 = val / 100;
    char char2 = val / 10 % 10;
    char char3 = val % 10;

    charOutput(char1, xBytes, y, activepos == 0 ? 0xFF : 0);
    charOutput(char2, xBytes + 4, y, activepos == 1 ? 0xFF : 0);
    charOutput(char3, xBytes + 8, y, activepos == 2 ? 0xFF : 0);
}

