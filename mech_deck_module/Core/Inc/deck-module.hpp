//
// Created by Ilia.Motornyi on 15/04/2019.
//

#ifndef MECH_DECK_MODULE_HPP
#define MECH_DECK_MODULE_HPP

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

class Screen {
protected:
    virtual void enter() {};

public:
    static Screen *activeScreen;

    virtual void updatePicture() = 0;

    virtual void processKeyboard() = 0;

    static void nextScreen(Screen *next) {
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

    void draw3digits(unsigned int val, unsigned int xBytes, unsigned int y, int activepos) const;
};

extern AlarmCorrectScreen alarmCorrectScreen;

extern AffineTransform affineScreen;

extern const uint8_t FONT[];
extern const uint8_t FONT40x48[];

inline void charOutput(int charCode, int xBytes, int yPos, uint8_t xorMask = 0) {
    Display::copyPict(xBytes, yPos, 4, 32, xorMask, &FONT[32 * 4 * (14 - charCode)]);
}

inline void bigCharOutput(int charCode, int xBytes, int yPos, bool invert = false) {
    Display::copyPict(xBytes, yPos, 5, 48, invert ? 0xff : 0, &FONT40x48[48 * 5 * (19 - charCode)]);
}


#endif //MECH_DECK_MODULE_HPP
