# Building Betaflight for the SaolaH743 board

## Step 1 — Clone the source code

```bash
git clone https://github.com/betaflight/betaflight.git
cd betaflight
```

## Step 2 — Create the target directory

```bash
mkdir src/main/target/SAOLAH743
```

Create 2 files inside it: `CMakeLists.txt` and `target.h`.

**`CMakeLists.txt`**:
```cmake
target_stm32h743xx(SAOLAH743)
```

## Step 3 — Write `target.h`

Map the pins according to the original hwdef (UART, SPI, I2C, IMU, baro, mag, OSD, motors, ADC...). See the full content in the previous answer.

## Step 4 — Install the toolchain and build

**Install the ARM toolchain (Ubuntu/WSL):**
```bash
sudo apt install gcc-arm-none-eabi
```

**Build:**
```bash
make SAOLAH743
```

Output file: `obj/betaflight_SAOLAH743.hex`

**Or build with Docker (no toolchain install needed):**
```bash
docker run --rm -v $(pwd):/betaflight betaflight/build SAOLAH743
```

## Step 5 — Flash the firmware

Enter DFU mode: hold the **BOOT0** button, then plug in USB.

**Method 1 — Betaflight Configurator:**
Firmware Flasher tab → Load Firmware (Local) → select the `.hex` file

**Method 2 — dfu-util:**
```bash
dfu-util -D obj/betaflight_SAOLAH743.hex
```

## Step 6 — Verify

Open Betaflight Configurator and check:
- The IMU works (BMI088 / BMI270)
- The UARTs send and receive correctly (TELEM1, TELEM2, GPS, RC input)
- Motor output (DSHOT)
- The OSD displays (MAX7456)

---

## Important notes

The original `.hwdef` file is in **ArduPilot** format, not Betaflight. The two firmwares use completely different build and configuration systems:

| | ArduPilot | Betaflight |
|---|---|---|
| Configuration files | `hwdef.dat` | `target.h` + `CMakeLists.txt` |
| Build system | waf | make / CMake |
| Location | `libraries/AP_HAL_ChibiOS/hwdef/` | `src/main/target/` |

The pin assignments in `target.h` in Step 3 are translated directly from the ArduPilot hwdef file you supplied, and apply to the **STM32H743** MCU (H7 series).
