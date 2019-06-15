//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <math.h>

static const uint8_t CORRECT_ANGLE_BACKGROUND[] = {
#include "bitmaps/angle-corr.txt"
};


void AngleCorrectScreen::processKeyboard() {
    //todo change values
    //todo save
    //todo next
    //todo close
}

void AngleCorrectScreen::updatePicture() {
    Display::copyPict(CORRECT_ANGLE_BACKGROUND);
    //todo show comm failure
    //todo cursor , button save, button next, button close
    unsigned int char1, char2, char3;

    char1 = naviSettings.windAngleCorrection / 100;
    char2 = naviSettings.windAngleCorrection / 10 % 10;
    char3 = naviSettings.windAngleCorrection % 10;

    bigCharOutput(char1, 0, 50);
    bigCharOutput(char2, 5, 50);
    bigCharOutput(char3, 10, 50);

    if (state.windAngle >= 0) {
        char1 = state.windAngle / 100;
        char2 = state.windAngle / 10 % 10;
        char3 = state.windAngle % 10;
        charOutput(char1, 18, 54);
        charOutput(char2, 22, 54);
        charOutput(char3, 26, 54);
    } else {
        charOutput(CHAR_CONN_FAIL, 22, 54);
    }
}
