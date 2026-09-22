# Pyxis FC Pin Map

MCU pin assignments for the Pyxis flight controller (target name `SaolaH743`). All firmware definitions in this repository are derived from this table.

For connector positions on the board, see the [connector layout](images/connectivity.jpg).

## Serial

| Port | TX | RX | Connector | Default role |
| --- | --- | --- | --- | --- |
| USB (OTG FS) | `PA12` DP | `PA11` DM | USB-C | Configurator, MAVLink |
| UART1 | `PA9` | `PA10` | TELEM1 | Telemetry |
| UART2 | `PA2` | `PA3` | Air unit (back) | DJI air unit |
| UART3 | `PD8` | `PD9` | GPS | GPS |
| UART4 | `PA0` | `PA1` | TELEM2 | Telemetry |
| UART6 | `PC6` | `PC7` | SBUS/CRSF | RC input |
| UART7 | — | `PE7` | ESC | ESC telemetry |
| UART8 | `PE1` | `PE0` | Back header | TELEM3, companion computer |

The board has seven UARTs; there is no UART5. `PD0` drives the hardware SBUS inverter on UART6 and is held low at boot.

## SPI

| Bus | SCK | MISO | MOSI | Devices |
| --- | --- | --- | --- | --- |
| SPI1 | `PA5` | `PA6` | `PA7` | AT7456E OSD, CS `PB12` |
| SPI2 | `PD3` | `PC2` | `PC3` | BMI270, CS `PA15`; BMI088 accel CS `PD4`, gyro CS `PD5` |

## I²C

| Bus | SCL | SDA | Use |
| --- | --- | --- | --- |
| I²C1 | `PB6` | `PB7` | External connector, GPS compass |
| I²C2 | `PB10` | `PB11` | On-board sensors, internal pull-ups enabled |

| Device | Bus | Address |
| --- | --- | --- |
| DPS310 barometer | I²C2 | `0x76` or `0x77` |
| IST8310 compass | I²C2 | `0x0E` |
| QMC5883L compass | I²C2 | `0x0D` |

## Motor outputs

| Output | Pin | Timer | Bidirectional DShot |
| --- | --- | --- | :---: |
| M1 | `PE14` | TIM1_CH4 | yes |
| M2 | `PE13` | TIM1_CH3 | |
| M3 | `PE11` | TIM1_CH2 | yes |
| M4 | `PE9` | TIM1_CH1 | |
| M5 | `PB1` | TIM3_CH4 | yes |
| M6 | `PB0` | TIM3_CH3 | |
| M7 | `PD12` | TIM4_CH1 | |
| M8 | `PD13` | TIM4_CH2 | |
| M9 | `PD14` | TIM4_CH3 | |
| M10 | `PD15` | TIM4_CH4 | |

Outputs on the same timer share a protocol and update rate.

## Analog

| Signal | Pin | ADC | Scale |
| --- | --- | --- | --- |
| Battery voltage | `PC0` | ADC1 | 21.12 |
| Battery current | `PC1` | ADC1 | 40.2 |

Scale values are in ArduPilot's format (`BATT_VOLT_MULT`, `BATT_AMP_PERVLT`).

## Other

| Function | Pins | Notes |
| --- | --- | --- |
| microSD | `PC12` CK, `PD2` CMD, `PC8`–`PC11` D0–D3 | SDMMC1, 4-bit |
| CAN1 | `PB8` RX, `PB9` TX | DroneCAN |
| RGB LED | `PE5` red, `PE6` green, `PE4` blue | Active low |
| SWD | `PA13` SWDIO, `PA14` SWCLK | Debug header |

## Related documents

- [Pinout-inav.txt](Pinout-inav.txt) — plain-text version of this table
- [SAOLAH743_IO_Port_Reference.docx](SAOLAH743_IO_Port_Reference.docx)
- [h7fc.pdf](h7fc.pdf) — board schematic
