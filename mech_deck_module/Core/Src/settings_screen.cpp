//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <cmath>
#include "flash.inc"

/**
  * @brief  Disable the FLASH data cache.
  * @retval none
  */
#define __HAL_FLASH_DATA_CACHE_DISABLE()        CLEAR_BIT(FLASH->ACR, FLASH_ACR_DCEN)
/**
  * @brief  Flush the instruction and data caches.
  * @retval None
  */
void FLASH_FlushCaches(void)
{
    FLASH_CacheTypeDef cache = pFlash.CacheToReactivate;

    /* Flush instruction cache  */
    if((cache == FLASH_CACHE_ICACHE_ENABLED) ||
       (cache == FLASH_CACHE_ICACHE_DCACHE_ENABLED))
    {
        /* Reset instruction cache */
        __HAL_FLASH_INSTRUCTION_CACHE_RESET();
        /* Enable instruction cache */
        __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    }

    /* Flush data cache */
    if((cache == FLASH_CACHE_DCACHE_ENABLED) ||
       (cache == FLASH_CACHE_ICACHE_DCACHE_ENABLED))
    {
        /* Reset data cache */
        __HAL_FLASH_DATA_CACHE_RESET();
        /* Enable data cache */
        __HAL_FLASH_DATA_CACHE_ENABLE();
    }

    /* Reset internal variable */
    pFlash.CacheToReactivate = FLASH_CACHE_DISABLED;
}

/**
  * @brief  Program double word or fast program of a row at a specified address.
  * @param  TypeProgram  Indicate the way to program at a specified address.
  *                           This parameter can be a value of @ref FLASH_Type_Program
  * @param  Address  specifies the address to be programmed.
  * @param  Data specifies the data to be programmed
  *                This parameter is the data for the double word program and the address where
  *                are stored the data for the row fast program
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
void HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data) {
    uint32_t prog_bit = 0;

    /* Process Locked */

    /* Wait for last operation to be completed */
    FLASH_WaitForLastOperation();

    /* Deactivate the data cache if they are activated to avoid data misbehavior */
    if (READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U) {
        /* Disable data cache  */
        __HAL_FLASH_DATA_CACHE_DISABLE();
    }

    /* Program double-word (64-bit) at a specified address */
    FLASH_Program_DoubleWord(Address, Data);
    prog_bit = FLASH_CR_PG;

    /* Wait for last operation to be completed */
    FLASH_WaitForLastOperation();

    /* If the program operation is completed, disable the PG or FSTPG Bit */
    if (prog_bit != 0U) {
        CLEAR_BIT(FLASH->CR, prog_bit);
    }

    /* Flush the caches to be sure of the data consistency */
    FLASH_FlushCaches();
}

void SettingsScreen::processKeyboard() {
    if ((state.keyOk & (KEY_PRESS_E | KEY_REPEAT_E)) && !(state.keyUp || state.keyDown || state.keyCancel)) {
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
    } else if ((state.keyDown & (KEY_PRESS_E | KEY_REPEAT_E)) && !(state.keyOk || state.keyUp || state.keyCancel)) {
        position = (position + 1) % (maxPosition() + 1);
        if (!isChanged() && (position == SAVE_POSITION)) {
            position = CLOSE_POSITION;
        }
    } else if ((state.keyUp & (KEY_PRESS_E | KEY_REPEAT_E)) && !(state.keyOk || state.keyDown || state.keyCancel)) {
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
    uint32_t dstAddr = eraseDataFlash();

    auto *srcAddr = (uint64_t *) &localSettings;
    static const uint32_t endDstAddress = sizeof(struct NaviSettings) + dstAddr;
    FLASH_Unlock();
    /* Program the user Flash area word by word
      (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
    while (dstAddr < endDstAddress) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dstAddr, *srcAddr) != HAL_OK) {
            puts("Flash write error");
        }
        dstAddr += 8;
        srcAddr++;
    }
    FLASH_Lock();

    findSettings();
    localSettings = *naviSettings;
    if (position == SAVE_POSITION) position = CLOSE_POSITION;
}

uint32_t SettingsScreen::eraseDataFlash() {
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;

    FLASH_Unlock();
    FLASH_CLEAR_FLAG_OPTVERR();
    const auto dstAddr = (uint32_t) &flashSettings;

    /* Get the bank */
    if ((dstAddr) < (FLASH_BANK1_END)) {
//        eraseInit.Banks = FLASH_BANK_1;
//        eraseInit.Page = (dstAddr - FLASH_BASE) / 2048;
        //todo erase bank1
    } else {
//        eraseInit.Banks = FLASH_BANK_2;
//        eraseInit.Page = (dstAddr - FLASH_BASE - FLASH_BANK_SIZE) / 2048;
        //todo erase bank2
    }

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.NbPages = 1;
    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        puts("Flash erase error");
    }
    FLASH_Lock();
    return dstAddr;
}
