//
// Created by Ilia.Motornyi on 14/04/2019.
//

#ifndef MECH_DECK_MODULE_SCREEN_H
#define MECH_DECK_MODULE_SCREEN_H

#include "main.h"

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
    virtual void updatePicture();

    virtual void processKeyboard();
};

class MainScreen : public Screen {
public:
    virtual void updatePicture() override;

    virtual void processKeyboard() override;

};

class AngleCorrectScreen : public Screen {
public:
    virtual void updatePicture() override;

    virtual void processKeyboard() override;

};

#endif //MECH_DECK_MODULE_SCREEN_H
