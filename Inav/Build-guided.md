# Building INAV firmware

This document gives the basic steps to set up the environment and configure an INAV firmware build for a custom flight controller (FC) board.

## 1. Install the required tools (dependencies)

First, install the build tools and the source control tools. Run the following in a terminal:

```bash
sudo apt update
sudo apt install gcc-arm-none-eabi make git dfu-util
```

## 2. Get the INAV source code

Download the INAV source code from the official GitHub repository and switch to the `master` branch (or the release branch you want):

```bash 
git clone https://github.com/iNavFlight/inav.git
cd inav 
git checkout master
```

## 3. Create the configuration directory for the flight controller (target)

To compile firmware for a specific FC board (for example `SaolaH743`), create a new target directory holding the hardware settings for that board:

```bash
mkdir -p src/main/target/SaolaH743
cd src/main/target/SaolaH743/
touch target.h target.c CMakeLists.txt
```

> **Note:** The target directory is usually the flight controller's name written as one word, and is used as the build command name later.

### Structure of a target directory

Inside the new target directory (`src/main/target/SaolaH743/`) you need to create the configuration files. The essential files and their roles are:

```text
src/main/target/SaolaH743/
├── CMakeLists.txt    # Declares the chip (MCU) type and the build flags
├── target.h          # Declares and defines every pin and peripheral
├── target.c          # Board-specific initialisation code (usually minimal)
└── config.c          # (Optional) Default configuration applied when the firmware is first flashed
```


## 4. Building (compiling) the firmware

The build uses CMake. The essentials you need are `gcc-arm-none-eabi`, `make` and `cmake`.

Run the following commands in a terminal:

```bash
# 1. Go back to the root directory of the INAV project
cd /path/to/inav

# 2. Create the build directory and change into it
mkdir -p build && cd build

# 3. Generate the Makefiles (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. Build (replace 'SaolaH743' with your target name)
make SaolaH743
```

> **Build notes:** After `make` runs, CMake invokes the ARM GCC toolchain. On success, INAV produces an executable `.elf` at `build/bin/SaolaH743.elf`, which is then automatically converted to a hex file: **`build/inav_SaolaH743.hex`**.

## 5. Flashing the firmware to the board

Once you have the `.hex` file, you can flash it to the flight controller:
1. Connect the board to the computer with a USB cable.
2. Open **INAV Configurator**.
3. Go to the **Firmware Flasher** tab in the left-hand menu.
4. Click **Load firmware [Local]** (load a firmware file from this computer).
5. Select the `inav_SaolaH743.hex` file you just built.
6. Click **Flash Firmware**.
   *If the COM port (VCP) is not working yet because the board is brand new, hold the physical **BOOT** button on the board while plugging in USB to enter **DFU mode**, then flash.*


Checks before building:

Check whether INAV supports the BMI270
cd inav 
grep -r "BMI270" src/main/ --include="*.h" --include="*.c" -l

 Check that SystemClock_Config is not overridden
 grep -r "SystemClock_Config" src/main/ --include="*.c" -l


# Build and flash

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make SaolaH743 -j$(nproc) 2>&1 | tee ../build.log
tail -20 ../build.log
