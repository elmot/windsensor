//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <math.h>

void SettingsScreen::processKeyboard() {
    if ((state.keyOk & KEY_PRESS_E) && !(state.keyUp || state.keyDown || state.keyCancel)) {
        switch (position) {
            case SAVE_POSITION:
                save();
                break;
            case CLOSE_POSITION:
                nextScreen(&mainScreen);
                break;
            case NEXT_POSITION:
                gotoNextScreen();
                break;
            default:
                changeValue(1);
        }
    } else if ((state.keyDown & KEY_PRESS_E) && !(state.keyOk || state.keyUp || state.keyCancel)) {
        position = (position + 1) % positionsCount();
        if(!isChanged() && (position == SAVE_POSITION)) {
            position = CLOSE_POSITION;
        }
    } else if ((state.keyUp & KEY_PRESS_E) && !(state.keyOk || state.keyDown || state.keyCancel)) {
        position = (position + positionsCount() - 1) % positionsCount();
        if(!isChanged() && (position == SAVE_POSITION)) {
            position = positionsCount()-1;
        }
    }
}

void SettingsScreen::drawButtons() {
    if (!isChanged()) {
        chessBox(0, 0, 10, 35);
    }
    switch (position) {
        case SAVE_POSITION:
            invertBox(0, 0, 10, 35);
            break;
        case CLOSE_POSITION:
            invertBox(10, 0, 10, 35);
            break;
        case NEXT_POSITION:
            invertBox(20, 0, 10, 35);
            break;
        default:
            break;
    }
}

bool AngleCorrectScreen::isChanged() {
    return localSettings.windAngleCorrection != naviSettings->windAngleCorrection;
}

void SettingsScreen::save() {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    auto dstAddr = (uint32_t) &flashSettings;

    /* Get the bank */
    if ((dstAddr) < (FLASH_BASE + FLASH_BANK_SIZE)) {
        eraseInit.Banks = FLASH_BANK_1;
        eraseInit.Page = (dstAddr - FLASH_BASE) / 2048;
    } else {
        eraseInit.Banks = FLASH_BANK_2;
        eraseInit.Page = (dstAddr - FLASH_BASE - FLASH_BANK_SIZE) / 2048;
    }

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.NbPages = 1;
    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        puts("Flash erase error");
    }

    auto *srcAddr = (uint64_t *) &localSettings;
    static const uint32_t endDstAddress = sizeof(struct NaviSettings) + dstAddr;

    /* Program the user Flash area word by word
      (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
    while (dstAddr < endDstAddress) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dstAddr, *srcAddr) != HAL_OK) {
            puts("Flash write error");
        }
        dstAddr += 8;
        srcAddr++;
    }
    HAL_FLASH_Lock();

    findSettings();
    localSettings = *naviSettings;
    if(position == SAVE_POSITION) position = CLOSE_POSITION;
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
    int digitDivider;
    switch (position) {
        case 3:
            digitDivider = 100;
            break;
        case 4:
            digitDivider = 10;
            break;
        case 5:
            digitDivider = 1;
            break;
        default:
            return;
    }
    int digitValue = (localSettings.windAngleCorrection / digitDivider) % 10;
    int newDigitValue = (digitValue + delta + 10) % 10;
    int correctionValue = (newDigitValue - digitValue) * digitDivider;
    localSettings.windAngleCorrection += correctionValue;
    if (localSettings.windAngleCorrection >= 360) localSettings.windAngleCorrection = 0;
    else
        while (localSettings.windAngleCorrection < 0) localSettings.windAngleCorrection += 360;
}