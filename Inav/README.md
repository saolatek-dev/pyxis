# INAV firmware for the Pyxis FC (SAOLA_H743)

This directory holds the INAV target for the **Pyxis** flight controller
(INAV target name **`SAOLA_H743`**), built around the STM32H743.

## Hardware configuration

The hardware as declared in the INAV target:

*   **Microcontroller (MCU):** STM32H743, 8 MHz HSE crystal
*   **IMU:** BMI088 or BMI270 on SPI2, detected at boot (BMI088 is used when fitted)
*   **Barometer:** DPS310 or DPS368 on internal I2C2 (same driver)
*   **Compass:** IST8310 or QMC5883L on internal I2C2
*   **OSD:** AT7456E on SPI1, CS `PD11`
*   **Flight logging (Blackbox):** SD card over SDMMC1, enabled by default
*   **Serial ports:** 7 UARTs + USB VCP
    *   `UART1`: TELEM1
    *   `UART2`: DJI O3 / TELEM2 (the two connectors share UART2)
    *   `UART3`: GPS
    *   `UART4`: spare (UART4 connector)
    *   `UART6`: RC input, SBUS by default (inverted in the UART itself; the board has no hardware inverter)
    *   `UART7`: ESC telemetry (RX only)
    *   `UART8`: companion computer
*   **I2C:** `I2C1` external connector, `I2C2` internal baro/mag
*   **Motor outputs:** 10 outputs, DSHOT on M1–M9, M10 PWM only
    *   `M1-M4`: TIM1
    *   `M5-M6`: TIM3
    *   `M7-M10`: TIM4
*   **RGB LED:** `PE5` red, `PE6` green, `PE4` blue, active high
*   **CAN:** CAN1 on `PB8`/`PB9`
*   **Power sensing (ADC):**
    *   Voltage: `PC0`, 11:1 divider, `vbat_scale` = 1100
    *   Current: `PC1`, `current_meter_scale` = 4020

For every pin, see [pinout.txt](./pinout.txt).

## Building the firmware

Full instructions: [Build-guided.md](./Build-guided.md). In short:

```bash
git clone https://github.com/iNavFlight/inav.git
cd inav
git checkout 4939a7ff7cd263b60718080b3655bfae7b589c93

PYXIS=/path/to/pyxis
cp -r "$PYXIS/Inav/SAOLA_H743" src/main/target/
git apply "$PYXIS/Inav/patches/upstream-changes.patch"

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make SAOLA_H743 -j$(nproc)
```

On success, the firmware file is `build/inav_<version>_SAOLA_H743.hex`.

## Flashing the firmware

1. Connect the board to the computer over USB Type-C.
2. Open **INAV Configurator**.
3. Go to the **Firmware Flasher** tab.
4. Click **Load firmware [Local]** and select the `.hex` file you built.
5. Click **Flash Firmware**.
   *(If this is a blank board, hold the **BOOT** button while plugging in USB to enter DFU mode.)*

> [!WARNING]
> Motor output is enabled by default in this target. Remove the propellers
> before connecting a battery.
