#include <memory.h>
#include <stdbool.h>
#include "support.h"
#include "nrf24.h"
//
// Created by ilia.motornyi on 13-Dec-18.
//
// Buffer to store a payload of maximum width

extern I2C_HandleTypeDef hi2c1;
static char lamp = 0;

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
        Error_Handler();
    }
    printf("OK\r\n");
}

void radioLoop() {
    if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
        // Get a payload from the transceiver
        pipe = nRF24_ReadPayloadDpl(nRF24_payload, &payload_length);
        if (payload_length > 0) {
            nRF24_WriteAckPayload(pipe, reply, (uint8_t) strlen(reply));
            toggleDiagLed();
        }

        // Clear all pending IRQ flags
        nRF24_ClearIRQFlags();

        // Print a payload contents to UART

        nRF24_payload[payload_length] = 0;
        printf("RCV PIPE#%d PAYLOAD:>%s<\r\n", pipe, nRF24_payload);
        lamp = (char) (nRF24_payload[1]);
        switch(lamp) {
            case '2':
                HAL_GPIO_WritePin(NAVI_GPIO_Port,NAVI_Pin,GPIO_PIN_SET);
                HAL_GPIO_WritePin(ANCHOR_GPIO_Port,ANCHOR_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LIGHT_ENA_GPIO_Port,LIGHT_ENA_Pin,GPIO_PIN_RESET);
                break;
            case '3':
                HAL_GPIO_WritePin(NAVI_GPIO_Port,NAVI_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(ANCHOR_GPIO_Port,ANCHOR_Pin,GPIO_PIN_SET);
                HAL_GPIO_WritePin(LIGHT_ENA_GPIO_Port,LIGHT_ENA_Pin,GPIO_PIN_RESET);
                break;
            default:
                HAL_GPIO_WritePin(NAVI_GPIO_Port,NAVI_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(ANCHOR_GPIO_Port,ANCHOR_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LIGHT_ENA_GPIO_Port,LIGHT_ENA_Pin,GPIO_PIN_SET);
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
static char sens_data_internal[15];
char *reply;
void sensorLoop() {
    HAL_StatusTypeDef status;
    uint8_t agc = 0;
    uint16_t angle = 0;
    uint16_t speed = 123;//todo read from somewhere
    agc = (uint8_t) as5601ReadReg(0x1A, false, 0xFF, &status);
    if(status == HAL_OK) {
        angle = as5601ReadReg(0x0C, true, 0xFFF, &status);
    }
    if(status == HAL_OK) {
        snprintf(sens_data_internal, sizeof(sens_data_internal),"%c;%02x;%03x;%04x", lamp ? '0' : '1' ,agc,angle,speed);
        reply = sens_data_internal;
    } else {
        reply = "ERROR";
        resetI2C();
    }
}
#pragma clang diagnostic pop

/*
void sensorLoop() {
    AS5601_print_reg8("Status: %02x; ", 0xb,0x38);
    AS5601_print_reg8("AGC: %3x; ", 0x1a,0xff);
    AS5601_print_reg16("Raw Angle: %04x; ", 0x0c, 0xFFF);
    AS5601_print_reg16("Angle: %04x\n\r", 0x0e, 0xFFF);

}
 */