//
// Created by Ilia.Motornyi on 14/04/2019.
//

#include <memory.h>
#include "screen.hpp"


#define WIDTH 240
#define WIDTH_BYTES (WIDTH / 8)
#define HEIGHT 128

/* LCM commands */
#define LCM_SET_TEXT_HOME 0x40
#define LCM_SET_TEXT_COLS 0x41

#define LCM_SET_GRAPH_HOME 0x42
#define LCM_SET_GRAPH_COLS 0x43

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
        LL_mDelay(5);
    }
    LL_mDelay(700);
}

void updateLcd(void) {
    Screen::clearPict();
    Screen::copyPict(0, 0, 16, 128, LEFT_BACKGROUND);
    Screen::displayScreen();
    TIM22->CCR1 = (TIM22->CCR1 + 37) % 99; //todo
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
