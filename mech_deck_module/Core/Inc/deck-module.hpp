//
// Created by Ilia.Motornyi on 15/04/2019.
//

#ifndef MECH_DECK_MODULE_KEYBOARD_H
#define MECH_DECK_MODULE_KEYBOARD_HPP

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


#define KEY_RELEASED     0b000000u
#define KEY_PRESSED      0b000010u
#define KEY_L_PRESSED    0b000100u
#define KEY_XL_PRESSED   0b001000u
#define KEY_PRESS_E      0b010000u
#define KEY_REPEAT_E     0b100000u
#define KEY_RELEASE_E    0b000001u

#define WIND_TABLE_LEN 5
struct NaviSettings {
    int windAngleCorrection ;
    double windTpsToMs [WIND_TABLE_LEN][2];
    double minWindMs ;
    int tooCloseAngle;
    int tooFreeAngle;

};
struct NaviState {
//public:
    uint8_t keyUp;
    uint8_t keyDown;
    uint8_t keyOk;
    uint8_t keyCancel;

    enum DeviceState gpsState;
    enum DeviceState anemState;

    enum LightsState lights;

    /***
     * @brief value 0-100
     *
     */
    int8_t backLightPwm;

    double lat;
    double lon;
    double speedK;
    double course;

    double windSpd;
    int windAngle;

    uint_fast64_t lastPosTS;
    uint_fast64_t lastWindTS;
    uint_fast64_t lastScreenTS;
} ;

extern struct NaviState state;
extern struct NaviSettings naviSettings;


enum DeviceState radioCheck();

#endif //MECH_DECK_MODULE_KEYBOARD_H
