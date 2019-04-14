//
// Created by Ilia.Motornyi on 14/04/2019.
//

#ifndef MECH_DECK_MODULE_SCREEN_H
#define MECH_DECK_MODULE_SCREEN_H

#include "main.h"

class Screen {
public:
    void static bgBrightness(uint16_t);
    void static copyPict(const uint8_t * background);
    void static displayScreen();
    void static clearPict();
    void static copyPict(int xBytes, int y, int wBytes, int h, const uint8_t * pict);
    static const uint16_t BG_MAX = 0;
    static const uint16_t BG_MIN = 99;
};

extern Screen Screen;
#endif //MECH_DECK_MODULE_SCREEN_H
