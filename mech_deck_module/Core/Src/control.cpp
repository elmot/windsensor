#include <deck-module.hpp>
#include "support.h"
#include "nrf24.h"
//
// Created by ilia.motornyi on 13-Dec-18.
//
// Buffer to store a payload of maximum width + 1 for termination zero

uint8_t nRF24_payload[33];

bool radioDebug = false;

void Toggle_LED() {
    LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

// Length of received payload
uint8_t payload_length;

#define nRF24_WAIT_TIMEOUT         0x000FFFFFu

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
nRF24_TXResult nRF24_TransmitPacket(void *pBuf, uint8_t length) {
    volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
    uint8_t status;

    // Deassert the CE pin (in case if it still high)
    nRF24_CE_L();

    // Transfer a data from the specified buffer to the TX FIFO
    nRF24_WritePayload((uint8_t *)pBuf, length);

    // Start a transmission by asserting CE pin (must be held at least 10us)
    nRF24_CE_H();

    // Poll the transceiver status register until one of the following flags will be set:
    //   TX_DS  - means the packet has been transmitted
    //   MAX_RT - means the maximum number of TX retransmits happened
    // note: this solution is far from perfect, better to use IRQ instead of polling the status
    do {
        status = nRF24_GetStatus();
        if (status & (uint8_t)(nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
            break;
        }
    } while (wait--);

    // Deassert the CE pin (Standby-II --> Standby-I)
    nRF24_CE_L();

    if (!wait) {
        // Timeout
        return nRF24_TX_TIMEOUT;
    }

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

static char button[] = "B1";

void checkButtons();

void appendChecksumEol(char *nmea, int maxLen);

void radioLoop(void) {


    // Some variables
    static uint32_t packets_lost = 0; // global counter of lost packets
    static uint8_t otx;
    static uint8_t otx_plos_cnt; // lost packet count
    static uint8_t otx_arc_cnt; // retransmit count


    if (radioDebug) printf("$ELMOT,PAYLOAD:>%s<TX:,", button);

    // Transmit a packet
    tx_res = nRF24_TransmitPacket(button, 6);
    otx = nRF24_GetRetransmitCounters();
    nRF24_ReadPayloadDpl(nRF24_payload, &payload_length);
    otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT) >> 4u; // packets lost counter
    otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT); // auto retransmissions counter
    switch (tx_res) {
        case nRF24_TX_SUCCESS:
            if (radioDebug) printf("OK");
            break;
        case nRF24_TX_TIMEOUT:
            if (radioDebug) printf("TIMEOUT");
            break;
        case nRF24_TX_MAXRT:
            if (radioDebug) printf("MAX RETRANSMIT");
            packets_lost += otx_plos_cnt;
            nRF24_ResetPLOS();
            break;
        default:
            if (radioDebug) printf("ERROR");
            break;
    }
    nRF24_payload[payload_length] = 0;

    if (radioDebug) printf(",ACK_PAYLOAD=>%s<,ARC=%d,LOST=%ld\r\n", nRF24_payload, otx_arc_cnt, packets_lost);
    int buttonReport, agcReport, angleReport, speedReport;
    sscanf((const char *) nRF24_payload, "%x;%x;%x;%x", &buttonReport, &agcReport, &angleReport, &speedReport);

    static char nmea[100];

    snprintf(nmea, sizeof(nmea), "$GPRMC,123519,A,6022.0962,N,02829.173,E,012.4,064.4,230119,003.1,W");
    appendChecksumEol(nmea, sizeof(nmea));
    if (radioDebug) printf(nmea);

    const char *lr;
    int angle;
    if (agcReport == 0x80 || agcReport == 0) {
        lr = "";
        angle = -1;
    } else if ( angleReport >= 0x800 )
    {
        lr="L";
        angle = 180 *(0xFFF - angleReport) / 0x800;
    } else {
        lr="R";
        angle = 180 * angleReport / 0x800;
    }
    snprintf(nmea, sizeof(nmea), "$WIVWR,%d.0,%s,12.2,N,6.27,M,22.6,K", angle, lr);
    appendChecksumEol(nmea, sizeof(nmea));
    if (radioDebug) printf(nmea);

    // Wait ~0.5s
    if (payload_length > 0) {
        Toggle_LED();
    }
    checkButtons();

}

void appendChecksumEol(char *nmea, int maxLen) {
    uint sum = 0;
    int pos;
    if ((nmea == nullptr) || (*nmea == 0)) return;
    for (pos = 1; nmea[pos] != 0; pos++) {
        sum ^= nmea[pos];
    }
    snprintf(&nmea[pos], (size_t) (maxLen - pos), "*%02x\r\n", sum);

}

void checkButtons() {

/* todo restore
    if (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET) {
        button = "B1";
    } else if (HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin) == GPIO_PIN_RESET) {
        button = "B2";
    } else if (HAL_GPIO_ReadPin(BTN3_GPIO_Port, BTN3_Pin) == GPIO_PIN_RESET) {
        button = "B3";
    }
*/
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

DeviceState radioCheck() {
    if (radioDebug) printf("\r\nSTM32L432KC is online.\r\n");
    Delay_ms(100);

    // RX/TX disabled
    nRF24_CE_L();

    // Configure the nRF24L01+
    if (radioDebug) printf("nRF24L01+ check: ");
    if (!nRF24_Check()) {
        if (radioDebug) printf("SPI ERROR\n");
        state.anemState = CONN_FAIL;
    }
    if (radioDebug) printf("OK\r\n");
    state.anemState = DATA_NO_FIX;
    return state.anemState;
}