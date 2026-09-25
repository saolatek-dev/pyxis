# Saolah743 Flight Controller

The Saolah743 is a DIY flight controller based on the STM32H743.

## Features

- STM32H743 microcontroller
- BMI088/BMI270 dual IMUs
- DPS368 barometer (earlier boards: DPS310), read by the DPS310 driver
- IST8310 / QMC5883L magnetometer
- AT7456E OSD
- MicroSD Card Slot
- 7 UARTs
- 10 PWM outputs
- 1 CAN
- 2 I2C
- 1 SWD

## UART Mapping

- SERIAL0 -> USB
- SERIAL1 -> UART1 (TELEM1)
- SERIAL2 -> UART2 (AIR UNIT / DisplayPort)
- SERIAL3 -> UART3 (GPS)
- SERIAL4 -> UART4 (TELEM2)
- SERIAL5 -> UART6 (RC_INPUT)
- SERIAL6 -> UART7 (RX only, ESC Telemetry)
- SERIAL7 -> UART8 (TELEM3)

## RC Input

The default RC input is configured on UART6. The SBUS pin is inverted and
connected to RX6. Non-SBUS, single wire serial inputs can be directly tied
to RX6 if the SBUS pin is left unconnected.

## PWM Output

The Saolah743 supports up to 10 PWM outputs. All channels support DShot,
and channels 1, 3 and 5 support bi-directional DShot.

PWM outputs are grouped and every group must use the same output protocol:

- 1, 2, 3, 4 are Group 1
- 5, 6 are Group 2
- 7, 8, 9, 10 are Group 3

## Battery Monitoring

The default battery parameters are:

- BATT_MONITOR 4
- BATT_VOLT_PIN 10
- BATT_CURR_PIN 11
- BATT_VOLT_MULT 11.0
- BATT_AMP_PERVLT 40.2

## Compass

The Saolah743 has a built-in compass sensor (IST8310 or QMC5883L,
auto-detected), and also supports an external compass on the I2C1
connector.

## Loading Firmware

Initial firmware load can be done with DFU by plugging in USB with the
bootloader button pressed (or via SWD), loading the "with_bl.hex" firmware.
Once the initial firmware is loaded you can update the firmware using any
ArduPilot ground station software with the "*.apj" firmware files.
