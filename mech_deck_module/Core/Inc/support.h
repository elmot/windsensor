//
// Created by ilia.motornyi on 13-Dec-18.
//

#ifndef __SUPPORT_H
#define __SUPPORT_H

#include "main.h"
#ifdef __cplusplus
extern "C" {
#endif

static inline void nRF24_CE_L() {
    LL_GPIO_ResetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin);
}

static inline void nRF24_CE_H() {
    LL_GPIO_SetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin);
}

static inline void nRF24_CSN_L() {
    LL_GPIO_ResetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin);
}

static inline void nRF24_CSN_H() {
    LL_GPIO_SetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin);
}

uint8_t nRF24_LL_RW(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif //__SUPPORT_H
