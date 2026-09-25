# PX4 — Saolatek SAOLAH743

PX4 port for the SAOLAH743 flight controller (STM32H743VIT6, 2 MB flash).

## Contents

| Path | Description |
|---|---|
| `build-guided.md` | How to build the firmware from source |
| `flash-guided.md` | How to flash the firmware to the board |
| `SOURCE.md` | Pins the upstream commit and lists our changes (used for releases) |
| `boards/saolah743/h743/` | PX4 board port (25 files) |
| `src/drivers/barometer/dps368/` | Infineon DPS368 barometer driver (written by us) |
| `patches/upstream-changes.patch` | Changes to apply to 3 original PX4 files |
| `LICENSE` | BSD 3-Clause |

## Prebuilt firmware

If you don't want to build it yourself, download it from **[Releases](../../../releases)** — pick a tag with the `PX4-` prefix.

Each release contains:

| File | When to use it |
|---|---|
| `*_factory.hex` / `.bin` | **New/blank board** — bootloader and firmware combined, flash once and it runs |
| `*.px4` | Board **already has** the PX4 bootloader — update through QGroundControl |
| `saolah743_h743_bootloader.bin` | Flash the bootloader on its own |

Two variants, depending on the barometer fitted to the board:

- `default` → **DPS310** barometer
- `dps368` → **DPS368** barometer

The IMU **does not need to be chosen** — both builds detect BMI088/BMI270 at boot, so the same firmware works on 1-IMU and 2-IMU boards.

## Identifiers

| | |
|---|---|
| Board ID | `6130` (bootloader and firmware must match) |
| USB VID:PID | `0x1209:0x7743` |
| Bootloader | `0x08000000`, 128 KiB |
| Firmware | `0x08020000`, up to 1792 KiB |
| Parameters | sector 15, `0x081E0000` |

## License

PX4-Autopilot uses **BSD 3-Clause**. This port is a derivative work and is therefore also BSD 3-Clause — see [LICENSE](LICENSE). Files keep the original PX4 Development Team copyright lines.

> Note: this differs from ArduPilot/Betaflight/INAV in this repository (all **GPLv3**). Do not copy code between the two — GPL code that ends up in the `PX4/` directory would force this whole port to become GPLv3.
