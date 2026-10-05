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
make arm_sdk_install                   # Arm GNU Toolchain 13.3.rel1

PYXIS=/path/to/pyxis
git -C "$PYXIS" checkout 3e03018        # source of the released files
cp -r "$PYXIS/BetaFlight/configs/CUST/SAOLAH743"        src/config/configs/CUST/
cp -r "$PYXIS/BetaFlight/configs/CUST/SAOLAH743_BMI270" src/config/configs/CUST/
git apply "$PYXIS/BetaFlight/patches/upstream-changes.patch"

# Fix the embedded build date to that of each released file (UTC)
SOURCE_DATE_EPOCH=1790309130 make CONFIG=SAOLAH743_BMI270        # 2026-09-25 04:05:30
SOURCE_DATE_EPOCH=1790309130 make CONFIG=SAOLAH743_BMI270 \
  obj/betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
SOURCE_DATE_EPOCH=1790309156 make CONFIG=SAOLAH743               # 2026-09-25 04:05:56
SOURCE_DATE_EPOCH=1790309156 make CONFIG=SAOLAH743 \
  obj/betaflight_2026.12.0-alpha_STM32H743_SAOLAH743.dfu
```

With these commands the `.hex` and `.dfu` files in `obj/` are
**byte-identical** to the released files. This was verified on 2026-09-25
with `arm-none-eabi-gcc` 13.3.1 (`13.3.rel1`, installed by
`make arm_sdk_install`). Without `SOURCE_DATE_EPOCH`, only the embedded
build date and time differ.

Build instructions in detail: [build-guided.md](build-guided.md).

## Released artifacts

Release [`BetaFlight-v0.0.2`](../../../releases/tag/BetaFlight-v0.0.2),
archive `BetaFlight-SAOLAH743.zip`, built 2026-09-25 from this repository
at commit `3e03018`. The release tag itself points at an older commit
(`04d009c`); the commit given here is the authoritative source.

```
MD5                               Bytes     File
35e2213cc816b07e6935e52b96c377fd   596194   betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
374b80ecad25f8bbeea433d2663ddf4e  1676186   betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.hex
cffc5b40e8cfbcd2322915a769c159ab   599170   betaflight_2026.12.0-alpha_STM32H743_SAOLAH743.dfu
89c3cb6f2dd9b327b848f69756da94fc  1684543   betaflight_2026.12.0-alpha_STM32H743_SAOLAH743.hex
```

```
SHA-256                                                           File
5f21f100130abe725592549f8c633c54c607ca034b382a3b8c7a64837e11fce9  betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
441fced7d5ffa22f234d090682f0b18b9c46dbd6a505290bf8bf800dfceb716f  betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.hex
3ba72254fd2b3ea58d108b9e47ba548381ed635f605e164771b82cfd4152b788  betaflight_2026.12.0-alpha_STM32H743_SAOLAH743.dfu
9214b89e590874cf6d62de3a60cbf62b081dcdfa577cd2b60e3bff180df1a2a0  betaflight_2026.12.0-alpha_STM32H743_SAOLAH743.hex
```

> [!WARNING]
> These files were built before `ADC3_DMA_OPT` was added to both configs
> (2026-10-05). Without it ADC3, which carries VREFINT and the core
> temperature sensor on the H743, is never started. Battery voltage and CPU
> temperature then read wrong: about 30 V and 217 °C on USB power alone.
> Workaround on a board already flashed with this release, in the CLI:
>
> ```
> dma adc 3 9
> save
> ```
>
> Rebuilding from the current configs does **not** reproduce these
> checksums. The next release must be built from the current files and
> recorded here.

The archive also contains `LICENSE`, this `SOURCE.md`, `MD5SUMS.txt` and
`SHA256SUMS.txt`. Verify a download with `md5sum <file>` or
`sha256sum <file>` and compare against the tables, or run
`sha256sum -c SHA256SUMS.txt` inside the extracted folder.

Checksum of the archive itself (listed only in the repository copy of this
file, since the copy inside the archive cannot contain its own checksum):

```
MD5                               Bytes     File
2ae6b5fba617df67047be29c6285dbcc  2150292   BetaFlight-SAOLAH743.zip
SHA-256  12506a44661089c83aa45ec22b52edee4e89bd93fe220031699a0072c7bcc19f
```

The first upload of `BetaFlight-SAOLAH743.zip` on 2026-09-25 (MD5 `5006a47256bfa62ec3ddb266208a6935`,
SHA-256 `5e3ef11e95dcb902…`) had the same firmware files but no `LICENSE`; it
was replaced by the archive above.

### Previously published assets

Until 2026-09-25 this release carried `bmi270.zip`
(MD5 `708adc45278747c8b02ed081080c7871`), built 2026-09-15 from the configs
at commit `83ee3b7` with `SOURCE_DATE_EPOCH=1789441767`. That firmware has
the OSD chip select on `PB12` instead of `PD11` (the OSD does not work) and
a voltage scale of 213 instead of 110 (battery voltage reads about twice
the real value). It has been replaced by the files above.

## License

Betaflight is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE).
Our configs and driver changes are derivative works and carry the same license.

See [`../COMPLIANCE.md`](../COMPLIANCE.md) for the source offer and trademark
usage.
