# SOURCE — Betaflight for Pyxis (SAOLAH743)

This file records how the released binaries were built, so that anyone can
reproduce them. It satisfies GPLv3 section 6. **Ship it with every release.**

## Upstream

Betaflight builds from two repositories: the main tree and the board-config
tree, which the main tree carries as the `src/config` submodule. Both must be
pinned.

```
Project: Betaflight
URL:     https://github.com/betaflight/betaflight
Commit:  80b0bceb2f1231896c763cb04919f44007fe2e11
Date:    2026-08-17
Version: 2026.12.0-alpha

Project: Betaflight config (src/config submodule)
URL:     https://github.com/betaflight/config
Commit:  036eaa86f69cd24d34c05dcdeb7a005e59c7ca29
Date:    2026-08-16
```

The commit hashes are authoritative. A version number alone does not
identify a unique tree.

The firmware embeds the config revision, which you can check:

```bash
arm-none-eabi-objcopy -I ihex -O binary betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.hex fw.bin
strings fw.bin | grep -E "2026\.12\.0-alpha|036eaa8|norevision"
```

The main-tree revision appears as `norevision`. Betaflight writes this string
whenever the tree has local changes (here, the patch below), so the
`80b0bceb2` commit cannot be read back from the binary. Use this file for it.

## Saolatek changes

### Added

| Path in the Betaflight tree | Source in this repository |
|---|---|
| `src/config/configs/CUST/SAOLAH743_BMI270/config.h` | [`configs/CUST/SAOLAH743_BMI270/config.h`](configs/CUST/SAOLAH743_BMI270/config.h) |
| `src/config/configs/CUST/SAOLAH743/config.h` | [`configs/CUST/SAOLAH743/config.h`](configs/CUST/SAOLAH743/config.h) |

Each file carries a GPLv3 header identifying it as a modified work, as
required by section 5(a).

### Changes to original Betaflight files

Apply [`patches/upstream-changes.patch`](patches/upstream-changes.patch)
(2026-09-15). It touches 5 files:

| File | Change |
|---|---|
| `src/main/drivers/accgyro/accgyro_spi_bmi270.c` | New `BMI270_POLLING_STRICT` option for boards without INT1/INT2 wired: interrupts and FIFO mode are disabled, and gyro/accel reads check the STATUS data-ready bits over SPI. The config-blob upload now checks `INTERNAL_STATUS` and retries up to 3 times after a soft reset. Previously a failed upload went unnoticed and left the gyro outputting noise. |
| `src/main/drivers/accgyro/accgyro_spi_bmi270.h` | Declares `bmi270ConfigUploadOk()`, `bmi270ConfigUploadAttempts()`, `bmi270ReadRegisterExt()` |
| `src/main/sensors/gyro_init.c` | `gyroReadRegisterBmi270()`: register read with the BMI270's leading dummy byte |
| `src/main/sensors/gyro_init.h` | Declares `gyroReadRegisterBmi270()` |
| `src/main/cli/cli.c` | `gyroregisters` prints BMI270 registers (CHIP_ID, ERR_REG, INTERNAL_STATUS, GYR_CONF, GYR_RANGE, PWR_CONF, PWR_CTRL) and the upload result instead of meaningless MPU registers |

## Rebuilding

```bash
git clone https://github.com/betaflight/betaflight.git
cd betaflight
git checkout 80b0bceb2f1231896c763cb04919f44007fe2e11
git submodule update --init src/config
git -C src/config checkout 036eaa86f69cd24d34c05dcdeb7a005e59c7ca29

PYXIS=/path/to/pyxis
cp -r "$PYXIS/BetaFlight/configs/CUST/SAOLAH743_BMI270" src/config/configs/CUST/
git apply "$PYXIS/BetaFlight/patches/upstream-changes.patch"

# Fix the embedded build date to that of the release (2026-09-15 03:09:27 UTC)
export SOURCE_DATE_EPOCH=1789441767
make CONFIG=SAOLAH743_BMI270
make CONFIG=SAOLAH743_BMI270 obj/betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
```

With `SOURCE_DATE_EPOCH` set, the `.hex` and `.dfu` in `obj/` are
**byte-identical** to the released files. This was verified with
`arm-none-eabi-gcc` 13.2.1 (`13.2.rel1`). Without it, only the embedded
build date and time differ.

Build instructions in detail: [build-guided.md](build-guided.md).

## Released artifacts

Release [`BetaFlight-v0.0.2`](../../../releases/tag/BetaFlight-v0.0.2),
archive `bmi270.zip`, built 2026-09-15.

```
MD5                               Bytes     File
708adc45278747c8b02ed081080c7871   993836   bmi270.zip
bd94d9c090c44d43284c4f5ee073b50e   596194   betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
f78f00c6cbeb17eef0406f257e88b593  1676186  betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.hex
```

Verify a download with `md5sum <file>` and compare against the table.

The `SAOLAH743` (BMI088 + BMI270) variant has not been released.

## License

Betaflight is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE).
Our configs and driver changes are derivative works and carry the same license.

See [`../COMPLIANCE.md`](../COMPLIANCE.md) for the source offer and trademark
usage.
