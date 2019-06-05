On-mast part of mechanical anemomether
====


Pinouts
===

nRF24L01 (Radio), connected to SPI1
---

|nRF Name | nRF Pin | MCPU Pin |
|---------|---------|----------|
| GND     |       1 | GND      |
| VCC     |       2 | +3.3V    |
| CE      |       3 | PB0      |
| CSN     |       4 | PA4      |
| SCK     |       5 | PA5      |
| MOSI    |       6 | PA7      |
| MISO    |       7 | PA6      |
| IRQ     |       8 | PB1      |

AS5601 - Wind Angle Sensor, connected to I<sub>2</sub>C1
---
|  Name   | MCPU Pin |
|---------|----------|
| SDA     | PB7      |
| SCL     | PB6      |


AS5601 - Wind Speed Sensor, connected to TIM1_CH1
---
29	PA8	TIM1_CH1	WIND_TICK 

| Sensor<br>Pin Name | MCPU Pin |
|--------------------|----------|
| B                  | PA8      |

Diagnostics LED
---
Blue, Connected to PB12

Signals:

|  Blink type                   |  State              |
|-------------------------------|---------------------|
| Once per two rotor rotations  |OK                   |
| Fast blinking series, 30/30ms |Magnet fail          |
| 400ms on, 50ms off            |Radio module fail    |
| 200ms on, 50ms off            |General failure      |
| 130ms on, 30ms off            |Angle sensor failure |

Nav Lights control
---
|      Name    |  MCPU Pin |
|--------------|-----------|
| ANCHOR LIGHT | PB13      |
| ENABLE DC/DC | PB14      |
| NAVI LIGHT   | PB15      |

Radio transimssion format
====
See deck module  readme

MCU Configs
---
[Full STM32CubeXM report](mech_mast_module.pdf)

