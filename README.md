# Pyxis FC

![MCU](https://img.shields.io/badge/MCU-STM32H743VIT6-03234b?style=flat-square)
![Targets](https://img.shields.io/badge/targets-ArduPilot%20%7C%20INAV%20%7C%20Betaflight%20%7C%20PX4-4c6b8a?style=flat-square)
![Build host](https://img.shields.io/badge/build%20host-Ubuntu%2024.04%20%2F%20WSL2-555?style=flat-square)
![License](https://img.shields.io/badge/license-GPL--3.0%20%7C%20BSD--3--Clause-4c6b8a?style=flat-square)

Board support files, build guides and flashing procedures for the **Pyxis** flight controller by Saolatek, covering ArduPilot, INAV, Betaflight and PX4.

![Pyxis FC, an STM32H743 flight controller for professional UAV work](Docs/images/hero.jpg)

## Overview

Pyxis is a 36 × 36 mm flight controller built around the STM32H743VIT6. It is a custom design, so no upstream flight stack includes a target for it. This repository provides:

- An ArduPilot hardware definition with build and flash guides.
- A drop-in INAV target.
- A Betaflight porting guide.
- A complete PX4 board port, including a driver for the DPS368 barometer.
- The pin map, connector drawings and board schematic.

In firmware sources and build commands the board is identified as
**`SaolaH743`**, except in PX4, where the build targets are
**`saolah743_h743`** and **`saolah743_h743_dps368`**.

| Firmware | Provided | Build system |
| --- | --- | --- |
| ArduPilot (ArduCopter) | Hardware definition | waf |
| INAV | Target source | CMake |
| Betaflight | Porting guide | make |
| PX4 | Board port and DPS368 driver | CMake |

## Key features

- STM32H743VIT6 Cortex-M7, rated to 480 MHz, with 2 MB flash and 1 MB RAM.
- Eight serial ports: seven UARTs plus USB VCP.
- Ten motor outputs with DShot; bidirectional DShot on M1, M3 and M5.
- BMI270 IMU, barometer and IST8310 compass on board.
- AT7456E analog OSD.
- Blackbox logging to microSD.
- 2S–8S LiPo input with voltage and current sensing, 9 V and 5 V regulated rails.
- CAN bus (DroneCAN ready), external I²C and SWD debug header.
- Hardware SBUS inverter on the RC input.
- USB DFU flashing, no external programmer required.

## Specifications

| | |
| --- | --- |
| MCU | STM32H743VIT6, Cortex-M7, 480 MHz |
| Memory | 2 MB flash, 1 MB RAM |
| IMU | BMI270 (default) or BMI088 |
| Barometer | DPS368 |
| Magnetometer | IST8310 |
| OSD | AT7456E |
| Input voltage | 2S–8S LiPo |
| Regulators | 9 V TPS54560 buck, 5 V TPS52933 step-down, dedicated LDOs for MCU and IMU |
| Board size | 36 × 36 mm |
| Mounting | 31 × 31 mm |
| Weight | 10 g |

> [!NOTE]
> Boards ship with either a DPS310 or a DPS368 barometer. Confirm which part is fitted to yours. The ArduPilot, INAV and Betaflight definitions here are written for the DPS310; the PX4 port builds both variants (`saolah743_h743` for DPS310, `saolah743_h743_dps368` for DPS368).

## Prerequisites

### Hardware

| Item | Requirement |
| --- | --- |
| Flight controller | Pyxis FC |
| Cable | USB Type-C, data capable |
| Boot mode | Access to the BOOT button |
| Optional | microSD card for logging, SWD probe for debugging |

### Software

| | ArduPilot | INAV | Betaflight | PX4 |
| --- | --- | --- | --- | --- |
| Host OS | Ubuntu 24.04 or WSL2 | Ubuntu or WSL2 | Ubuntu or WSL2 | Ubuntu 24.04 or WSL2 |
| Toolchain | Installed by ArduPilot's setup script | `gcc-arm-none-eabi`, `cmake`, `make` | `gcc-arm-none-eabi`, `make`, or Docker | `gcc-arm-none-eabi`, `cmake`, `ninja-build` |
| Flashing | `dfu-util` | INAV Configurator | Betaflight Configurator or `dfu-util` | STM32CubeProgrammer, `dfu-util` or INAV Configurator for the factory image; QGroundControl for updates |
| Ground station | Mission Planner or QGroundControl | INAV Configurator | Betaflight Configurator | QGroundControl |

## Quick start

**1. Build the firmware** by following the guide for your flight stack:

| Firmware | Guide |
| --- | --- |
| ArduPilot | [Build](Ardupilot/build-guided.md), then [flash](Ardupilot/boot-firmware-guided.md) |
| INAV | [Build and flash](Inav/Build-guided.md) |
| Betaflight | [Port, build and flash](BetaFlight/guide.md) |
| PX4 | [Build](PX4/build-guided.md), then [flash](PX4/flash-guided.md) |

Prebuilt firmware is published under [Releases](../../releases), tagged by
firmware prefix: `Ardupilot-*`, `BetaFlight-*`, `Inav-*`, `PX4-*`. Each
release carries the `SOURCE.md` for that build, pinning the upstream commit
and listing checksums.

**2. Enter DFU mode.**

1. Unplug the USB cable.
2. Press and hold the BOOT button.
3. Plug the board in while holding the button, then release it.

Check that the board is visible:

```bash
sudo dfu-util -l    # lists a device named "STM32 BOOTLOADER"
```

**3. Flash** using the method in your firmware's guide.

**4. Configure** ports, receiver and battery monitoring as described at the end of each guide.

## Usage

### Wiring

See the [wiring diagram](Docs/images/wiring.jpg) and the [connector layout](Docs/images/connectivity.jpg) for both sides of the board.

| Peripheral | Connector | Port |
| --- | --- | --- |
| RC receiver (SBUS, CRSF, ELRS) | SBUS/CRSF | UART6 |
| GPS and external compass | GPS | UART3, I²C1 |
| Telemetry radio | TELEM1 | UART1 |
| Second telemetry link | TELEM2 | UART4 |
| DJI air unit | Back header | UART2 |
| Companion computer | Back header | UART8 |
| ESC | ESC | M1–M4, UART7 telemetry, current sense |
| Analog VTX | VIDEO-OUT | 9 V |
| FPV camera | VIDEO-IN | 9 V |
| CAN peripherals | CAN | CAN1 |

The MCU pin for every function is listed in the [pin map](Docs/pinout.md).

## License

The four firmware directories are **four separate programs**. Each inherits
the license of its upstream project, so licensing applies **per directory**.
There is no repository-wide license.

| Directory | Upstream | License |
| --- | --- | --- |
| [`Ardupilot/`](Ardupilot/) | ArduPilot | [GPL-3.0-or-later](Ardupilot/LICENSE) |
| [`BetaFlight/`](BetaFlight/) | Betaflight | [GPL-3.0-or-later](BetaFlight/LICENSE) |
| [`Inav/`](Inav/) | INAV | [GPL-3.0-or-later](Inav/LICENSE) |
| [`PX4/`](PX4/) | PX4-Autopilot | [BSD-3-Clause](PX4/LICENSE) |

Aggregating GPL and BSD programs in one repository is permitted under the
*mere aggregation* provision of GPLv3 section 5. Code must not be copied
between directories: GPL code moved into `PX4/` would place that port under
GPLv3 irreversibly.

See [COMPLIANCE.md](COMPLIANCE.md) for source availability, the written offer
for pre-installed firmware, and trademark usage.

"ArduPilot", "Betaflight", "INAV" and "PX4" are marks of their respective
projects. Pyxis is compatible with them; it is not affiliated with or endorsed
by them.

## Contact

Pyxis is designed and manufactured by **Saolatek** in Vietnam.

- Website: [saolatek.vn](https://saolatek.vn)
- Email: [contact@saolatek.vn](mailto:contact@saolatek.vn)
- Phone: +84 978 357 681
- Address: SHTP Incubation Center, D2B St. & D1 St., Long Thanh My Ward, Thu Duc City, Ho Chi Minh City
- Related sites: [vietdrone.vn](https://vietdrone.vn) · [vatomus.com](https://vatomus.com)

For firmware and board-support issues, please use this repository's issue tracker.
