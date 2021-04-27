//
// Created by Ilia.Motornyi on 15/04/2019.
//

#ifndef MECH_DECK_MODULE_HPP
#define MECH_DECK_MODULE_HPP

#include <string>
#include "main.h"


#define KEY_RELEASED     0b000000u
#define KEY_PRESSED      0b000010u
#define KEY_L_PRESSED    0b000100u
#define KEY_XL_PRESSED   0b001000u
#define KEY_PRESS_E      0b010000u
#define KEY_REPEAT_E     0b100000u
#define KEY_RELEASE_E    0b000001u
#define CHAR_BACK_LIGHT 10
#define CHAR_CONN_FAIL 12
#define CHAR_ANCHOR 13
#define CHAR_NAVI 14

class NaviSettings {

public:
    unsigned int windAngleCorrection = 0;
    bool windLineEnabled[WIND_TABLE_LEN] = {true, true, true};
    float windTpMtoMs[WIND_TABLE_LEN][2] = {{10,  1},
                                             {100, 6},
                                             {30,  4},
                                             {300, 20}};
    float minWindMs = 1;
    int tooCloseAngle = 33;
    int tooFreeAngle = 170;
    unsigned long long CONSISTENCY_SIGN = CONSISTENCY_SIGN_VALUE;
};

extern const NaviSettings *naviSettings;
extern const NaviSettings flashSettings;
extern const NaviSettings defaultSettings;

class Display {
public:
    void static bgBrightness(uint16_t);

    void static copyPict(const uint8_t *background);

    void static paint();

    void static clearPict();

    void static copyPict(int xBytes, int y, int wBytes, int h, uint8_t xorMask, const uint8_t *pict);

    void static copyPict(int xBytes, int y, int wBytes, int h, const uint8_t *pict);

    static const uint16_t BG_MAX = 99;
    static const uint16_t BG_MIN = 0;

    virtual void pixel(float x, float y, int color);

    virtual void line(float x1, float y1, float x2, float y2, float width, const char *pattern);

    static void savePictTo(uint8_t screen_buffer[SCREEN_WIDTH_BYTES * SCREEN_HEIGHT]);

};


class MaskedDisplay : public Display {
public:
    void pixel(float x, float y, int color) override;
};

class AffineTransform : public Display {
public:
    explicit AffineTransform(Display &screen);

    void pixel(float x, float y, int color) override;

    void line(float x1, float y1, float x2, float y2, float width, const char *pattern) override;

    void reset();

    void rotate(float theta);

    void translate(float tx, float ty);

    void rotate(float theta, float anchorx, float anchory);

    void scale(float sx, float sy);

private:
    float m00 = 0;

    /**
     * The Y coordinate shearing element of the 3x3
     * affine transformation matrix.
     *
     * @serial
     */
    float m10 = 0;

    /**
     * The X coordinate shearing element of the 3x3
     * affine transformation matrix.
     *
     * @serial
     */
    float m01 = 0;

    /**
     * The Y coordinate scaling element of the 3x3
     * affine transformation matrix.
     *
     * @serial
     */
    float m11 = 0;

    /**
     * The X coordinate of the translation element of the
     * 3x3 affine transformation matrix.
     *
     * @serial
     */
    float m02 = 0;

    /**
     * The Y coordinate of the translation element of the
     * 3x3 affine transformation matrix.
     *
     * @serial
     */
    float m12 = 0;

    Display &screen;
};

extern AffineTransform affineScreen;
extern MaskedDisplay maskedDisplay;

class Screen {
protected:

    virtual void leave() {};

    void static draw3digits(bool big, unsigned int val, unsigned int xBytes, unsigned int y, int activepos);

public:
    static Screen *activeScreen;

    virtual void enter() {};

    virtual void updatePicture() = 0;

    virtual void processKeyboard() = 0;

    static void nextScreen(Screen *next) {
        activeScreen->leave();
        activeScreen = next;
        activeScreen->enter();
        state.waitUntilKeysReleased = true;
    };

    static void invertBox(int xBytes, int y, int wBytes, int h);

    static void chessBox(int xBytes, int y, int wBytes, int h);

};

class MainScreen : public Screen {
private:
    static void drawScreenData();

public:
    void updatePicture() override;

    void processKeyboard() override;

};

class SettingsScreen : public Screen {
protected:
    static const int SAVE_POSITION = 0;
    static const int CLOSE_POSITION = 1;
    static const int NEXT_POSITION = 2;
    static const int LAST_STD_BUTTON = NEXT_POSITION;
    int position = NEXT_POSITION;

    void processKeyboard() override;

    void enter() override {
        position = NEXT_POSITION;
        localSettings = *naviSettings;
    };

    virtual void gotoNextScreen() = 0;

    virtual void save();

    virtual int maxPosition() = 0;

    virtual void changeValue(int delta) = 0;

    virtual bool isChanged() = 0;

    void drawButtons();

    void changeValue(int delta, unsigned int *changeableValue, unsigned int max, unsigned int min,
                     int activePosition) const;

protected:
    NaviSettings localSettings = *naviSettings;

    static uint32_t eraseDataFlash();

    static uint8_t bg_buffer[SCREEN_WIDTH_BYTES * SCREEN_HEIGHT];
};

extern MainScreen mainScreen;

class AngleCorrectScreen : public SettingsScreen {
public:
    void updatePicture() override;

protected:
    void gotoNextScreen() override;

    int maxPosition() override;

    void changeValue(int delta) override;

    bool isChanged() override;

};

extern AngleCorrectScreen angleCorrectScreen;

class AlarmCorrectScreen : public SettingsScreen {
public:
    void updatePicture() override;

protected:
    void gotoNextScreen() override;

    int maxPosition() override;

    void changeValue(int delta) override;

    bool isChanged() override;

};

extern AlarmCorrectScreen alarmCorrectScreen;

class FactoryResetScreen : public SettingsScreen {
public:
    void updatePicture() override;

protected:
    void gotoNextScreen() override { nextScreen(&mainScreen); };

    int maxPosition() override { return LAST_STD_BUTTON; };

    void changeValue(int delta) override {};

    bool isChanged() override { return true; };

    void save() override {
        SettingsScreen::eraseDataFlash();
        NVIC_SystemReset();
    }
};

extern FactoryResetScreen factoryResetScreen;

class CalibScreen : public SettingsScreen {
public:
    void updatePicture() override;

protected:
    uint8_t myBackground[SCREEN_WIDTH_BYTES * SCREEN_HEIGHT];

    void enter() override;

    void leave() override;

    void gotoNextScreen() override {
        nextScreen(&mainScreen);
    }

    void changeValue(int delta) override;

    int maxPosition() override;

    bool isChanged() override;

    Display calibDisplay = Display();
    AffineTransform myAffineTransform = AffineTransform(calibDisplay);

private:
    float localWindTable[WIND_TABLE_LEN][2];
    bool zoom = false;

    void printCalibrations();

    void prepareBackground();

    int maxScreenWind() { return zoom ? 10 : 30; };
};

extern CalibScreen calibScreen;

extern const uint8_t FONT[];
extern const uint8_t FONT40x48[];

inline void charOutput(int charCode, int xBytes, int yPos, uint8_t xorMask = 0) {
    Display::copyPict(xBytes, yPos, 4, 32, xorMask, &FONT[32 * 4 * (14 - charCode)]);
}

inline void bigCharOutput(int charCode, int xBytes, int yPos, bool invert = false) {
    Display::copyPict(xBytes, yPos, 5, 48, invert ? 0xff : 0, &FONT40x48[48 * 5 * (19 - charCode)]);
}

float calcSpeed(const float windTable[][2], int ticksPerMin);

void normalizetWindTable(float dest[WIND_TABLE_LEN][2], const NaviSettings *actualSettings);

#endif //MECH_DECK_MODULE_HPP
