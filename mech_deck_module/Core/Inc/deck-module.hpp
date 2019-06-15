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

    static const uint16_t BG_MAX = 0;
    static const uint16_t BG_MIN = 99;

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
public:
    virtual void updatePicture() = 0;

    virtual void processKeyboard() = 0;
};

class MainScreen : public Screen {
private:
    void drawScreenData();
public:
    void updatePicture() override;

    void processKeyboard() override;

};

class AngleCorrectScreen : public Screen {
public:
    void updatePicture() override;

    void processKeyboard() override;

};

extern AffineTransform affineScreen;

extern MainScreen mainScreen;
extern AngleCorrectScreen angleCorrectScreen;
extern Screen *activeScreen;

extern const uint8_t FONT[];
extern const uint8_t FONT40x48[];

inline void charOutput(int charCode, int xBytes, int yPos) {
    Display::copyPict(xBytes, yPos, 4, 32, &FONT[32 * 4 * (14 - charCode)]);
}

inline void bigCharOutput(int charCode, int xBytes, int yPos) {
    Display::copyPict(xBytes, yPos, 5, 48, &FONT40x48[48 * 5 * (19 - charCode)]);
}


#endif //MECH_DECK_MODULE_HPP
