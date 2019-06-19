//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <cmath>

FactoryResetScreen factoryResetScreen;

static const uint8_t FACTORY_RESET_BACKGROUND[] = {
#include "bitmaps/factory-reset.txt"
};

void FactoryResetScreen::updatePicture() {
    Display::copyPict(FACTORY_RESET_BACKGROUND);

    SettingsScreen::drawButtons();
}


