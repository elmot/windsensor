#include <deck-module.hpp>

bool SpeedCorrectScreen::isChanged() {
    return true;
}

static const uint8_t CORRECT_SPEED_BACKGROUND[] = {
#include "bitmaps/speed-corr.txt"
};

void SpeedCorrectScreen::updatePicture() {
    Display::copyPict(CORRECT_SPEED_BACKGROUND);

    draw3digits(true, (int)localSettings.fixedWindMs, 0, 50, position - LAST_STD_BUTTON - 1);

    if (state.windTics >= 0) {
        draw3digits(false, (int)state.windSpdMps, 18, 54, -1);
    } else {
        charOutput(CHAR_CONN_FAIL, 22, 54);
    }
    SettingsScreen::drawButtons();
}

void SpeedCorrectScreen::changeValue(int delta) {
    SettingsScreen::changeValue(delta, &localSettings.fixedWindMs, 99, 0, position - LAST_STD_BUTTON - 1);
}

void SpeedCorrectScreen::gotoNextScreen() {
    nextScreen(&mainScreen);
}

int SpeedCorrectScreen::maxPosition() {
    return LAST_STD_BUTTON + 3;
}

void SpeedCorrectScreen::save() {
    localSettings.fixedTics = state.windTics;
    SettingsScreen::save();
}
