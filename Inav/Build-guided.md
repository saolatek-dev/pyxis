# Building INAV firmware for the Pyxis FC (SAOLA_H743)

If you don't want to build it yourself, download `Inav-SAOLA_H743.zip` from
**[Releases](../../../releases)** (tag `Inav-*`), extract it, check the file
against [SOURCE.md](SOURCE.md#released-artifacts) and go to step 5.

## 1. Install the tools

```bash
sudo apt update
sudo apt install git cmake make ruby gcc-arm-none-eabi dfu-util
```

If CMake cannot find a suitable compiler, INAV downloads its own pinned ARM
toolchain into `tools/` on the first `cmake` run.

## 2. Get the right version of the INAV source

```bash
git clone https://github.com/iNavFlight/inav.git
cd inav
git checkout 4939a7ff7cd263b60718080b3655bfae7b589c93
```

This is the commit recorded in [SOURCE.md](SOURCE.md) (INAV 9.1). The driver
patch in step 3 is written against it.

## 3. Add the SAOLA_H743 target

Assuming the `pyxis` repository is at `~/pyxis`:

```bash
PYXIS=~/pyxis

cp -r "$PYXIS/Inav/SAOLA_H743" src/main/target/
git apply "$PYXIS/Inav/patches/upstream-changes.patch"
```

The target directory holds:

```text
src/main/target/SAOLA_H743/
├── CMakeLists.txt    # MCU type (STM32H743xI) and the 8 MHz HSE crystal
├── target.h          # Pins, sensors, UARTs, ADC, default features
└── target.c          # Motor timer map
```

**Apply the patch.** It fixes three shared INAV drivers
(see [SOURCE.md](SOURCE.md)). Without it the firmware still builds, but:

- the board hangs at boot when the AT7456E OSD does not answer on SPI1;
- an SPI timeout on STM32H7 can hang the next SPI transfer forever.

## 4. Build

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make SAOLA_H743 -j$(nproc)
```

The output is `build/inav_<version>_SAOLA_H743.hex`, for example
`inav_9.1.0_SAOLA_H743.hex`.

## 5. Flash

1. Connect the board to the computer with a USB cable.
2. Open **INAV Configurator**.
3. Go to the **Firmware Flasher** tab.
4. Click **Load firmware [Local]** and select the `.hex` file you just built.
5. Click **Flash Firmware**.

If the board is new or runs another flight stack, the COM port may not
appear. Hold the **BOOT** button while plugging in USB to enter **DFU mode**,
then flash. See [../Docs/flashing-setup.md](../Docs/flashing-setup.md) for
the USB drivers.

## 6. First setup

> [!WARNING]
> This target enables `PWM_OUTPUT_ENABLE` by default, so the ESCs receive a
> signal from the first boot. **Remove the propellers** before connecting a
> battery.

- RC input defaults to SBUS on UART6. For a CRSF/ELRS receiver run
  `set serialrx_provider = CRSF` in the CLI.
- ESC telemetry: set UART7 to *ESC Sensor* in the **Ports** tab.
- Battery voltage: `vbat_scale` defaults to 1100 (11:1 divider). Calibrate
  per board with `set vbat_scale = <old_scale * V_meter / V_configurator>`.
