//
// Created by Ilia.Motornyi on 15/04/2019.
//

#include "deck-module.hpp"

#define DEBOUNCE 1

#define FIRST_DELAY 120
#define DELAY 40


class Key {
public:
    bool lastState = false;
    uint_fast64_t lastChangeTime = 0;
    int repeatCount = 0;
};

Key keyUp;
Key keyDown;
Key keyOk;
Key keyCancel;

static void updateKey(uint8_t &keyByte, GPIO_TypeDef *port, uint16_t pin, Key &key, uint_fast64_t timeStamp) {
    bool newState = !LL_GPIO_IsInputPinSet(port, pin);
    int dt = timeStamp - key.lastChangeTime;

    //Debounce processing
    if (key.lastState != newState) {
        if (dt < DEBOUNCE) return;
        key.lastChangeTime = timeStamp;
        key.lastState = newState;
        key.repeatCount = 0;
        if (newState) {
            keyByte = KEY_PRESS_E | KEY_PRESSED;
        } else {
            keyByte = KEY_RELEASE_E | KEY_RELEASED;
        }
        return;
    }

    if (keyByte & KEY_PRESS_E) {
        keyByte &= ~KEY_PRESS_E;
    } else if (keyByte & KEY_RELEASE_E) {
        keyByte &= ~KEY_RELEASE_E;
    } else if (keyByte & KEY_REPEAT_E) {
        keyByte &= ~KEY_REPEAT_E;
    } else if (keyByte & KEY_PRESSED &&
               (((key.repeatCount > 0) && (dt >= DELAY)) || (dt >= FIRST_DELAY))) {
        key.lastChangeTime = timeStamp;
        keyByte |= KEY_REPEAT_E;
        key.repeatCount++;
        switch (key.repeatCount) {
            case 4:
                keyByte |= KEY_L_PRESSED;
                break;
            case 16:
                keyByte |= KEY_XL_PRESSED;
                break;
        }
    }
}

void updateKeyboard(uint_fast64_t timeStamp) {
    updateKey(state.keyUp, KEY_UP_GPIO_Port, KEY_UP_Pin, keyUp, timeStamp);
    updateKey(state.keyDown, KEY_DOWN_GPIO_Port, KEY_DOWN_Pin, keyDown, timeStamp);
    updateKey(state.keyOk, KEY_OK_GPIO_Port, KEY_OK_Pin, keyOk, timeStamp);
    updateKey(state.keyCancel, KEY_CANCEL_GPIO_Port, KEY_CANCEL_Pin, keyCancel, timeStamp);
}