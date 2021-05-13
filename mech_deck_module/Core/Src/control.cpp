#include <deck-module.hpp>
#include "nrf24.h"
//
// Created by ilia.motornyi on 13-Dec-18.
//
// Buffer to store a payload of maximum width + 1 for termination zero

uint8_t nRF24_payload[33];

static const bool radioDebug = true;

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
nRF24_TXResult nRF24_TransmitPacket(const void *pBuf, uint8_t length) {
    volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
    uint8_t status;

    // Deassert the CE pin (in case if it still high)
    nRF24_CE_L();

    // Transfer a data from the specified buffer to the TX FIFO
    nRF24_WritePayload((uint8_t *) pBuf, length);

    // Start a transmission by asserting CE pin (must be held at least 10us)
    nRF24_CE_H();

    // Poll the transceiver status register until one of the following flags will be set:
    //   TX_DS  - means the packet has been transmitted
    //   MAX_RT - means the maximum number of TX retransmits happened
    // note: this solution is far from perfect, better to use IRQ instead of polling the status
    do {
        status = nRF24_GetStatus();
        if (status & (uint8_t) (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
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

void appendChecksumEol(char *nmea, int maxLen);

int calcTpm(int speedReport);

void radioLoop(void) {

    // Some variables
    static uint32_t packets_lost = 0; // global counter of lost packets
    static uint8_t otx;
    static uint8_t otx_plos_cnt; // lost packet count
    static uint8_t otx_arc_cnt; // retransmit count
    char button;

    static uint_fast64_t nextDiag = 0;
    bool doDebug = false;
    if (nextDiag < sysTicks()) {
        doDebug = true;
        nextDiag = sysTicks() + 200;
    }

    switch (state.lights) {
        case ANCHOR:
            button = '2';
            break;
        case NAVI:
            button = '3';
            break;
        default:
            button = '1';
    }
    if (doDebug) {
        printf("$ELMOT,PL:>%c<TX:,", button);
    }

    // Transmit a packet
    char buffer[30] = "B?;";
    buffer[1] = button;
    strcat(buffer, SENSOR_SIGNATURE);
    tx_res = nRF24_TransmitPacket(buffer, strlen(buffer));
    otx = nRF24_GetRetransmitCounters();
    nRF24_ReadPayloadDpl(nRF24_payload, &payload_length);
    otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT) >> 4u; // packets lost counter
    otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT); // auto retransmissions counter
    switch (tx_res) {
        case nRF24_TX_SUCCESS:
            char buttonReport;
            int agcReport, speedReport, voltage, angle;
            char receivedSignature[9];
            sscanf((const char *) nRF24_payload, "%c;%x;%d;%x;%d;%8s",
                   &buttonReport, &agcReport, &angle, &speedReport, &voltage, receivedSignature);
            if (strncmp(SENSOR_SIGNATURE, receivedSignature, SENSOR_SIGNATURE_LEN) == 0) {
                if (agcReport == 0x80 || agcReport == 0) {
                    state.windAngle = -1;
                    state.windAngleNonCorrected = -1;
                    state.anemState = DATA_ERROR;
                    if (doDebug) printf("ANGLE_SENSOR_ERROR");
                } else {
                    state.windAngleNonCorrected = angle;
                    state.windAngle = (angle + (int) naviSettings->windAngleCorrection) % 360;
                    state.anemState = OK;
                }
                state.windTics = calcTpm(speedReport);
                state.windSpdMps = calcSpeed(state.fixedTics,state.fixedWindMs, state.windTics);
            }
            break;
        case nRF24_TX_TIMEOUT:
            state.anemState = CONN_FAIL;
            if (doDebug) printf("TIMEOUT");
            break;
        case nRF24_TX_MAXRT:
            state.anemState = CONN_FAIL;
            if (doDebug) printf("MAX RETRANSMIT");
            packets_lost += otx_plos_cnt;
            nRF24_ResetPLOS();
            break;
        default:
            state.anemState = CONN_FAIL;
            if (doDebug) printf("ERROR");
            break;
    }
    nRF24_payload[payload_length] = 0;

    if (doDebug) printf(",ACK_PL=>%s<,ARC=%d,LST=%ld\r\n", nRF24_payload, otx_arc_cnt, packets_lost);
}

float calcSpeed(const int fixedTics, const float fixedWind, int ticksPerMin) {
    if (ticksPerMin <= 0) return 0;

    float speed = 0.5f + fixedWind * (float) ticksPerMin / (float)fixedTics;
    return speed < naviSettings->minWindMs ? 0 : speed;
}

int calcTpm(int speedReport) {
    if (speedReport <= 0) return -1;
    return 60 * 10000 / speedReport;
}

void outputNmea() {
    const char *lr;
    static char nmea[150];
    static uint_fast64_t nextNmea = 0;
    if (nextNmea > sysTicks()) return;
    nextNmea = sysTicks() + 100;
    if (state.windAngle < 0) {
        strcpy(nmea, "$WIVWR,,,,N,,M,,K");
    } else {
        int nmeaAngle;
        if (state.windAngle > 180) {
            lr = "L";
            nmeaAngle = 360 - state.windAngle;
        } else {
            lr = "R";
            nmeaAngle = state.windAngle;
        }
        float windSpeedKph = state.windSpdMps * 3.6f;
        float windSpeedKn = windSpeedKph / 1.852f;
        snprintf(nmea, sizeof(nmea), "$WIVWR,%d.0,%s,%f,N,%f,M,%f,K", nmeaAngle, lr,
                 windSpeedKn, state.windSpdMps, windSpeedKph);
    }
    appendChecksumEol(nmea, sizeof(nmea));
    fwrite(nmea,1, strlen(nmea),stdout);
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
DeviceState radioCheck() {
    if (radioDebug) printf("\r\nSTM32L432KC is online.\r\n");
    msDelay(100);

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
#pragma clang diagnostic pop
