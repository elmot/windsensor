#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include "support.h"
#include "nrf24.h"
//
// Created by ilia.motornyi on 13-Dec-18.
//
// Buffer to store a payload of maximum width

extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;
extern IWDG_HandleTypeDef hiwdg;
static char lamp = '0';

uint8_t nRF24_payload[32];

// Pipe number
nRF24_RXResult pipe;


// Length of received payload
uint8_t payload_length;


void radioInit() {// Initialize the nRF24L01 to its default state
    nRF24_Init();

/***************************************************************************/

    // This is simple receiver with Enhanced ShockBurst:
//   - RX address: 'ESB'
//   - payload: 10 bytes
//   - RF channel: 40 (2440MHz)
//   - data rate: 2Mbps
//   - CRC scheme: 2 byte

    // The transmitter sends a 10-byte packets to the address 'ESB' with Auto-ACK (ShockBurst enabled)

    // Set RF channel
    nRF24_SetRFChannel(40);

    // Set data rate
    nRF24_SetDataRate(nRF24_DR_2Mbps);

    // Set CRC scheme
    nRF24_SetCRCScheme(nRF24_CRC_2byte);

    // Set address width, its common for all pipes (RX and TX)
    nRF24_SetAddrWidth(3);

    // Configure RX PIPE
    static const uint8_t nRF24_ADDR[] = {'E', 'S', 'B'};
    nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for pipe
    nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, 10); // Auto-ACK: enabled, payload length: 10 bytes

    // Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
    nRF24_SetTXPower(nRF24_TXPWR_0dBm);

    // Set operational mode (PRX == receiver)
    nRF24_SetOperationalMode(nRF24_MODE_RX);

    // Clear any pending IRQ flags
    nRF24_ClearIRQFlags();

    // Wake the transceiver
    nRF24_SetPowerMode(nRF24_PWR_UP);

    // Enable DPL
    nRF24_SetDynamicPayloadLength(nRF24_DPL_ON);

    nRF24_SetPayloadWithAck(1);


    // Put the transceiver to the RX mode
    nRF24_CE_H();
}

void radioCheck() {
    Delay_ms(100);
    nRF24_Init();

    printf("\r\nSTM32F303RE is online.\r\n");

    // RX/TX disabled
    nRF24_CE_L();

    // Configure the nRF24L01+
    printf("nRF24L01+ check: ");
    if (!nRF24_Check()) {
        Error_Indication(400, 50);
    }
    printf("OK\r\n");
}

void radioLoop() {
    if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
        // Get a payload from the transceiver
        pipe = nRF24_ReadPayloadDpl(nRF24_payload, &payload_length);
        if (payload_length > 0) {
            char *tmp = (char *) reply;
            nRF24_WriteAckPayload(pipe, tmp, (uint8_t) strlen(tmp));
        }

        // Clear all pending IRQ flags
        nRF24_ClearIRQFlags();

        // Print a payload contents to UART

        nRF24_payload[payload_length] = 0;
        printf("RCV PIPE#%d PAYLOAD:>%s<\r\n", pipe, nRF24_payload);
        lamp = (char) (nRF24_payload[1]);
        switch (lamp) {
            case '2':
                HAL_GPIO_WritePin(NAVI_GPIO_Port, NAVI_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(ANCHOR_GPIO_Port, ANCHOR_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LIGHT_ENA_GPIO_Port, LIGHT_ENA_Pin, GPIO_PIN_RESET);
                break;
            case '3':
                HAL_GPIO_WritePin(NAVI_GPIO_Port, NAVI_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(ANCHOR_GPIO_Port, ANCHOR_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(LIGHT_ENA_GPIO_Port, LIGHT_ENA_Pin, GPIO_PIN_RESET);
                break;
            default:
                HAL_GPIO_WritePin(NAVI_GPIO_Port, NAVI_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(ANCHOR_GPIO_Port, ANCHOR_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LIGHT_ENA_GPIO_Port, LIGHT_ENA_Pin, GPIO_PIN_SET);
        }
    }
}

#define  AS5601_ADDR 0x36
#define  I2C_TIMEOUT 1000

uint16_t as5601ReadReg(int addr, bool wide, uint16_t mask, HAL_StatusTypeDef *status) {
    uint16_t buf;
    *status = HAL_I2C_Mem_Read(&hi2c1,
                               AS5601_ADDR * 2,
                               (uint16_t) addr,
                               I2C_MEMADD_SIZE_8BIT,
                               (uint8_t *) &buf,
                               (uint16_t) (wide ? 2 : 1),
                               I2C_TIMEOUT);
    if (wide) {
        return __bswap16(buf) & mask;
    } else {
        return buf & mask;
    }
}

void AS5601_print_reg8(const char *formatStr, int addr, uint8_t mask) {
    HAL_StatusTypeDef status;
    uint8_t result = (uint8_t) as5601ReadReg(addr, false, mask, &status);
    if (status == HAL_OK) {
        printf(formatStr, result & mask);
    } else {
        printf("ERROR %x\n\r", status);
    }
}

void AS5601_print_reg16(const char *formatStr, int addr, uint16_t mask) {
    HAL_StatusTypeDef status;
    uint16_t result = as5601ReadReg(addr, true, mask, &status);
    if (status == HAL_OK) {
        printf(formatStr, result & mask);
    } else {
        printf("ERROR %x\n\r", status);
    }
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
static char sens_data_internal[2][30];
static unsigned int replyBufIdx = 0;

#define ANGLE_BUFF_LEN 21

static int angle[ANGLE_BUFF_LEN];
static int angleCopy[ANGLE_BUFF_LEN];
static int angleIdx;

static int volts;

#define VREF_MV 1200

volatile char *reply;

static int compareInt(const void *a, const void *b) {
    return (*(int *) a) - (*(int *) b);
}

void measureAngleAndMakeReply(HAL_StatusTypeDef *status, uint8_t agc, uint16_t ticks);

void measureVolts();

void sensorLoop() {
    HAL_StatusTypeDef status;
    uint8_t agc = 0;
    __disable_irq();
    if (++wind_ticks_skipped >= 20) {
        wind_ticks = 0;
    }
    uint16_t ticks = wind_ticks;
    __enable_irq();
    measureVolts();
    measureAngleAndMakeReply(&status, agc, ticks);
}

void measureVolts() {
    HAL_ADCEx_InjectedPollForConversion(&hadc1, 30);
    int vRef = HAL_ADCEx_InjectedGetValue(&hadc1, 2);
    int measured = HAL_ADCEx_InjectedGetValue(&hadc1, 1);
    volts = VREF_MV * measured / vRef / 4095;
    HAL_ADCEx_InjectedStart(&hadc1);
}

void measureAngleAndMakeReply(HAL_StatusTypeDef *status, uint8_t agc, uint16_t ticks) {
    agc = (uint8_t) as5601ReadReg(0x1A, false, 0xFF, status);
    if ((*status) == HAL_OK) {
        angle[angleIdx] = as5601ReadReg(0x0C, true, 0xFFF, status);
        angleIdx = (angleIdx + 1) % ANGLE_BUFF_LEN;
        memcpy(angleCopy, angle, ANGLE_BUFF_LEN * sizeof(angleCopy[0]));
        qsort(angleCopy, ANGLE_BUFF_LEN, sizeof(angleCopy[0]), compareInt);
    }
    if ((*status) == HAL_OK) {
        snprintf(sens_data_internal[replyBufIdx], sizeof(sens_data_internal[0]), "%c;%02x;%03d;%04x;%d", lamp, agc,
                 angleCopy[ANGLE_BUFF_LEN / 2] * 360 / 4095,
                 ticks, volts);
        if (agc == 0 || agc == 0x80) {
            for (int i = 0; i < 40; i++) {
                HAL_GPIO_TogglePin(DIAG_GPIO_Port, DIAG_Pin);
                HAL_Delay(30);
            }
        }
        reply = sens_data_internal[replyBufIdx];
        replyBufIdx ^= 1u;
    } else {
        reply = "ERROR";
        for (int i = 0; i < 40; i++) {
            HAL_GPIO_WritePin(DIAG_GPIO_Port, DIAG_Pin, GPIO_PIN_SET);
            HAL_IWDG_Refresh(&hiwdg);
            HAL_Delay(130);
            HAL_GPIO_WritePin(DIAG_GPIO_Port, DIAG_Pin, GPIO_PIN_RESET);
            HAL_IWDG_Refresh(&hiwdg);
            HAL_Delay(30);
        }
        resetI2C();
    }
}

#pragma clang diagnostic pop

volatile int wind_ticks = 0;
volatile int wind_ticks_skipped = 100;
/*
void sensorLoop() {
    AS5601_print_reg8("Status: %02x; ", 0xb,0x38);
    AS5601_print_reg8("AGC: %3x; ", 0x1a,0xff);
    AS5601_print_reg16("Raw Angle: %04x; ", 0x0c, 0xFFF);
    AS5601_print_reg16("Angle: %04x\n\r", 0x0e, 0xFFF);

}
 */