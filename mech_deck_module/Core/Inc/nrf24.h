#ifndef __NRF24_H
#define __NRF24_H


// Low level functions (hardware depended)
#include "support.h"

#ifdef __cplusplus
extern "C" {
#endif

// nRF24L0 instruction definitions
#define nRF24_CMD_R_REGISTER         0x00u // Register read
#define nRF24_CMD_W_REGISTER         0x20u // Register write
#define nRF24_CMD_ACTIVATE           0x50u // (De)Activates R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK features
#define nRF24_CMD_R_RX_PL_WID         0x60u // Read RX-payload width for the top R_RX_PAYLOAD in the RX FIFO.
#define nRF24_CMD_R_RX_PAYLOAD       0x61u // Read RX payload
#define nRF24_CMD_W_TX_PAYLOAD       0xA0u // Write TX payload
#define nRF24_CMD_W_ACK_PAYLOAD      0xA8u // Write ACK payload
#define nRF24_CMD_W_TX_PAYLOAD_NOACK 0xB0u//Write TX payload and disable AUTOACK
#define nRF24_CMD_FLUSH_TX           0xE1u // Flush TX FIFO
#define nRF24_CMD_FLUSH_RX           0xE2u // Flush RX FIFO
#define nRF24_CMD_REUSE_TX_PL        0xE3u // Reuse TX payload
#define nRF24_CMD_LOCK_UNLOCK        0x50u // Lock/unlock exclusive features
#define nRF24_CMD_NOP                0xFFu // No operation (used for reading status register)

// nRF24L0 register definitions
#define nRF24_REG_CONFIG           0x00u // Configuration register
#define nRF24_REG_EN_AA            0x01u // Enable "Auto acknowledgment"
#define nRF24_REG_EN_RXADDR        0x02u // Enable RX addresses
#define nRF24_REG_SETUP_AW         0x03u // Setup of address widths
#define nRF24_REG_SETUP_RETR       0x04u // Setup of automatic retransmit
#define nRF24_REG_RF_CH            0x05u // RF channel
#define nRF24_REG_RF_SETUP         0x06u // RF setup register
#define nRF24_REG_STATUS           0x07u // Status register
#define nRF24_REG_OBSERVE_TX       0x08u // Transmit observe register
#define nRF24_REG_RPD              0x09u // Received power detector
#define nRF24_REG_RX_ADDR_P0       0x0Au // Receive address data pipe 0
#define nRF24_REG_RX_ADDR_P1       0x0Bu // Receive address data pipe 1
#define nRF24_REG_RX_ADDR_P2       0x0Cu // Receive address data pipe 2
#define nRF24_REG_RX_ADDR_P3       0x0Du // Receive address data pipe 3
#define nRF24_REG_RX_ADDR_P4       0x0Eu // Receive address data pipe 4
#define nRF24_REG_RX_ADDR_P5       0x0Fu // Receive address data pipe 5
#define nRF24_REG_TX_ADDR          0x10u // Transmit address
#define nRF24_REG_RX_PW_P0         0x11u // Number of bytes in RX payload in data pipe 0
#define nRF24_REG_RX_PW_P1         0x12u // Number of bytes in RX payload in data pipe 1
#define nRF24_REG_RX_PW_P2         0x13u // Number of bytes in RX payload in data pipe 2
#define nRF24_REG_RX_PW_P3         0x14u // Number of bytes in RX payload in data pipe 3
#define nRF24_REG_RX_PW_P4         0x15u // Number of bytes in RX payload in data pipe 4
#define nRF24_REG_RX_PW_P5         0x16u // Number of bytes in RX payload in data pipe 5
#define nRF24_REG_FIFO_STATUS      0x17u // FIFO status register
#define nRF24_REG_DYNPD            0x1Cu // Enable dynamic payload length
#define nRF24_REG_FEATURE          0x1Du // Feature register

// Register bits definitions
#define nRF24_CONFIG_PRIM_RX       0x01u // PRIM_RX bit in CONFIG register
#define nRF24_CONFIG_PWR_UP        0x02u // PWR_UP bit in CONFIG register
#define nRF24_FEATURE_EN_DYN_ACK   0x01u // EN_DYN_ACK bit in FEATURE register
#define nRF24_FEATURE_EN_ACK_PAY   0x02u // EN_ACK_PAY bit in FEATURE register
#define nRF24_FEATURE_EN_DPL       0x04u // EN_DPL bit in FEATURE register
#define nRF24_FLAG_RX_DR           0x40u // RX_DR bit (data ready RX FIFO interrupt)
#define nRF24_FLAG_TX_DS           0x20u // TX_DS bit (data sent TX FIFO interrupt)
#define nRF24_FLAG_MAX_RT          0x10u // MAX_RT bit (maximum number of TX retransmits interrupt)

// Register masks definitions
#define nRF24_MASK_REG_MAP         0x1Fu // Mask bits[4:0] for CMD_RREG and CMD_WREG commands
#define nRF24_MASK_CRC             0x0Cu // Mask for CRC bits [3:2] in CONFIG register
#define nRF24_MASK_STATUS_IRQ      0x70u // Mask for all IRQ bits in STATUS register
#define nRF24_MASK_RF_PWR          0x06u // Mask RF_PWR[2:1] bits in RF_SETUP register
#define nRF24_MASK_RX_P_NO         0x0Eu // Mask RX_P_NO[3:1] bits in STATUS register
#define nRF24_MASK_DATARATE        0x28u // Mask RD_DR_[5,3] bits in RF_SETUP register
#define nRF24_MASK_EN_RX           0x3Fu // Mask ERX_P[5:0] bits in EN_RXADDR register
#define nRF24_MASK_RX_PW           0x3Fu // Mask [5:0] bits in RX_PW_Px register
#define nRF24_MASK_RETR_ARD        0xF0u // Mask for ARD[7:4] bits in SETUP_RETR register
#define nRF24_MASK_RETR_ARC        0x0Fu // Mask for ARC[3:0] bits in SETUP_RETR register
#define nRF24_MASK_RXFIFO          0x03u // Mask for RX FIFO status bits [1:0] in FIFO_STATUS register
#define nRF24_MASK_TXFIFO          0x30u // Mask for TX FIFO status bits [5:4] in FIFO_STATUS register
#define nRF24_MASK_PLOS_CNT        0xF0u // Mask for PLOS_CNT[7:4] bits in OBSERVE_TX register
#define nRF24_MASK_ARC_CNT         0x0Fu // Mask for ARC_CNT[3:0] bits in OBSERVE_TX register

// Fake address to test transceiver presence (5 bytes long)
#define nRF24_TEST_ADDR            "nRF24"


// Retransmit delay
enum {
    nRF24_ARD_NONE = 0x00u, // Dummy value for case when retransmission is not used
    nRF24_ARD_250us = 0x00u,
    nRF24_ARD_500us = 0x01u,
    nRF24_ARD_750us = 0x02u,
    nRF24_ARD_1000us = 0x03u,
    nRF24_ARD_1250us = 0x04u,
    nRF24_ARD_1500us = 0x05u,
    nRF24_ARD_1750us = 0x06u,
    nRF24_ARD_2000us = 0x07u,
    nRF24_ARD_2250us = 0x08u,
    nRF24_ARD_2500us = 0x09u,
    nRF24_ARD_2750us = 0x0Au,
    nRF24_ARD_3000us = 0x0Bu,
    nRF24_ARD_3250us = 0x0Cu,
    nRF24_ARD_3500us = 0x0Du,
    nRF24_ARD_3750us = 0x0Eu,
    nRF24_ARD_4000us = 0x0Fu
};

// Data rate
enum {
    nRF24_DR_250kbps = 0x20u, // 250kbps data rate
    nRF24_DR_1Mbps = 0x00u, // 1Mbps data rate
    nRF24_DR_2Mbps = 0x08u  // 2Mbps data rate
};

// RF output power in TX mode
enum {
    nRF24_TXPWR_18dBm = 0x00u, // -18dBm
    nRF24_TXPWR_12dBm = 0x02u, // -12dBm
    nRF24_TXPWR_6dBm = 0x04u, //  -6dBm
    nRF24_TXPWR_0dBm = 0x06u  //   0dBm
};

// CRC encoding scheme
enum {
    nRF24_CRC_off = 0x00u, // CRC disabled
    nRF24_CRC_1byte = 0x08u, // 1-byte CRC
    nRF24_CRC_2byte = 0x0cu  // 2-byte CRC
};

// nRF24L01 power control
enum {
    nRF24_PWR_UP = 0x02u, // Power up
    nRF24_PWR_DOWN = 0x00u  // Power down
};

// Transceiver mode
enum {
    nRF24_MODE_RX = 0x01u, // PRX
    nRF24_MODE_TX = 0x00u  // PTX
};

enum {
    nRF24_DPL_ON = 0x01u, // PRX
    nRF24_DPL_OFF = 0x00u  // PTX
};

// Enumeration of RX pipe addresses and TX address
enum {
    nRF24_PIPE0 = 0x00u, // pipe0
    nRF24_PIPE1 = 0x01u, // pipe1
    nRF24_PIPE2 = 0x02u, // pipe2
    nRF24_PIPE3 = 0x03u, // pipe3
    nRF24_PIPE4 = 0x04u, // pipe4
    nRF24_PIPE5 = 0x05u, // pipe5
    nRF24_PIPETX = 0x06u  // TX address (not a pipe in fact)
};

// State of auto acknowledgment for specified pipe
enum {
    nRF24_AA_OFF = 0x00u,
    nRF24_AA_ON = 0x01u
};

// Status of the RX FIFO
enum {
    nRF24_STATUS_RXFIFO_DATA = 0x00u, // The RX FIFO contains data and available locations
    nRF24_STATUS_RXFIFO_EMPTY = 0x01u, // The RX FIFO is empty
    nRF24_STATUS_RXFIFO_FULL = 0x02u, // The RX FIFO is full
    nRF24_STATUS_RXFIFO_ERROR = 0x03u  // Impossible state: RX FIFO cannot be empty and full at the same time
};

// Status of the TX FIFO
enum {
    nRF24_STATUS_TXFIFO_DATA = 0x00u, // The TX FIFO contains data and available locations
    nRF24_STATUS_TXFIFO_EMPTY = 0x01u, // The TX FIFO is empty
    nRF24_STATUS_TXFIFO_FULL = 0x02u, // The TX FIFO is full
    nRF24_STATUS_TXFIFO_ERROR = 0x03u  // Impossible state: TX FIFO cannot be empty and full at the same time
};

// Result of RX FIFO reading
typedef enum {
    nRF24_RX_PIPE0 = 0x00u, // Packet received from the PIPE#0
    nRF24_RX_PIPE1 = 0x01u, // Packet received from the PIPE#1
    nRF24_RX_PIPE2 = 0x02u, // Packet received from the PIPE#2
    nRF24_RX_PIPE3 = 0x03u, // Packet received from the PIPE#3
    nRF24_RX_PIPE4 = 0x04u, // Packet received from the PIPE#4
    nRF24_RX_PIPE5 = 0x05u, // Packet received from the PIPE#5
    nRF24_RX_EMPTY = 0xffu  // The RX FIFO is empty
} nRF24_RXResult;


// Addresses of the RX_PW_P# registers
static const uint8_t nRF24_RX_PW_PIPE[6] = {
        nRF24_REG_RX_PW_P0,
        nRF24_REG_RX_PW_P1,
        nRF24_REG_RX_PW_P2,
        nRF24_REG_RX_PW_P3,
        nRF24_REG_RX_PW_P4,
        nRF24_REG_RX_PW_P5
};

// Addresses of the address registers
static const uint8_t nRF24_ADDR_REGS[7] = {
        nRF24_REG_RX_ADDR_P0,
        nRF24_REG_RX_ADDR_P1,
        nRF24_REG_RX_ADDR_P2,
        nRF24_REG_RX_ADDR_P3,
        nRF24_REG_RX_ADDR_P4,
        nRF24_REG_RX_ADDR_P5,
        nRF24_REG_TX_ADDR
};


// Function prototypes
void nRF24_Init(void);

uint8_t nRF24_Check(void);

void nRF24_SetPowerMode(uint8_t mode);

void nRF24_SetOperationalMode(uint8_t mode);

void nRF24_SetRFChannel(uint8_t channel);

void nRF24_SetAutoRetr(uint8_t ard, uint8_t arc);

void nRF24_SetAddrWidth(uint8_t addr_width);

void nRF24_SetAddr(uint8_t pipe, const uint8_t *addr);

void nRF24_SetTXPower(uint8_t tx_pwr);

void nRF24_SetDataRate(uint8_t data_rate);

void nRF24_SetCRCScheme(uint8_t scheme);

void nRF24_SetRXPipe(uint8_t pipe, uint8_t aa_state, uint8_t payload_len);

void nRF24_ClosePipe(uint8_t pipe);

void nRF24_EnableAA(uint8_t pipe);

void nRF24_DisableAA(uint8_t pipe);

void nRF24_SetDynamicPayloadLength(uint8_t mode);

void nRF24_SetPayloadWithAck(uint8_t mode);

uint8_t nRF24_GetStatus(void);

uint8_t nRF24_GetIRQFlags(void);

uint8_t nRF24_GetStatus_RXFIFO(void);

uint8_t nRF24_GetStatus_TXFIFO(void);

uint8_t nRF24_GetRXSource(void);

uint8_t nRF24_GetRetransmitCounters(void);

uint8_t nRF24_GetFeatures(void);

void nRF24_ResetPLOS(void);

void nRF24_FlushTX(void);

void nRF24_FlushRX(void);

void nRF24_ClearIRQFlags(void);

void nRF24_ActivateFeatures(void);

void nRF24_WritePayload(uint8_t *pBuf, uint8_t length);

void nRF24_WriteAckPayload(nRF24_RXResult pipe, char *payload, uint8_t length);

nRF24_RXResult nRF24_ReadPayload(uint8_t *pBuf, uint8_t *length);

nRF24_RXResult nRF24_ReadPayloadDpl(uint8_t *pBuf, uint8_t *length);

#define nRF24_RX_ON()   nRF24_CE_H();
#define nRF24_RX_OFF()  nRF24_CE_L();
#ifdef __cplusplus
}
#endif

#endif // __NRF24_H
