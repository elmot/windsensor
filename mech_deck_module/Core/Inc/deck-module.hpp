//
// Created by Ilia.Motornyi on 15/04/2019.
//

#ifndef MECH_DECK_MODULE_KEYBOARD_H
#define MECH_DECK_MODULE_KEYBOARD_HPP

#include <cstdbool>
#include "main.h"

enum DeviceState {
    CONN_FAIL,
    CONN_TIMEOUT,
    DATA_ERROR,
    DATA_NO_FIX,
    OK
};

enum LightsState {
    OFF,
    ANCHOR,
    NAVI
};


#define KEY_RELEASED 1
#define KEY_PRESSED 2
#define KEY_L_PRESSED 4
#define KEY_XL_PRESSED 8
#define KEY_PRESS_E 16
#define KEY_REPEAT_E 32
#define KEY_RELEASE_E 64

struct NaviState {
public:
    uint8_t keyup;
    uint8_t keydown;
    uint8_t keyok;
    uint8_t keycancel;

    DeviceState gpsState;
    DeviceState anemSatte;

    LightsState lights;

    /***
     * @brief value 0-100
     *
     */
    uint8_t backLightPwm;

    double lat;
    double lon;
    double speedK;
    double course;

    double windSpd;
    double windAngle;


private:
    uint_fast64_t lastPosTS;
    uint_fast64_t lastWindTS;
    uint_fast64_t lastScreenTS;


} ;

extern struct NaviState state;

#endif //MECH_DECK_MODULE_KEYBOARD_H
