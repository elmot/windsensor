//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <memory.h>
#include <tgmath.h>
#include "screen.hpp"


#define WIDTH 240
#define WIDTH_BYTES (WIDTH / 8)
#define HEIGHT 128

/* LCM commands */
#define LCM_SET_TEXT_HOME 0x40
#define LCM_SET_TEXT_COLS 0x41

#define LCM_SET_GRAPH_HOME 0x42
#define LCM_SET_GRAPH_COLS 0x43

Screen mainScreen;
AffineTransform affineScreen = AffineTransform(mainScreen);

static void writeCommand0(uint8_t command);

static void writeCommand1(uint8_t command, u_int8_t param1);

static void writeCommand2(uint8_t command, u_int8_t param1, u_int8_t param2);

static void writeData(uint8_t dataByte);

static void lcmClear();

static void locateGraphZero();

static uint8_t screen_buffer[WIDTH_BYTES * HEIGHT];

static const uint8_t SPLASH_PICTURE[] = {
#include "splash.txt"
};

static const uint8_t LEFT_BACKGROUND[] = {
#include "background.txt"
};

void initLcd(void) {
    LL_GPIO_ResetOutputPin(DISP_RESET_GPIO_Port, DISP_RESET_Pin);
    LL_mDelay(50);
    LL_GPIO_SetOutputPin(DISP_RESET_GPIO_Port, DISP_RESET_Pin);
    LL_mDelay(50);
    writeCommand2(LCM_SET_TEXT_HOME, 0, 0x8);
    writeCommand2(LCM_SET_TEXT_COLS, WIDTH_BYTES, 0);
    writeCommand2(LCM_SET_GRAPH_HOME, 0, 0);
    writeCommand2(LCM_SET_GRAPH_COLS, WIDTH_BYTES, 0);
    writeCommand0(0xa7);//cursor pattern 8 lines
    writeCommand0(0x80);//Internal GC rom mode, OR mode
    lcmClear();
    writeCommand0(0x98);//text off, graphics on
    Screen::bgBrightness(Screen::BG_MAX);
}

void splashLcd(void) {
    for (int i = 0; i < (2 * WIDTH_BYTES) + 1; i++) {
        Screen::copyPict(SPLASH_PICTURE);
        for (int j = 0; j < HEIGHT; j++) {
            int k = -j / 5 + i;
            if (k < 0) k = 0;
            for (; k < WIDTH_BYTES; k++) {
                screen_buffer[j * WIDTH_BYTES + k] = 0;
            }
        }
        Screen::displayScreen();
        LL_mDelay(1);
    }
    LL_mDelay(70);
}

void updateLcd(uint_fast64_t timeStamp) {
    static uint_fast64_t lastUpdate = 0;
    if (timeStamp - lastUpdate < 50) return;
    lastUpdate = timeStamp;
    static float theta = 0.0;
    Screen::clearPict();
    Screen::copyPict(0, 0, 16, 128, LEFT_BACKGROUND);
    affineScreen.reset();
    affineScreen.rotate(theta += 0.1, 63, 63);
    affineScreen.line(63, 127, 63, 63, 2, "10001100");
    Screen::displayScreen();
}

void lcmClear() {
    uint a;
    locateGraphZero();
    writeCommand0(0xb0);
    for (a = 0; a < 8192; a++) {
        writeData(0);
    }
    writeCommand0(0xb2);
}

void locateGraphZero() {
    writeCommand2(0x24, 0, 0);
}

void Screen::displayScreen() {
    locateGraphZero();
    writeCommand0(0xb0);
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH_BYTES; x++)
            writeData(screen_buffer[y * WIDTH_BYTES + x]);
    }
    writeCommand0(0xb2);
}

/***************************/
/* Low Level functions     */
/***************************/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"

void static inline __attribute__((optimize("O0"))) shortDelay() {
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
}

#pragma clang diagnostic pop

void static waitForBits(uint32_t bits) {
    MODIFY_REG(DISP_D0_GPIO_Port->MODER, 0xFFFFU, 0x0000U);
    uint b;
    do {
        LL_GPIO_SetOutputPin(DISP_CD_GPIO_Port, DISP_CD_Pin);
        LL_GPIO_ResetOutputPin(DISP_RD_GPIO_Port, DISP_RD_Pin);
        shortDelay();
        b = LL_GPIO_ReadInputPort(DISP_D0_GPIO_Port);
        LL_GPIO_SetOutputPin(DISP_RD_GPIO_Port, DISP_RD_Pin);
        shortDelay();
    } while ((b & bits) != bits);

    MODIFY_REG(DISP_D0_GPIO_Port->MODER, 0xFFFFU, 0b0101010101010101U);
}

static void checkBusy_S0S1() {
    waitForBits(0x3);
}

void checkBusy_S3() {
    waitForBits(0x8);
}


static void writeData(uint8_t dataByte) {
    LL_GPIO_ResetOutputPin(DISP_CD_GPIO_Port, DISP_CD_Pin);
    DISP_D0_GPIO_Port->ODR = dataByte;
    LL_GPIO_ResetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();
    LL_GPIO_SetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();
}

static void writeCommand0(uint8_t command) {

    checkBusy_S3();
    LL_GPIO_SetOutputPin(DISP_CD_GPIO_Port, DISP_CD_Pin);
    LL_GPIO_WriteOutputPort(DISP_D0_GPIO_Port, command);
    LL_GPIO_ResetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();
    LL_GPIO_SetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();

}

void writeCommand1(uint8_t command, u_int8_t param1) {
    checkBusy_S0S1();
    writeData(param1);
    writeCommand0(command);
}

void writeCommand2(uint8_t command, u_int8_t param1, u_int8_t param2) {
    checkBusy_S0S1();
    writeData(param1);
    writeCommand1(command, param2);
}

void Screen::bgBrightness(uint16_t brightness) {
    TIM22->CCR1 = brightness;
}

void inline Screen::copyPict(const uint8_t *background) {
    memcpy(screen_buffer, background, WIDTH_BYTES * HEIGHT);
}

void inline Screen::clearPict() {
    memset(screen_buffer, 0, HEIGHT * WIDTH_BYTES);
}

void inline Screen::copyPict(int xBytes, int y, int wBytes, int h, const uint8_t *pict) {
    for (int iy = 0; iy < h; iy++) {
        for (int ix = 0; ix < wBytes; ix++) {
            screen_buffer[(y + iy) * WIDTH_BYTES + xBytes + ix] = pict[iy * wBytes + ix];
        }
    }
}

void Screen::pixel(float fX, float fY, int color) {
    uint16_t x = lroundf(0.5f + fX);
    uint16_t y = lroundf(0.5f + fY);
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    uint8_t mask = 0x80U >> (x & 7U);
    uint8_t *addr = &screen_buffer[(x >> 3U) + (WIDTH_BYTES * y)];
    if (color) *addr |= mask; else *addr &= (uint8_t) ~mask;

}

void AffineTransform::pixel(float x, float y, int color) {
    float tX = 0.5f + m00 * x + m01 * y + m02;
    float tY = 0.5f + m10 * x + m11 * y + m12;
    screen.pixel(tX, tY, color);
}

void AffineTransform::reset() {
    m00 = m11 = 1;
    m10 = m01 = m02 = m12 = 0;
}

void AffineTransform::rotate(float theta) {
    float aSin = sinf(theta);
    float aCos = cosf(theta);
    float M0, M1;
    M0 = m00;
    M1 = m01;
    m00 = aCos * M0 + aSin * M1;
    m01 = -aSin * M0 + aCos * M1;
    M0 = m10;
    M1 = m11;
    m10 = aCos * M0 + aSin * M1;
    m11 = -aSin * M0 + aCos * M1;
}

void AffineTransform::translate(float tx, float ty) {
    m02 = tx * m00 + ty * m01 + m02;
    m12 = tx * m10 + ty * m11 + m12;
}

void AffineTransform::rotate(float theta, float anchorx, float anchory) {
    translate(anchorx, anchory);
    rotate(theta);
    translate(-anchorx, -anchory);
}

void AffineTransform::line(float x1, float y1, float x2, float y2, float width, const char *pattern) {
    float tX1 = (int) (0.5f + m00 * x1 + m01 * y1 + m02);
    float tY1 = (int) (0.5f + m10 * x1 + m11 * y1 + m12);
    float tX2 = (int) (0.5f + m00 * x2 + m01 * y2 + m02);
    float tY2 = (int) (0.5f + m10 * x2 + m11 * y2 + m12);
    screen.line(tX1, tY1, tX2, tY2, width, pattern);
}

AffineTransform::AffineTransform(class Screen &ascreen) : screen(ascreen) {
    reset();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

inline void swap(int &a, int &b) {
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
}

#pragma clang diagnostic pop

void Screen::line(float fX1, float fY1, float fX2, float fY2, float fWidth, const char *pattern) {
    int x1 = lroundf(fX1);
    int x2 = lroundf(fX2);

    int y1 = lroundf(fY1);
    int y2 = lroundf(fY2);
    int width = lroundf(fWidth);

    bool steep = labs(y2 - y1) > labs(x2 - x1);
    if (steep) {
        swap(x1, y1);
        swap(x2, y2);
    }
    int deltax = labs(x2 - x1);
    int deltay = labs(y2 - y1);
    int err = deltax / 2;
    int y = y1;

    int inc = (x1 < x2) ? 1 : -1;
    int ystep = (y1 < y2) ? 1 : -1;
    const char *patternChar = pattern;
    for (int x = x1; (x2 - x) * inc >= 0; x += inc) {

        if (*patternChar == 0) {
            patternChar = pattern;
        }
        int color = *(patternChar++) & 1u;
        for (int d = 0; d < width; d++) {
            int dy = d - width / 2;
            if (steep) pixel(y + dy, x, color); else pixel(x, y + dy, color);
        }
        err -= deltay;
        if (err < 0) {
            y += ystep;
            err += deltax;
        }
    }
}

