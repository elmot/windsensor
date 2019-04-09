//----------------------------------------------------------------------
/*EASTRISING TECHNOLOGY CO,.LTD.*/
// Module    : ERM240128-2 Series
// Drive IC  : T6963
// INTERFACE : 8080_8bit
// VDD		 : 5V
//***************************************************************************

#include <math.h>
#include <stdlib.h>
#include "main.h"

/********************DEFINE IO********************/

//TP CHIP IO
#define uchar  unsigned char
#define uint  unsigned int
#define Graphic        1
#define    Character    0

unsigned char const BMP0[];
unsigned char const Chinese_character[];

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

void Locatexy(uchar xa, uchar ya, uchar mode);

void CheckBusy_S0S1();

void CheckBusy_S3();

void WriteCommand(uchar dat1, uchar dat2, uchar command, uchar parameters_num);

void WriteData(uchar DataByte);

void LcmClear(void);

void Display_Str(uchar x, uchar y, char *ascll);

void Display_16_16(uchar x, uchar y, uchar const *Chinese_character, uchar character_code);

void DisplayDots(uchar DotByte, uchar DotByte1);

void LcmInit(void);

void DisplayBMP(uchar x, uchar y, uchar W, uchar H, uchar const *puts);

void ReverseDisplayBMP(uchar x, uchar y, uchar W, uchar H, uchar const *puts);

void Delay(uint MS);

/************************LCM*************************/
/********************************************************/
// LOCATE X AND Y OF LCM
/********************************************************/
void Locatexy(uchar xa, uchar ya, uchar mode) {
    uint a = 0;
    if (mode)                //Graphic mode   x=0-29  y=0-127    START ADDRESS £º0800
    {
        a = ya * 30 + xa;
        WriteCommand((uchar) a, (a >> 8u) + 0x08u, 0x24u, 2u);
    } else                    //Character mode   x=0-29  y=0-15    START ADDRESS £º0000
    {
        a = ya * 30 + xa;
        WriteCommand((uchar) (a), (a >> 8u), 0x24, 2);
    }

}

/***************************/
/*LCM READ AND WRITE BUSY CHECK*/
/***************************/
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

void CheckBusy_S0S1() {
    waitForBits(0x3);
}

/***************************/
/*DATA AUTO READ BUSY CHECK*/
/***************************/
/*
void CheckBusy_S2()
{uchar b;
    do
    {DataBus=0xff;
        CD=1;
        _RD=0;
        b=DataBus;
        _RD=1;
    }
    while(b&0x03==0);

}
*/

/***************************/
/*DATA AUTO WRITE BUSY CHECK*/
/***************************/

void CheckBusy_S3() {
    waitForBits(0x8);
}



/***************************/
/*WRITE COMMAND             */
/***************************/
void WriteCommand(uchar dat1, uchar dat2, uchar command, uchar parameters_num) {
    if ((parameters_num == 1) || (parameters_num == 2)) {
        CheckBusy_S0S1();
        WriteData(dat1);
    }
    if (parameters_num == 2) {
        CheckBusy_S0S1();
        WriteData(dat2);
    }

    CheckBusy_S3();
    LL_GPIO_SetOutputPin(DISP_CD_GPIO_Port, DISP_CD_Pin);
    LL_GPIO_WriteOutputPort(DISP_D0_GPIO_Port, command);
    LL_GPIO_ResetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();
    LL_GPIO_SetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();

}


/***************************/
/*WRITE DATA                 */
/***************************/
void WriteData(uchar DataByte) {
    LL_GPIO_ResetOutputPin(DISP_CD_GPIO_Port, DISP_CD_Pin);
    DISP_D0_GPIO_Port->ODR = DataByte;
    LL_GPIO_ResetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();
    LL_GPIO_SetOutputPin(DISP_WR_GPIO_Port, DISP_WR_Pin);
    shortDelay();
}

void LcmClear(void) {
    uint a;
    Locatexy(0, 0, Character);
    WriteCommand(0, 0, 0xb0, 0);
    for (a = 0; a < 8192; a++) {
        WriteData(0);
    }
    WriteCommand(0, 0, 0xb2, 0);
}

void Display_Str(uchar x, uchar y, char *ascll) {
    Locatexy(x, y, Character);
    while (*ascll > 0) {
        WriteCommand(*ascll - 0x20, 0, 0xc0, 1);
        ascll++;
    }
}


void Display_16_16(uchar x, uchar y, uchar const *character, uchar character_code) {
    uchar k, j;
    uint num = 0;
    num = character_code * 0x20;
    for (j = 0; j < 16; j++) {
        Locatexy(x, y, Graphic);
        for (k = 0; k < 2; k++) {
            WriteCommand(*(character + num), 0, 0xc0, 1);
            num++;
        }
        y += 1;
    }

}

void DisplayDots(uchar DotByte, uchar DotByte1) {
    uchar k, j;
    Locatexy(0, 0, Graphic);
    WriteCommand(0, 0, 0xb0, 0);
    for (j = 0; j < 64; j++) {
        for (k = 0; k < 30; k++) {
            WriteData(DotByte);

        }
        for (k = 0; k < 30; k++) {
            WriteData(DotByte1);

        }
    }
    WriteCommand(0, 0, 0xb2, 0);
}


void DisplayBMP(uchar x, uchar y, uchar W, uchar H, uchar const *puts) {
    uchar k, j;

    for (j = 0; j < H; j++) {
        Locatexy(x, y, Graphic);
        WriteCommand(0, 0, 0xb0, 0);
        for (k = 0; k < W / 8; k++) {
            WriteData(*puts++);

        }
        WriteCommand(0, 0, 0xb2, 0);
        y += 1;
    }
}

void ReverseDisplayBMP(uchar x, uchar y, uchar W, uchar H, uchar const *puts) {
    uchar k, j;

    for (j = 0; j < H; j++) {
        Locatexy(x, y, Graphic);
        WriteCommand(0, 0, 0xb0, 0);
        for (k = 0; k < W / 8; k++) {
            WriteData(~(*puts));
            puts++;
        }
        WriteCommand(0, 0, 0xb2, 0);
        y += 1;
    }
}


void Delay(uint MS) {
    LL_mDelay(MS);
}


void LcmInit(void) {
    LL_GPIO_ResetOutputPin(DISP_RESET_GPIO_Port, DISP_RESET_Pin);
    Delay(50);
    LL_GPIO_SetOutputPin(DISP_RESET_GPIO_Port, DISP_RESET_Pin);
    Delay(50);
    WriteCommand(0, 0, 0x40, 2);//set text home address to 0
    WriteCommand(30, 0, 0x41, 2);//set text area to 30 columns
    WriteCommand(0, 0x08, 0x42, 2);//set graphics home address to 0x800(?)
    WriteCommand(30, 0, 0x43, 2);//set graphics area to 30 columns
    WriteCommand(0, 0, 0xa7, 0);//cursor pattern 8 lines
    WriteCommand(0, 0, 0x80, 0);//Internal GC rom mode, OR mode
    WriteCommand(0, 0, 0x9C, 0);//text on, graphics on
}


void mainLcd(void) {

    LcmInit();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        TIM22->CCR1 = (TIM22->CCR1 + 37) % 99;
        TIM22->CNT = 1;
        LcmClear();
        Display_16_16(0, 0, Chinese_character, 0);
        Display_16_16(2, 0, Chinese_character, 1);
        Display_16_16(4, 0, Chinese_character, 2);
        Display_16_16(6, 0, Chinese_character, 3);
        Display_16_16(8, 0, Chinese_character, 4);
        Display_16_16(10, 0, Chinese_character, 5);
        Display_16_16(12, 0, Chinese_character, 6);
        Display_16_16(14, 0, Chinese_character, 7);
        Display_16_16(16, 0, Chinese_character, 8);
        Display_16_16(18, 0, Chinese_character, 9);
        Display_16_16(20, 0, Chinese_character, 10);
        Display_16_16(22, 0, Chinese_character, 11);
        Display_16_16(24, 0, Chinese_character, 12);


        Display_Str(0, 3, "EAST RISING TOUCH TEST PROGRAM");
        Display_Str(7, 5, "PLEASE TOUCH ME!");
        Display_Str(0, 7, "X:COORDINATES:");
        Display_Str(0, 9, "Y:COORDINATES:");

        Display_Str(0, 15, "EXIT ");

        char buf[100] = "Sin(2) = ";
        double v = sin(2);
        sprintf(buf,"Sin(2) = %06.2f",v);
        Display_Str(0, 11, buf);

        Delay(1000);

        LcmClear();
        DisplayDots(0x55, 0xaa);
        Delay(1000);
        DisplayDots(0xaa, 0x55);
        Delay(1000);

        DisplayDots(0x55, 0x55);
        Delay(1000);
        DisplayDots(0xaa, 0xaa);
        Delay(1000);

        DisplayDots(0xff, 0x00);
        Delay(1000);
        DisplayDots(0x00, 0xff);
        Delay(1000);
        DisplayBMP(0, 0, 240, 128, BMP0);
        Delay(1500);
        ReverseDisplayBMP(0, 0, 240, 128, BMP0);
        Delay(1500);


    }
#pragma clang diagnostic pop
}


unsigned char const Chinese_character[] =
        {/*--  ÎÄ×Ö:  »¶  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x00, 0x80, 0x00, 0x80, 0xFC, 0x80, 0x05, 0xFE, 0x85, 0x04, 0x4A, 0x48, 0x28, 0x40, 0x10, 0x40,
                0x18, 0x40, 0x18, 0x60, 0x24, 0xA0, 0x24, 0x90, 0x41, 0x18, 0x86, 0x0E, 0x38, 0x04, 0x00, 0x00,

/*--  ÎÄ×Ö:  Ó­  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x40, 0x00, 0x21, 0x80, 0x36, 0x7C, 0x24, 0x44, 0x04, 0x44, 0x04, 0x44, 0xE4, 0x44, 0x24, 0x44,
                0x25, 0x44, 0x26, 0x54, 0x24, 0x48, 0x20, 0x40, 0x20, 0x40, 0x50, 0x00, 0x8F, 0xFE, 0x00, 0x00,

/*--  ÎÄ×Ö:  Ê¹  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x08, 0x40, 0x0C, 0x40, 0x1B, 0xFE, 0x10, 0x40, 0x37, 0xFC, 0x64, 0x44, 0xA4, 0x44, 0x27, 0xFC,
                0x24, 0x44, 0x22, 0x40, 0x21, 0x80, 0x20, 0x80, 0x21, 0x70, 0x22, 0x1E, 0x2C, 0x04, 0x00, 0x00,

/*--  ÎÄ×Ö:  ÓÃ  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x00, 0x00, 0x1F, 0xFC, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x1F, 0xFC, 0x10, 0x84, 0x10, 0x84,
                0x10, 0x84, 0x1F, 0xFC, 0x10, 0x84, 0x10, 0x84, 0x20, 0x84, 0x20, 0x84, 0x40, 0x94, 0x80, 0x88,

/*--  ÎÄ×Ö:  Ðñ  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x10, 0x00, 0x10, 0xFC, 0x10, 0x84, 0x10, 0x84, 0xFC, 0x84, 0x14, 0xFC, 0x14, 0x84, 0x14, 0x84,
                0x14, 0x84, 0x24, 0x84, 0x24, 0xFC, 0x24, 0x00, 0x44, 0x02, 0x44, 0x02, 0x83, 0xFE, 0x00, 0x00,

/*--  ÎÄ×Ö:  ÈÕ  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x00, 0x00, 0x1F, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0xF0, 0x10, 0x10,
                0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0xF0, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,

/*--  ÎÄ×Ö:  ¶«  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x02, 0x00, 0x02, 0x00, 0x7F, 0xFC, 0x04, 0x00, 0x08, 0x80, 0x08, 0x80, 0x10, 0x80, 0x1F, 0xF8,
                0x00, 0x80, 0x08, 0xA0, 0x0C, 0x90, 0x18, 0x88, 0x10, 0x8C, 0x22, 0x84, 0x01, 0x80, 0x00, 0x80,

/*--  ÎÄ×Ö:  ·½  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x02, 0x00, 0x01, 0x00, 0x00, 0x80, 0xFF, 0xFE, 0x02, 0x00, 0x02, 0x00, 0x03, 0xF0, 0x02, 0x10,
                0x04, 0x10, 0x04, 0x10, 0x08, 0x10, 0x08, 0x10, 0x10, 0x10, 0x20, 0x90, 0xC0, 0x60, 0x00, 0x00,

/*--  ÎÄ×Ö:  ´¥  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x20, 0x10, 0x3F, 0x10, 0x21, 0x10, 0x42, 0x10, 0x7F, 0x7C, 0xC9, 0x54, 0x7F, 0x54, 0x49, 0x54,
                0x49, 0x54, 0x7F, 0x7C, 0x49, 0x10, 0x49, 0x14, 0x49, 0x12, 0x49, 0x1E, 0x85, 0xF2, 0x02, 0x00,

/*--  ÎÄ×Ö:  Ãþ  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x11, 0x10, 0x11, 0x10, 0x17, 0xFE, 0xF9, 0x10, 0x13, 0xF8, 0x12, 0x08, 0x1B, 0xF8, 0x32, 0x08,
                0xD3, 0xF8, 0x10, 0x40, 0x17, 0xFE, 0x10, 0x40, 0x10, 0xA0, 0x10, 0x98, 0x51, 0x0E, 0x26, 0x04,

/*--  ÎÄ×Ö:  ÆÁ  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x3F, 0xFC, 0x20, 0x04, 0x20, 0x04, 0x3F, 0xFC, 0x24, 0x10, 0x22, 0x20, 0x2F, 0xFC, 0x22, 0x20,
                0x22, 0x20, 0x3F, 0xFE, 0x22, 0x20, 0x22, 0x20, 0x44, 0x20, 0x44, 0x20, 0x88, 0x20, 0x10, 0x20,

/*--  ÎÄ×Ö:  Ä£  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x20, 0x90, 0x20, 0x90, 0x27, 0xFC, 0x20, 0x90, 0xFB, 0xF8, 0x22, 0x08, 0x73, 0xF8, 0x6A, 0x08,
                0xA3, 0xF8, 0xA0, 0x40, 0x27, 0xFE, 0x20, 0x40, 0x20, 0xA0, 0x20, 0x98, 0x21, 0x0E, 0x26, 0x04,

/*--  ÎÄ×Ö:  ¿é  --*/
/*--  ËÎÌå12;  ´Ë×ÖÌåÏÂ¶ÔÓ¦µÄµãÕóÎª£º¿íx¸ß=16x16   --*/
                0x20, 0x80, 0x20, 0x80, 0x20, 0x80, 0x23, 0xF8, 0xF8, 0x88, 0x20, 0x88, 0x20, 0x88, 0x20, 0x88,
                0x2F, 0xFE, 0x20, 0x80, 0x39, 0x40, 0xE1, 0x20, 0x42, 0x18, 0x02, 0x0E, 0x04, 0x04, 0x08, 0x00,


        };


unsigned char const BMP0[] = {

/*--240x128  --*/
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x83, 0xF0, 0xE0, 0xF9, 0xFE, 0xFC, 0x63, 0xE3, 0x31, 0x87, 0xC0, 0xFF, 0x7E, 0x1F,
        0x31, 0x98, 0xC3, 0xC3, 0x00, 0xF0, 0x3E, 0x61, 0x80, 0xF8, 0x78, 0x00, 0x60, 0xFF, 0x7C, 0x01,
        0x83, 0x00, 0xE1, 0x8C, 0x30, 0xC6, 0x66, 0x33, 0x39, 0x8C, 0x60, 0x18, 0x60, 0x31, 0xB1, 0x9C,
        0xC6, 0x63, 0x01, 0x98, 0x63, 0x61, 0x81, 0x8C, 0xCC, 0x00, 0x60, 0x18, 0x66, 0x01, 0x83, 0x01,
        0xB1, 0x80, 0x30, 0xC6, 0x66, 0x03, 0x39, 0x98, 0x00, 0x18, 0x60, 0x60, 0x31, 0x9C, 0xCC, 0x33,
        0x03, 0x0C, 0xC0, 0x33, 0x03, 0x01, 0x86, 0x00, 0x60, 0x18, 0x63, 0x01, 0x83, 0x01, 0xB1, 0xC0,
        0x30, 0xC6, 0x67, 0x03, 0x3D, 0x98, 0x00, 0x18, 0x60, 0x60, 0x31, 0x9E, 0xCC, 0x33, 0x03, 0x0C,
        0xC0, 0x1E, 0x03, 0x01, 0x86, 0x00, 0x60, 0x18, 0x63, 0x01, 0x83, 0xF1, 0xB0, 0xF0, 0x30, 0xC6,
        0x63, 0xC3, 0x35, 0x98, 0x00, 0x18, 0x7E, 0x60, 0x3F, 0x9A, 0xCC, 0x33, 0x03, 0x0C, 0xC0, 0x1E,
        0x03, 0x01, 0x86, 0x00, 0x60, 0x18, 0x63, 0x01, 0x83, 0x03, 0x18, 0x78, 0x30, 0xFC, 0x61, 0xE3,
        0x37, 0x99, 0xE0, 0x18, 0x60, 0x60, 0x31, 0x9B, 0xCC, 0x33, 0x03, 0x0C, 0xCF, 0x0C, 0x03, 0x01,
        0x86, 0x00, 0x60, 0x18, 0x63, 0x01, 0x83, 0x03, 0x18, 0x1C, 0x30, 0xCC, 0x60, 0x73, 0x33, 0x98,
        0x60, 0x18, 0x60, 0x60, 0x31, 0x99, 0xCC, 0x33, 0x03, 0x0C, 0xC3, 0x0C, 0x03, 0x01, 0x86, 0x00,
        0x60, 0x18, 0x63, 0x01, 0x83, 0x03, 0xF8, 0x0C, 0x30, 0xC6, 0x60, 0x33, 0x33, 0x98, 0x60, 0x18,
        0x60, 0x60, 0x31, 0x99, 0xCC, 0x33, 0x03, 0x0C, 0xC3, 0x0C, 0x03, 0x01, 0x86, 0x00, 0x60, 0x18,
        0x63, 0x01, 0x83, 0x06, 0x0D, 0x8C, 0x30, 0xC6, 0x66, 0x33, 0x31, 0x8C, 0x60, 0x18, 0x60, 0x31,
        0xB1, 0x98, 0xC6, 0x63, 0x01, 0x98, 0x63, 0x0C, 0x01, 0x8C, 0xCC, 0x66, 0x60, 0x18, 0x66, 0x31,
        0x83, 0xF6, 0x0C, 0xF8, 0x30, 0xC3, 0x63, 0xE3, 0x31, 0x87, 0xC0, 0x18, 0x7E, 0x1F, 0x31, 0x98,
        0xC3, 0xC3, 0xF0, 0xF0, 0x3E, 0x0C, 0x00, 0xF8, 0x78, 0x66, 0x7E, 0x18, 0x7C, 0x31, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x40, 0x00, 0x22, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x00,
        0xC1, 0x80, 0x18, 0x60, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x8C, 0xFF,
        0xC0, 0x33, 0x6C, 0x01, 0x0F, 0x80, 0x3F, 0xF0, 0x00, 0x30, 0x00, 0x03, 0xE0, 0x07, 0x89, 0x80,
        0x18, 0x60, 0x00, 0x30, 0x01, 0xFB, 0xFC, 0x00, 0x4C, 0x00, 0x3F, 0xFD, 0x8E, 0x92, 0x40, 0x33,
        0x64, 0x0F, 0xF8, 0xC0, 0x10, 0x30, 0x07, 0xFF, 0xC0, 0xE0, 0x3F, 0x81, 0x8D, 0x80, 0x59, 0xFC,
        0x1F, 0xFF, 0xF0, 0xD9, 0x0C, 0x01, 0x87, 0x00, 0x40, 0x0D, 0x83, 0xA3, 0x40, 0xF3, 0x64, 0x09,
        0x48, 0x80, 0x10, 0x30, 0x00, 0x70, 0x01, 0xFF, 0xFF, 0x0F, 0xE3, 0x80, 0xFE, 0x60, 0x10, 0x70,
        0x70, 0xD1, 0xFC, 0x0F, 0x03, 0xC1, 0xFF, 0xFD, 0xB8, 0x0C, 0x01, 0xB3, 0x64, 0x03, 0x4F, 0x80,
        0x10, 0x30, 0x01, 0x98, 0x00, 0x03, 0x02, 0x01, 0x9D, 0x80, 0x19, 0xF0, 0x00, 0xE3, 0x00, 0xF1,
        0x0C, 0x1E, 0x31, 0xF0, 0x00, 0x0D, 0xBE, 0x8C, 0x00, 0x33, 0x64, 0x03, 0x48, 0x80, 0x1F, 0xF0,
        0x03, 0xFF, 0x80, 0x03, 0xF8, 0x03, 0xC1, 0x80, 0x3C, 0x18, 0x03, 0xFF, 0x80, 0xC9, 0xFC, 0x08,
        0x30, 0xF0, 0x7F, 0xCD, 0x81, 0xFF, 0xF0, 0x3F, 0x64, 0x02, 0x4F, 0x80, 0x1C, 0x70, 0x00, 0x98,
        0x00, 0x06, 0x38, 0x07, 0xDF, 0xF8, 0xD9, 0x90, 0x1F, 0x7F, 0x80, 0xD9, 0x08, 0x00, 0x60, 0x00,
        0x60, 0xCD, 0x82, 0x2C, 0x60, 0xFA, 0x64, 0x06, 0x4F, 0x80, 0x10, 0x30, 0x00, 0xDB, 0x00, 0x0C,
        0x10, 0x0D, 0xB9, 0xB0, 0x18, 0x70, 0x1C, 0x61, 0x00, 0xF1, 0xF0, 0x00, 0xCE, 0x00, 0x60, 0xCD,
        0x8C, 0x7D, 0x81, 0xC6, 0x66, 0x1C, 0x70, 0x00, 0x10, 0x30, 0x01, 0x99, 0x80, 0x18, 0x30, 0x39,
        0x81, 0x80, 0x18, 0x78, 0x00, 0x7F, 0x00, 0xC1, 0x18, 0x03, 0x87, 0x00, 0x3F, 0xCD, 0x9C, 0x4C,
        0xC0, 0x1E, 0x66, 0x18, 0x3F, 0xF0, 0x1F, 0xF0, 0x06, 0x98, 0xE0, 0x70, 0x30, 0x01, 0x81, 0x80,
        0x1F, 0x8F, 0x80, 0xC1, 0x00, 0xC3, 0x6F, 0x87, 0x8D, 0x80, 0x00, 0x0D, 0x98, 0x8C, 0x60, 0x18,
        0x46, 0x00, 0x1F, 0xF0, 0x1F, 0xF0, 0x18, 0x78, 0x31, 0xE6, 0xE0, 0x01, 0x81, 0x81, 0xF6, 0x07,
        0x03, 0xC1, 0x80, 0xC3, 0xC7, 0x00, 0x78, 0x00, 0x01, 0xFD, 0x80, 0x0C, 0x00, 0x00, 0x04, 0x00,
        0x03, 0xE0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0xC0, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x7F, 0xC1, 0xF0, 0x7C, 0x3F, 0x00, 0x70, 0x1E,
        0x00, 0x70, 0x3F, 0x03, 0xF0, 0x7F, 0xE0, 0x7C, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x7F, 0xE1, 0xF8, 0xFC, 0x7F, 0x80, 0xF0, 0x7F, 0x80, 0x70,
        0x7F, 0x87, 0xF8, 0x7F, 0xE1, 0xFF, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x1F, 0xFC, 0x7F, 0xF1, 0xF8, 0xFC, 0xF3, 0xC0, 0xF0, 0x73, 0x80, 0xF0, 0xF3, 0xCF,
        0x3C, 0x7F, 0xE3, 0xFF, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x1C, 0x00, 0x70, 0x71, 0xD8, 0xDC, 0xE1, 0xC1, 0xF0, 0xE1, 0xC3, 0xF0, 0xE1, 0xCE, 0x1C, 0x70,
        0x03, 0x87, 0x80, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1C, 0x00,
        0x70, 0x71, 0xD8, 0xDC, 0x01, 0xC1, 0xF0, 0xE1, 0xC7, 0xF0, 0x01, 0xCE, 0x1C, 0x70, 0x03, 0x83,
        0x80, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x70, 0x71,
        0xD8, 0xDC, 0x01, 0xC3, 0x70, 0xE1, 0xC7, 0x70, 0x01, 0xC7, 0x38, 0x70, 0x03, 0xC0, 0x00, 0x1D,
        0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x7F, 0xF1, 0xDD, 0xDC,
        0x03, 0x86, 0x70, 0xE1, 0xC4, 0x70, 0x03, 0x83, 0xF0, 0x7F, 0xC1, 0xF8, 0x00, 0x11, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x7F, 0xE1, 0xDD, 0xDC, 0x07, 0x86,
        0x70, 0xE1, 0xC0, 0x70, 0x07, 0x83, 0xF0, 0x7F, 0xC0, 0xFF, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x7F, 0x81, 0xCD, 0x9C, 0x0F, 0x0C, 0x70, 0xE1,
        0xC0, 0x70, 0x0F, 0x07, 0x38, 0x7F, 0xC0, 0x3F, 0x1F, 0x81, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x71, 0xC1, 0xCD, 0x9C, 0x1E, 0x18, 0x70, 0xE1, 0xC0, 0x70,
        0x1E, 0x0E, 0x1C, 0x70, 0x00, 0x07, 0x9F, 0x81, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x1C, 0x00, 0x71, 0xE1, 0xCF, 0x9C, 0x3C, 0x1F, 0xFC, 0xE1, 0xC0, 0x70, 0x3C, 0x0E,
        0x1C, 0x70, 0x07, 0x03, 0x9F, 0x81, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x1C, 0x00, 0x70, 0xE1, 0xCF, 0x9C, 0x38, 0x1F, 0xFC, 0xE1, 0xC0, 0x70, 0x38, 0x0E, 0x1C, 0x70,
        0x07, 0x87, 0x80, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xFC,
        0x70, 0x71, 0xCF, 0x9C, 0x70, 0x00, 0x70, 0x73, 0x80, 0x70, 0x70, 0x0F, 0x3C, 0x70, 0x03, 0xFF,
        0x80, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x70, 0x71,
        0xC7, 0x1C, 0xFF, 0xC0, 0x70, 0x7F, 0x80, 0x70, 0xFF, 0xC7, 0xF8, 0x70, 0x03, 0xFF, 0x00, 0x01,
        0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x70, 0x39, 0xC7, 0x1C,
        0xFF, 0xC0, 0x70, 0x1E, 0x00, 0x70, 0xFF, 0xC3, 0xF0, 0x70, 0x00, 0xFC, 0x00, 0x01, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0xC7, 0xCF, 0xE8, 0x20, 0x78, 0x11, 0xE4, 0x11, 0x0F, 0x1E, 0x00, 0x04, 0x00, 0x02, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
        0x21, 0x0C, 0x20, 0x84, 0x32, 0x12, 0x23, 0x10, 0xA1, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x21, 0x0A,
        0x20, 0x04, 0x32, 0x12, 0x25, 0x00, 0xA1, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x01, 0x0A, 0x20, 0x04,
        0x52, 0x11, 0x41, 0x00, 0xA1, 0x02, 0xC4, 0x88, 0xE2, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x87, 0x01, 0x09, 0x20, 0x04, 0x52, 0x10,
        0x81, 0x00, 0x9E, 0x03, 0x24, 0x51, 0x12, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xC1, 0x09, 0x20, 0x08, 0x92, 0x10, 0x81, 0x01,
        0x21, 0x02, 0x24, 0x51, 0x12, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x21, 0x08, 0xA0, 0x10, 0x92, 0x11, 0x41, 0x02, 0x21, 0x02,
        0x24, 0x21, 0xF2, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x08, 0x08, 0x21, 0x08, 0xA0, 0x21, 0xFA, 0x12, 0x21, 0x04, 0x21, 0x02, 0x24, 0x51,
        0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x08, 0x21, 0x08, 0x60, 0x40, 0x12, 0x12, 0x21, 0x08, 0x21, 0x03, 0x24, 0x51, 0x12, 0x44,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x07,
        0xC1, 0x08, 0x20, 0xFC, 0x11, 0xE4, 0x11, 0x1F, 0x9E, 0x02, 0xC4, 0x88, 0xE2, 0x38, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xBC, 0x10, 0xF1, 0xE0, 0x47, 0xCF, 0x3F, 0x3C, 0x78, 0x10,
        0x7E, 0x0E, 0x1F, 0x0F, 0xE7, 0xE1, 0xE1, 0x04, 0x82, 0x40, 0xA0, 0x40, 0x48, 0x21, 0xE1, 0xF8,
        0x3C, 0x3F, 0x0F, 0x89, 0xC2, 0x31, 0x0A, 0x10, 0xC4, 0x10, 0x82, 0x42, 0x84, 0x28, 0x41, 0x11,
        0x10, 0x88, 0x04, 0x02, 0x11, 0x04, 0x82, 0x41, 0x20, 0x60, 0xCC, 0x22, 0x11, 0x04, 0x42, 0x20,
        0x90, 0x49, 0xC2, 0x50, 0x08, 0x10, 0xC4, 0x10, 0x02, 0x42, 0x84, 0x28, 0x41, 0x20, 0x90, 0x48,
        0x04, 0x04, 0x09, 0x04, 0x82, 0x42, 0x20, 0x60, 0xCA, 0x24, 0x09, 0x04, 0x81, 0x20, 0x90, 0x49,
        0xC2, 0x10, 0x08, 0x11, 0x48, 0x10, 0x04, 0x42, 0x84, 0x28, 0x41, 0x20, 0x10, 0x48, 0x04, 0x04,
        0x01, 0x04, 0x82, 0x44, 0x20, 0x51, 0x4A, 0x24, 0x09, 0x04, 0x81, 0x20, 0x90, 0x09, 0xC2, 0x10,
        0x08, 0xE1, 0x4F, 0x97, 0x04, 0x3C, 0x8C, 0x44, 0x7E, 0x20, 0x10, 0x4F, 0xE7, 0xC4, 0x01, 0xFC,
        0x82, 0x48, 0x20, 0x51, 0x49, 0x24, 0x09, 0x04, 0x81, 0x20, 0x8E, 0x09, 0xC2, 0x10, 0x10, 0x12,
        0x48, 0x58, 0x88, 0x42, 0x74, 0x44, 0x41, 0x20, 0x10, 0x48, 0x04, 0x04, 0x79, 0x04, 0x82, 0x58,
        0x20, 0x4A, 0x49, 0x24, 0x09, 0xF8, 0x81, 0x3F, 0x01, 0x89, 0xC2, 0x10, 0x20, 0x12, 0x40, 0x50,
        0x88, 0x42, 0x04, 0xFE, 0x41, 0x20, 0x10, 0x48, 0x04, 0x04, 0x09, 0x04, 0x82, 0x64, 0x20, 0x4A,
        0x48, 0xA4, 0x09, 0x00, 0x81, 0x22, 0x00, 0x49, 0xC2, 0x10, 0x40, 0x17, 0xE0, 0x50, 0x90, 0x42,
        0x04, 0x82, 0x41, 0x20, 0x90, 0x48, 0x04, 0x04, 0x09, 0x04, 0xA2, 0x42, 0x20, 0x4A, 0x48, 0xA4,
        0x09, 0x00, 0x8D, 0x21, 0x10, 0x49, 0xC2, 0x10, 0x82, 0x10, 0x48, 0x50, 0x90, 0x42, 0x89, 0x01,
        0x41, 0x11, 0x10, 0x88, 0x04, 0x02, 0x11, 0x04, 0xA2, 0x41, 0x20, 0x44, 0x48, 0x62, 0x11, 0x00,
        0x42, 0x21, 0x10, 0x41, 0xBC, 0x11, 0xF9, 0xE0, 0x47, 0x8F, 0x10, 0x3C, 0x71, 0x01, 0x7E, 0x0E,
        0x1F, 0x0F, 0xE4, 0x01, 0xE1, 0x04, 0x9C, 0x40, 0xBF, 0x44, 0x48, 0x21, 0xE1, 0x00, 0x3D, 0x20,
        0x8F, 0x89, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF

};

