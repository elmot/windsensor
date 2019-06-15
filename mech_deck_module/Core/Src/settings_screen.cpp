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
        position = (position + 1) % (maxPosition() + 1);
        if (!isChanged() && (position == SAVE_POSITION)) {
            position = CLOSE_POSITION;
        }
    } else if ((state.keyUp & KEY_PRESS_E) && !(state.keyOk || state.keyDown || state.keyCancel)) {
        position = (position + maxPosition()) % (maxPosition() + 1);
        if (!isChanged() && (position == SAVE_POSITION)) {
            position = maxPosition();
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
    if (position == SAVE_POSITION) position = CLOSE_POSITION;
}