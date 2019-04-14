On-deck part of mechanical anemomether
====


Pinouts
===

nRF24L01 (Radio)
---

|nRF Name | nRF Pin | MCPU Pin | Nucleo Pin |
|---------|---------|----------|------------|
| GND     |       1 | GND      | CN10-20    |
| VCC     |       2 | +3.3V    | CN7-16     |
| CE      |       3 | PB11     | CN10-18    |
| CSN     |       4 | PB12     | CN10-16    |
| SCK     |       5 | PB13     | CN10-30    |
| MOSI    |       6 | PB15     | CN10-26    |
| MISO    |       7 | PB14     | CN10-28    |
| IRQ     |       8 | PB10     | CN10-25    |

ERM240128FS-1 (Display)
---

| LCM Name  | LCM Pin | MCPU Pin  | Nucleo Pin     |
|-----------|---------|-----------|----------------|
| FG        |       1 | GND       | CN7-20         |
| VSS       |       2 | GND       | CN7-20         |
| VDD       |       3 |**+5**     | CN7-18         |
| V0        |       4 | ---       |**trim top**    |
| !WR       |       5 | PA11      | CN10-14        |
| !RD       |       6 | PA10      | CN10-33        |
| !CE       |       7 | GND       | CN7-20         |
| C/D       |       8 | PA9       | CN10-21        |
| !RESET    |       9 | PA8       | CN10-23        |
| D0        |      10 | PC0       | CN7-38         |
| D1        |      11 | PC1       | CN7-36         |
| D2        |      12 | PC2       | CN7-35         |
| D3        |      13 | PC3       | CN7-37         |
| D4        |      14 | PC4       | CN10-34        |
| D5        |      15 | PC5       | CN10-6         |
| D6        |      16 | PC6       | CN10-19        |
| D7        |      17 | PC7       | CN10-6         |
| FS        |      18 | GND       | CN7-20         |
| VEE       |      19 | ---       | **trim mid**   |
| LEDA      |      20 | **+5**    | CN7-18         |
| LEDK      |      21 |**FET+PA6**| **FET+CN10-13**|

UARTS 
---

| UART Name |  MCPU Pin | Nucleo Pin |
|-----------|-----------|------------|
| USART2_TX |  PA2	    | CN10-35    |
| USART2_RX |  PA3	    | CN10-37    |
| LPUART1_TX|  PC10     | CN7-1      |
| LPUART1_RX|  PC11     | CN7-2      |
| USART1_TX	|  PB6	    | CN10-17    |
| USART1_RX	|  PB7	    | CN7-21     |

Misc 
---
|      Name |  MCPU Pin | Nucleo Pin |
|-----------|-----------|------------|
|  BLUE BTN |  PC13	    | CN7-23     |  
|  GRN LED  |  PA5 	    | CN10-11    |  

[Full STM32CubeXM report](mech_deck_module.pdf)