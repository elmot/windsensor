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

ERM240128-2 + STM8S-Discovery(Display)
---

[See stm8-workspace](../stm8-workspace/readme.md)

| LCM Name  | MCPU Pin  | Nucleo Pin     |
|-----------|-----------|----------------|
| VSS       | GND       | CN7-20         |
| VDD       |**+5**     | CN7-18         |
| DIN       | PC10      | CN7-1          |

UARTS
---

| UART Name |  MCPU Pin | Nucleo Pin |
|-----------|-----------|------------|
| USART2_TX |  PA2	    | CN10-35    |
| USART2_RX |  PA3	    | CN10-37    |
| UART4_TX  |  PC10     | CN7-1      |
| UART4_RX  |  PC11     | CN7-2      |
| USART1_TX	|  PB6	    | CN10-17    |
| USART1_RX	|  PB7	    | CN7-21     |

Keys/switches (all set to pull-up)
---
|      Name    |  MCPU Pin | Nucleo Pin |
|--------------|-----------|------------|
| NAV LIGHT    | PA0	   | CN7-28     |
| ANCHOR LIGHT | PA1	   | CN7-30     |
| UP           | PB4	   | CN10-27    |
| DOWN         | PB5	   | CN10-29    |
| OK           | PB3	   | CN10-31    |
| CANCEL       | PA10	   | CN10-33    |

Misc
---
|      Name |  MCPU Pin | Nucleo Pin |
|-----------|-----------|------------|
|  BLUE BTN |  PC13	    | CN7-23     |  
|  GRN LED  |  PA5 	    | CN10-11    |  

Radio transimssion format
====

Deck -> Mast
----
Symbolic button state + ';' + 8 hex digits of unique device id

*B1;abcde123* -> Everything is off

*B2;abcde123* -> Navigational lights on

*B3;abcde123* -> Anchor lights on

Mast -> Deck
----

*b;agc;angle;wind_ticks_length;mvolts;device_id*

*b* - char '1', '2' or '3'. Button state, same as coming from deck module

*agc* - automatic sensor gain control, hexadecimal value
0 means the magnet is too close to sensor; 0x80 means the magnet is too far, value is invalid in both cases

*angle* - wind angle, decimal, degrees

*wind_ticks_length* - rotation tick length, hexadecimal
Unit is 100 microseconds, 8 ticks per wind speed rotor revolution

*mvolts* - ADC mV, decimal, values in range 0..3300

*device_id* - 8 hex digits of unique device id. Usually calculated from
manufacturers unique device id (96 bits = 3 longs)  by xoring three words together

Example:

*2;4e;254;5ef;2200*

[Full STM32CubeXM report](mech_deck_module.pdf)

WIFI
===

ESP8266EX, 4Mbit
SSID="EL_NAVI"

TODO
===
* Solder top board
* Keys easier connect
* WIFI
