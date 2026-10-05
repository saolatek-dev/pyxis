# Pyxis FC Pin Map

MCU pin assignments for the Pyxis flight controller (ArduPilot `Saolah743`, INAV `SAOLA_H743`, Betaflight `SAOLAH743`, PX4 `saolah743_h743`), taken from the board schematic `h7_fc.kicad_sch`. The firmware definitions in this repository must match this table.

> [!WARNING]
> The current firmware sources in this repository match this table. The released binaries `Ardupilot-v0.0.3` and `PX4-v0.0.3` were built before some definitions were fixed (OSD chip select, battery voltage scale, LED polarity); see each directory's `SOURCE.md`. `BetaFlight-v0.0.3` is built from the current sources.

For connector positions on the board, see the [connector layout](images/connectivity.jpg).

## Serial

| Port | TX | RX | Connector | Default role |
| --- | --- | --- | --- | --- |
| USB (OTG FS) | `PA12` DP | `PA11` DM | USB-C | Configurator, MAVLink |
| UART1 | `PA9` | `PA10` | TELEM1 | Telemetry |
| UART2 | `PA2` | `PA3` | AIR UNIT **and** TELEM2 (shared) | Air unit or second telemetry link |
| UART3 | `PD8` | `PD9` | GPS | GPS |
| UART4 | `PA0` | `PA1` | UART4 | Spare |
| UART6 | `PC6` | `PC7` | SBUS/CRSF | RC input |
| UART7 | — | `PE7` | PWM1 (ESC), pin 7 | ESC telemetry |
| UART8 | `PE1` | `PE0` | UART8 | Companion computer |

The board has seven UARTs; there is no UART5.

The AIR UNIT and TELEM2 connectors are wired to the same UART2 pins. Use only one of them at a time.

There is **no hardware SBUS inverter** (`PD0` is not connected). CRSF/ELRS work on UART6 directly. SBUS relies on the STM32H7 UART's built-in RX inversion, which the flight stack must enable in software. Pin 6 of the AIR UNIT connector (`SBUS`) is not connected to the MCU.

### Connector pin order

| Connector | Pin 1 | Pin 2 | Pin 3 | Pin 4 | Pin 5 | Pin 6 |
| --- | --- | --- | --- | --- | --- | --- |
| TELEM1 | GND | 5V | **RX** (`PA10`) | **TX** (`PA9`) | | |
| TELEM2 | GND | 5V | TX | RX | | |
| UART4, UART8, SBUS/CRSF | GND | 5V | TX | RX | | |
| GPS | GND | 5V | TX | RX | SCL (I²C1) | SDA (I²C1) |
| AIR UNIT | 9V | GND | TX | RX | GND | SBUS (not connected) |

TELEM1 is the only connector with RX on pin 3 and TX on pin 4.

## SPI

| Bus | SCK | MISO | MOSI | Devices |
| --- | --- | --- | --- | --- |
| SPI1 | `PA5` | `PA6` | `PA7` | AT7456E OSD, CS `PD11` |
| SPI2 | `PD3` | `PC2` | `PC3` | BMI270, CS `PA15`; BMI088 accel CS `PD4`, gyro CS `PD5` |

## I²C

| Bus | SCL | SDA | Use |
| --- | --- | --- | --- |
| I²C1 | `PB6` | `PB7` | External connector, GPS compass; 10 kΩ pull-ups to 5 V |
| I²C2 | `PB10` | `PB11` | On-board sensors; 10 kΩ pull-ups to 3.3 V |

| Device | Bus | Address |
| --- | --- | --- |
| DPS368 barometer (earlier boards: DPS310) | I²C2 | `0x76` (SDO tied to GND) |
| IST8310 compass | I²C2 | `0x0E` |

The firmware configs name the barometer `DPS310`. That is intentional: the DPS368 has the same register map and Product ID (`0x10`), and every flight stack here reads it with its DPS310 driver.

## Motor outputs

Each output has a 1 kΩ series resistor.

| Output | Pin | Timer | Bidirectional DShot | Connector |
| --- | --- | --- | :---: | --- |
| M1 | `PE14` | TIM1_CH4 | yes | PWM1 pin 3 |
| M2 | `PE13` | TIM1_CH3 | | PWM1 pin 4 |
| M3 | `PE11` | TIM1_CH2 | yes | PWM1 pin 5 |
| M4 | `PE9` | TIM1_CH1 | | PWM1 pin 6 |
| M5 | `PB1` | TIM3_CH4 | yes | PWM2 pin 3 |
| M6 | `PB0` | TIM3_CH3 | | PWM2 pin 4 |
| M7 | `PD12` | TIM4_CH1 | | PWM2 pin 5 |
| M8 | `PD13` | TIM4_CH2 | | PWM2 pin 6 |
| M9 | `PD14` | TIM4_CH3 | | PWM2 pin 7 |
| M10 | `PD15` | TIM4_CH4 | | PWM2 pin 8 |

Outputs on the same timer share a protocol and update rate.

PWM1 (8 pins): GND, VBAT, M1–M4, ESC telemetry (UART7 RX), current sense.
PWM2 (8 pins): GND, VBAT, M5–M10.

## Analog

| Signal | Pin | ADC | Scale |
| --- | --- | --- | --- |
| Battery voltage | `PC0` | ADC1 | 11.0 (100 kΩ / 10 kΩ divider) |
| Battery current | `PC1` | ADC1 | 40.2 (depends on the ESC's current sensor) |

Scale values are in ArduPilot's format (`BATT_VOLT_MULT`, `BATT_AMP_PERVLT`). In PX4 the voltage scale is `BAT1_V_DIV`; in Betaflight it is `vbat_scale` = 110, in INAV `vbat_scale` = 1100.

## Other

| Function | Pins | Notes |
| --- | --- | --- |
| HSE crystal | `PH0`, `PH1` | Y1, 3.2 × 2.5 mm |
| microSD | `PC12` CK, `PD2` CMD, `PC8`–`PC11` D0–D3 | SDMMC1, 4-bit |
| CAN1 | `PB8` RX, `PB9` TX | DroneCAN, MAX3051 transceiver, 120 Ω termination on board |
| RGB LED | `PE5` red, `PE6` green, `PE4` blue | **Active high** (pin high = LED on) |
| BOOT | `BOOT0` | Button to 3.3 V, 10 kΩ pull-down |
| SWD | `PA13` SWDIO, `PA14` SWCLK | Header pin order: **5V**, GND, SWDIO, SWCLK. NRST is not routed. |

## Related documents

- [Pinout-inav.txt](Pinout-inav.txt) — plain-text version of this table
- [SAOLAH743_IO_Port_Reference.docx](SAOLAH743_IO_Port_Reference.docx)
- [h7fc.pdf](h7fc.pdf) — board schematic
