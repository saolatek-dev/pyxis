# SaolaH743 INAV Firmware

This project contains the INAV firmware configuration for the custom **SaolaH743** flight controller board. The board uses the STM32H743 microcontroller and is designed to provide high performance and a full set of peripherals for camera drones and multirotors.

## Features and hardware configuration

The basic hardware specifications of the board, based on the pin map built into the INAV target:

*   **Microcontroller (MCU):** STM32H743
*   **Accelerometer/gyro (IMU):** BMI270 (on SPI2)
*   **Barometer:** DPS310 (on internal I2C2)
*   **Compass:** IST8310 or QMC5883L supported (on internal I2C2)
*   **OSD:** AT7456E (on SPI1)
*   **Flight logging (Blackbox):** SD card over SDMMC
*   **Serial ports (UARTs):** 8 physical UARTs + 1 USB VCP
    *   `UART1`: TELEM1
    *   `UART2`: DJIO3
    *   `UART3`: GPS
    *   `UART4`: TELEM2
    *   `UART6`: RC INPUT (hardware SBUS inverter on pin `PD0`)
    *   `UART7`: ESC telemetry
    *   `UART8`: TELEM3
*   **I2C ports:**
    *   `I2C1`: for externally connected peripherals (external connector)
    *   `I2C2`: for internal sensors (internal baro/mag)
*   **Motor/servo outputs (PWM):** up to 10 outputs, with DSHOT and DMAR support.
    *   `M1-M4`: TIM1
    *   `M5-M6`: TIM3
    *   `M7-M10`: TIM4
*   **Power sensing (ADC):**
    *   Voltage: pin `PC0` (scale: 21.12)
    *   Current: pin `PC1` (scale: 40.2)

For the detailed pinout of each peripheral, see [pinout.txt](./pinout.txt).

## Building the firmware (compiling)

Setting up the environment and compiling the source are described in detail in [Build-guided.md](./Build-guided.md).

### Summary of the basic steps:

1. **Install the required tools:** `gcc-arm-none-eabi`, `make`, `cmake`, `git`.
2. **Get the INAV source code:**
   ```bash
   git clone https://github.com/iNavFlight/inav.git
   cd inav
   git checkout master
   ```
3. **Prepare the target directory:**
   Copy the `SaolaH743` configuration directory from this project (if it contains the code files) into `src/main/target/SaolaH743/` in the INAV source you just downloaded.
4. **Build:**
   ```bash
   mkdir -p build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make SaolaH743 -j$(nproc)
   ```
   On success, the firmware file is written to `build/inav_SaolaH743.hex`.

## Flashing the firmware

1. Connect the SaolaH743 board to the computer over USB Type-C.
2. Open **INAV Configurator**.
3. Go to the **Firmware Flasher** tab in the left-hand menu.
4. Click **Load firmware [Local]** and select the `inav_SaolaH743.hex` file you built.
5. Click **Flash Firmware** to start flashing.
   *(If this is a blank board, hold the **BOOT** button while plugging in USB to enter DFU mode.)*
