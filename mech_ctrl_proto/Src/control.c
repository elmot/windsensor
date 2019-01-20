#include <memory.h>
#include "support.h"
#include "nrf24.h"
//
// Created by ilia.motornyi on 13-Dec-18.
//
// Buffer to store a payload of maximum width + 1 for termination zero

uint8_t nRF24_payload[33];

void Toggle_LED() {
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
}

// Pipe number
nRF24_RXResult pipe;

uint32_t i, j, k;

// Length of received payload
uint8_t payload_length;

#define nRF24_WAIT_TIMEOUT         (uint32_t)0x000FFFFF

// Result of packet transmission
typedef enum {
    nRF24_TX_ERROR = (uint8_t) 0x00, // Unknown error
    nRF24_TX_SUCCESS,                // Packet has been transmitted successfully
    nRF24_TX_TIMEOUT,                // It was timeout during packet transmit
    nRF24_TX_MAXRT                   // Transmit failed with maximum auto retransmit count
} nRF24_TXResult;

nRF24_TXResult tx_res;

// Function to transmit data packet
// input:
//   pBuf - pointer to the buffer with data to transmit
//   length - length of the data buffer in bytes
// return: one of nRF24_TX_xx values
nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length) {
    volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
    uint8_t status;

    // Deassert the CE pin (in case if it still high)
    nRF24_CE_L();

    // Transfer a data from the specified buffer to the TX FIFO
    nRF24_WritePayload(pBuf, length);

    // Start a transmission by asserting CE pin (must be held at least 10us)
    nRF24_CE_H();

    // Poll the transceiver status register until one of the following flags will be set:
    //   TX_DS  - means the packet has been transmitted
    //   MAX_RT - means the maximum number of TX retransmits happened
    // note: this solution is far from perfect, better to use IRQ instead of polling the status
    do {
        status = nRF24_GetStatus();
        if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
            break;
        }
    } while (wait--);

    // Deassert the CE pin (Standby-II --> Standby-I)
    nRF24_CE_L();

    if (!wait) {
        // Timeout
        return nRF24_TX_TIMEOUT;
    }

    // Check the flags in STATUS register
    printf("[%02x]", status);

    // Clear pending IRQ flags
    nRF24_ClearIRQFlags();

    if (status & nRF24_FLAG_MAX_RT) {
        // Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
        return nRF24_TX_MAXRT;
    }

    if (status & nRF24_FLAG_TX_DS) {
        // Successful transmission
        return nRF24_TX_SUCCESS;
    }

    // Some banana happens, a payload remains in the TX FIFO, flush it
    nRF24_FlushTX();

    return nRF24_TX_ERROR;
}
static char * button ="B1";

void checkButtons();

void radioLoop(void) {


    // Some variables
    static uint32_t packets_lost = 0; // global counter of lost packets
    static uint8_t otx;
    static uint8_t otx_plos_cnt; // lost packet count
    static uint8_t otx_arc_cnt; // retransmit count


    // The main loop
    static int j = 33;

//    payload_length = (uint8_t) (2 + (j + j / 10) % 7);
//    payload_length = 4;

    // Prepare data packet
/*
    for (i = 0; i < payload_length; i++) {
        nRF24_payload[i] = (uint8_t) j++;
        if (j > 'z') j = 33;
    }
*/

    // Print a payload
//    nRF24_payload[payload_length] = 0;
//    printf("PAYLOAD:>%s< ... TX: ", nRF24_payload);
    printf("PAYLOAD:>%s< ... TX: ", button);

    // Transmit a packet
    tx_res = nRF24_TransmitPacket(button, 6);
    otx = nRF24_GetRetransmitCounters();
    nRF24_ReadPayloadDpl(nRF24_payload, &payload_length);
    otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT) >> 4; // packets lost counter
    otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT); // auto retransmissions counter
    switch (tx_res) {
        case nRF24_TX_SUCCESS:
            printf("OK");
            break;
        case nRF24_TX_TIMEOUT:
            printf("TIMEOUT");
            break;
        case nRF24_TX_MAXRT:
            printf("MAX RETRANSMIT");
            packets_lost += otx_plos_cnt;
            nRF24_ResetPLOS();
            break;
        default:
            printf("ERROR");
            break;
    }
    nRF24_payload[payload_length] = 0;
    printf("   ACK_PAYLOAD=>%s<   ARC=%d LOST=%ld\r\n", nRF24_payload, otx_arc_cnt, packets_lost);

    // Wait ~0.5s
    if(payload_length>0){
        Toggle_LED();
    }
    checkButtons();

}

void checkButtons() {

    if(HAL_GPIO_ReadPin(BTN1_GPIO_Port,BTN1_Pin)==GPIO_PIN_RESET) {
        button = "B1";
    } else

    if(HAL_GPIO_ReadPin(BTN2_GPIO_Port,BTN2_Pin)==GPIO_PIN_RESET) {
        button = "B2";
    } else

    if(HAL_GPIO_ReadPin(BTN3_GPIO_Port,BTN3_Pin)==GPIO_PIN_RESET) {
        button = "B3";
    }
}

void radioInit() {// Initialize the nRF24L01 to its default state
    nRF24_Init();


    // This is simple transmitter with Enhanced ShockBurst (to one logic address):
//   - TX address: 'ESB'
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

    // Configure TX PIPE
    static const uint8_t nRF24_ADDR[] = {'E', 'S', 'B'};
    nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
    nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR); // program address for pipe#0, must be same as TX (for Auto-ACK)

    // Set TX power (maximum)
    nRF24_SetTXPower(nRF24_TXPWR_0dBm);

    // Configure auto retransmit: 10 retransmissions with pause of 2500s in between
    nRF24_SetAutoRetr(nRF24_ARD_2500us, 10);

    // Enable Auto-ACK for pipe#0 (for ACK packets)
    nRF24_EnableAA(nRF24_PIPE0);

    // Set operational mode (PTX == transmitter)
    nRF24_SetOperationalMode(nRF24_MODE_TX);

    // Clear any pending IRQ flags
    nRF24_ClearIRQFlags();

    // Enable DPL
    nRF24_SetDynamicPayloadLength(nRF24_DPL_ON);
    nRF24_SetPayloadWithAck(1);


    // Wake the transceiver
    nRF24_SetPowerMode(nRF24_PWR_UP);
}

void radioCheck() {
    printf("\r\nSTM32L432KC is online.\r\n");
    Delay_ms(100);

    // RX/TX disabled
    nRF24_CE_L();

    // Configure the nRF24L01+
    printf("nRF24L01+ check: ");
    if (!nRF24_Check()) {
        Error_Handler();
    }
    printf("OK\r\n");
}
