#include <memory.h>
#include "support.h"
#include "nrf24.h"
//
// Created by ilia.motornyi on 13-Dec-18.
//
// Buffer to store a payload of maximum width


void Toggle_LED() {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

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
            nRF24_WriteAckPayload(pipe, "aCk PaYlOaD", 11);
        }

        // Clear all pending IRQ flags
        nRF24_ClearIRQFlags();

        // Print a payload contents to UART
        Toggle_LED();
        nRF24_payload[payload_length] = 0;
        printf("RCV PIPE#%d PAYLOAD:>%s<\r\n", pipe, nRF24_payload);
    }
}
