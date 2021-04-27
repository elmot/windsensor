ERM-2401282->UART bridge
===

Based on: STM8S-Discovery board, STM8S105K3 MCU
---

**Warning** PB2 line is brocken patched to PD2

Pinout:
 * PB0..PB2/PD2..PB7 => Display D0..D7
 * TIM1_CH4/PC4      => Backlight PWM
 * UART2_RX/PD6      => UART Data IN
 * PD1               => SWIM flashing interface
 * PA3               => !RST
 * PA4               => C/!D
 * PA5               => !WR
 * PA6               => !RD

UART Protocol
---

*0xAA 0xA0* <**0..99 Brightness**> + <**data stream**>

*0xAA* byte in the data stream should be replaced with *0x55 0x54* sequence