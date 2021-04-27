//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <deck-module.hpp>
#include <cmath>

/* LCM commands */
#define LCM_SET_TEXT_HOME 0x40
#define LCM_SET_TEXT_COLS 0x41

#define LCM_SET_GRAPH_HOME 0x42
#define LCM_SET_GRAPH_COLS 0x43

#define MAX_LINE_POINTS 200

MaskedDisplay maskedDisplay = MaskedDisplay();
AffineTransform affineScreen = AffineTransform(maskedDisplay);

AngleCorrectScreen angleCorrectScreen = AngleCorrectScreen();

Screen *Screen::activeScreen = &mainScreen;

static uint8_t screen_buffer[SCREEN_WIDTH_BYTES * SCREEN_HEIGHT];
static uint8_t brightness  = Display::BG_MIN;

static const uint8_t SPLASH_PICTURE[] = {
#include "bitmaps/splash.txt"
};

const uint8_t FONT[] = {
#include "bitmaps/font.txt"
};

const uint8_t FONT40x48[] = {
#include "bitmaps/font40x48.txt"
};

void initLcd(void) {
    LL_USART_Enable(UART4);
    Display::clearPict();
    Display::bgBrightness(Display::BG_MIN);
    Display::paint();
}

void splashLcd(void) {
    for (int i = 0; i < (2 * SCREEN_WIDTH_BYTES) + 1; i++) {
        Display::copyPict(SPLASH_PICTURE);
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            int k = -j / 5 + i;
            if (k < 0) k = 0;
            for (; k < SCREEN_WIDTH_BYTES; k++) {
                screen_buffer[j * SCREEN_WIDTH_BYTES + k] = 0;
            }
        }
        Display::paint();
        msDelay(1);
        LL_IWDG_ReloadCounter(IWDG);
    }
    msDelay(70);
}

void updateLcd(uint_fast64_t timeStamp) {
    static uint_fast64_t lastUpdate = 0;
    if (timeStamp - lastUpdate < 50) return;
    lastUpdate = timeStamp;
    Display::clearPict();
    Screen::activeScreen->updatePicture();
    Display::paint();
}

static inline void transmitByte(uint8_t b) {
    while(!LL_USART_IsActiveFlag_TXE(UART4)) {}
    LL_USART_TransmitData8(UART4, b);
}

void Display::paint() {
    transmitByte(0xAA);
    transmitByte(0xA0);
    transmitByte(brightness);
    for (int y = 127; y >= 0; y--) {
        for (int i = 0; i < 30; i++) {
            uint8_t byte = screen_buffer[y * 30 + i];
            transmitByte(byte);
            if (byte == 0xAA) {//escaping
                transmitByte(0xA9);
            }
        }
    }
}

/***************************/
/* Low Level functions     */
/***************************/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"

void static inline __attribute__((optimize("O0"))) shortDelay() {
    for (int i = 0; i < 10; i++) {
        __NOP();
    }
}

#pragma clang diagnostic pop


void Display::bgBrightness(uint16_t _brightness) {
    brightness = _brightness;
}

void Display::copyPict(const uint8_t *background) {
    memcpy(screen_buffer, background, SCREEN_WIDTH_BYTES * SCREEN_HEIGHT);
}

void Display::clearPict() {
    memset(screen_buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH_BYTES);
}

void Display::copyPict(int xBytes, int y, int wBytes, int h, const uint8_t *pict) {
    copyPict(xBytes, y, wBytes, h, 0, pict);
}

void Display::copyPict(int xBytes, int y, int wBytes, int h, const uint8_t xorMask, const uint8_t *pict) {
    for (int iy = 0; iy < h; iy++) {
        for (int ix = 0; ix < wBytes; ix++) {
            screen_buffer[(y + iy) * SCREEN_WIDTH_BYTES + xBytes + ix] |= pict[iy * wBytes + ix] ^ xorMask;
        }
    }
}

void Screen::chessBox(int xBytes, int y, int wBytes, int h) {
    for (int iy = 0; iy < h; iy += 2) {
        for (int ix = 0; ix < wBytes; ix++) {
            int idx = (y + iy) * SCREEN_WIDTH_BYTES + xBytes + ix;
            screen_buffer[idx] |= 0x55;
            screen_buffer[idx + SCREEN_WIDTH_BYTES] |= 0xAA;
        }
    }
}

void Screen::invertBox(int xBytes, int y, int wBytes, int h) {
    for (int iy = 0; iy < h; iy++) {
        for (int ix = 0; ix < wBytes; ix++) {
            int idx = (y + iy) * SCREEN_WIDTH_BYTES + xBytes + ix;
            screen_buffer[idx] = ~screen_buffer[idx];
        }
    }
}

void Display::pixel(float fX, float fY, int color) {
    uint16_t x = lroundf(0.5F + fX);
    uint16_t y = lroundf(0.5F + fY);
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint8_t mask = 0x80U >> (x & 7U);
    uint8_t *addr = &screen_buffer[(x >> 3U) + (SCREEN_WIDTH_BYTES * y)];
    if (color) *addr |= mask; else *addr &= (uint8_t) ~mask;

}

void AffineTransform::pixel(float x, float y, int color) {
    float tX = 0.5F + m00 * x + m01 * y + m02;
    float tY = 0.5F + m10 * x + m11 * y + m12;
    screen.pixel(tX, tY, color);
}

void AffineTransform::reset() {
    m00 = m11 = 1;
    m10 = m01 = m02 = m12 = 0;
}

void AffineTransform::rotate(float theta) {
    float aSin = sinf(theta);
    float aCos = cosf(theta);
    float M0;
    float M1;
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

AffineTransform::AffineTransform(class Display &ascreen) : screen(ascreen) {
    reset();
}

void AffineTransform::scale(float sx, float sy) {
    m00 *= sx;
    m11 *= sy;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

inline void swap(int &a, int &b) {
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
}

#pragma clang diagnostic pop

void Display::line(float fX1, float fY1, float fX2, float fY2, float fWidth, const char *pattern) {
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

    int pointCount = MAX_LINE_POINTS;
    for (int x = x1; ((x2 - x) * inc >= 0) &&(pointCount); x += inc,pointCount--) {

        if (*patternChar == 0) {
            patternChar = pattern;
        }
        unsigned int color = *(patternChar++) & 1u;
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

void Display::savePictTo(uint8_t saveScreenBuffer[SCREEN_WIDTH_BYTES * SCREEN_HEIGHT]) {
    memcpy(saveScreenBuffer, screen_buffer, sizeof(screen_buffer));
}

void MaskedDisplay::pixel(float x, float y, int color) {
    if ((x < 39) || (x > 87) || (y > 70) || (y < 37)) {
        Display::pixel(x, y, color);
    }
}

void processKeyboard() {
    if (state.waitUntilKeysReleased) {
        if (!(state.keyUp || state.keyDown || state.keyCancel || state.keyOk)) {
            state.waitUntilKeysReleased = false;
        }
        return;
    }
    Screen::activeScreen->processKeyboard();
}

void SettingsScreen::changeValue(int delta, unsigned int *changeableValue, unsigned int max, unsigned int min,
                                 int activePosition) const {
    int digitDivider;
    switch (activePosition) {
        case 0:
            digitDivider = 100;
            break;
        case 1:
            digitDivider = 10;
            break;
        case 2:
            digitDivider = 1;
            break;
        default:
            return;
    }
    int digitValue = (*changeableValue / digitDivider) % 10;
    int newDigitValue = (digitValue + delta + 10) % 10;
    int correctionValue = (newDigitValue - digitValue) * digitDivider;
    *changeableValue += correctionValue;
    if (*changeableValue >= max) *changeableValue = min;
    else
        while (*changeableValue < min) *changeableValue += max;
}

void Screen::draw3digits(bool big, unsigned int val, unsigned int xBytes, unsigned int y, int activepos) {
    char char1 = val / 100;
    char char2 = val / 10 % 10;
    char char3 = val % 10;
    if (big) {
        bigCharOutput(char1, xBytes, y, activepos == 0 ? 0xFF : 0);
        bigCharOutput(char2, xBytes + 5, y, activepos == 1 ? 0xFF : 0);
        bigCharOutput(char3, xBytes + 10, y, activepos == 2 ? 0xFF : 0);
    } else {
        charOutput(char1, xBytes, y, activepos == 0 ? 0xFF : 0);
        charOutput(char2, xBytes + 4, y, activepos == 1 ? 0xFF : 0);
        charOutput(char3, xBytes + 8, y, activepos == 2 ? 0xFF : 0);
    }
}