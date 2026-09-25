# Building ArduPilot firmware for the Pyxis FC (Saolah743)

If you don't want to build it yourself, download a prebuilt release from
**[Releases](../../../releases)** (tag `Ardupilot-*`) and see
[boot-firmware-guided.md](boot-firmware-guided.md).

> **Note:** Building requires Ubuntu 24.04. WSL2 can be used instead.

## 1. Get the right version of the ArduPilot source

```bash
git clone https://github.com/ArduPilot/ardupilot.git
cd ardupilot
git checkout 1511f27194f1dcc3728270883047bdf022b3fd53
git submodule update --init --recursive
```

This is the commit recorded in [SOURCE.md](SOURCE.md) (ArduPilot 4.7.0). The
patch in step 3 is written against it.

## 2. Set up the build environment

Install the packages required by the build environment:

```bash
Tools/environment_install/install-prereqs-ubuntu.sh -y
```

When installation finishes, reload the environment variables:

```bash
source ~/.profile
```

Check that the build tool works:

```bash
./waf --version
```

## 3. Add the Saolah743 board

A board needs two hardware definition files, both in
`libraries/AP_HAL_ChibiOS/hwdef/Saolah743/`:

| File | Describes |
| --- | --- |
| `hwdef.dat` | The main firmware: pins, sensors, UART order, OSD, SD card |
| `hwdef-bl.dat` | The bootloader: USB, LEDs and the flash layout |

The folder also holds `defaults.parm` (default parameters built into the
firmware) and `README.md` (board description).

Assuming the `pyxis` repository is at `~/pyxis`:

```bash
PYXIS=~/pyxis

cp -r "$PYXIS/Ardupilot/Saolah743" libraries/AP_HAL_ChibiOS/hwdef/
git apply "$PYXIS/Ardupilot/patches/upstream-changes.patch"
```

**The `git apply` step is mandatory.** The patch:

- registers the board ID `AP_HW_Saolah743` (6130) in
  `Tools/AP_Bootloader/board_types.txt`. Without it `waf configure` fails,
  because `hwdef.dat` refers to that ID.
- changes `Tools/AP_Bootloader/bl_protocol.cpp` so that the ArduPilot
  bootloader resets the clock tree, SysTick, NVIC and the CONTROL register
  before jumping to the application. This lets the same bootloader start
  NuttX-based firmware (PX4) as well as ArduPilot.

## 4. Build the bootloader

```bash
Tools/scripts/build_bootloaders.py Saolah743
```

The script builds the bootloader from `hwdef-bl.dat` and copies it to
`Tools/bootloaders/Saolah743_bl.bin` (plus `.hex` and `.elf`). The firmware
build in step 5 embeds that file into `arducopter_with_bl.*`. Skip this step
and the firmware still builds, but `waf` prints
`Not embedding bootloader` and no `_with_bl` files are produced.

## 5. Build the firmware

```bash
./waf configure --board Saolah743
./waf copter
```

Output in `build/Saolah743/bin/`:

| File | Use |
| --- | --- |
| `arducopter.apj` | Update through Mission Planner / QGroundControl |
| `arducopter.bin` | Firmware only, flashed to `0x08020000` |
| `arducopter_with_bl.bin` / `.hex` | Bootloader + firmware, for new boards |

The bootloader on its own is `Tools/bootloaders/Saolah743_bl.bin`, the file
released as `AP_Bootloader.bin`.

## 6. Flash

See [boot-firmware-guided.md](boot-firmware-guided.md).
