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

> [!NOTE]
> First time flashing on this computer? Install the tools and USB drivers in
> [flashing-setup.md](../Docs/flashing-setup.md) first (Ubuntu, Windows and WSL2).

### 5.1 Get the firmware file

**Prebuilt (recommended):** open **[Releases](../../../releases)**, pick the
newest tag starting with `BetaFlight-`, download the zip (for example
`bmi270.zip`) and extract it. It contains:

| File | Tool |
|---|---|
| `betaflight_<version>_STM32H743_SAOLAH743_BMI270.hex` | Betaflight Configurator, STM32CubeProgrammer |
| `betaflight_<version>_STM32H743_SAOLAH743_BMI270.dfu` | dfu-util |

Optional: check the download with `md5sum <file>` and compare with the table in
[SOURCE.md](SOURCE.md).

> [!IMPORTANT]
> The release build is for boards with the **BMI270** IMU (the name ends in
> `_BMI270`). Do not flash it to a board fitted with a BMI088.

**Built yourself:** the `.hex` file is in the `obj/` folder of the Betaflight tree.

### 5.2 Enter DFU mode

1. Unplug the USB cable.
2. Press and hold the **BOOT** button.
3. Plug the USB cable in while holding the button, then **release it**.

Check that the board is visible (Linux):

```bash
sudo dfu-util -l    # must list a device with ID 0483:df11 ("STM32 BOOTLOADER")
```

### 5.3 Flash

**Method 1 — Betaflight Configurator (Windows / Linux / macOS):**

1. Open the **Firmware Flasher** tab. The port selector shows **DFU**.
2. Click **Load Firmware [Local]** and select the `.hex` file.
3. Optional: enable **Full chip erase** when coming from another firmware (ArduPilot, INAV, PX4).
4. Click **Flash Firmware** and wait for *Programming: SUCCESSFUL*.

**Method 2 — dfu-util (Linux / WSL):** use the **`.dfu`** file. dfu-util cannot read `.hex` files.

```bash
sudo dfu-util -a 0 -s :leave -D betaflight_<version>_STM32H743_SAOLAH743_BMI270.dfu
```

Wait for `File downloaded successfully`. The board restarts by itself. If it
does not, unplug and replug USB.

After flashing, the board shows up as a serial port (`/dev/ttyACM*` on Linux, a
COM port on Windows). Connect with Betaflight Configurator.

**If the board does not show up:** unplug USB, make sure BOOT is **not** pressed,
and plug it back in. If `dfu-util -l` still lists `0483:df11`, the board is still
in DFU mode.

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
