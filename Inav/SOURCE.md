# SOURCE — INAV for Pyxis (SAOLA_H743)

This file records how INAV firmware for Pyxis is built, so that anyone can
reproduce it. It satisfies GPLv3 section 6. **Ship it with every release.**

## Upstream

```
Project: INAV
URL:     https://github.com/iNavFlight/inav
Commit:  4939a7ff7cd263b60718080b3655bfae7b589c93
Version: 9.1.0-RC1-20-g4939a7ff7
Date:    2026-06-18
```

The commit hash is authoritative. A version number alone does not identify a
unique tree.

```bash
git checkout 4939a7ff7cd263b60718080b3655bfae7b589c93
```

## Saolatek changes

### Added

| Path in the INAV tree | Source in this repository |
| --- | --- |
| `src/main/target/SAOLA_H743/target.c` | [`SAOLA_H743/target.c`](SAOLA_H743/target.c) |
| `src/main/target/SAOLA_H743/target.h` | [`SAOLA_H743/target.h`](SAOLA_H743/target.h) |
| `src/main/target/SAOLA_H743/CMakeLists.txt` | [`SAOLA_H743/CMakeLists.txt`](SAOLA_H743/CMakeLists.txt) |

`target.c` and `target.h` carry a GPLv3 header identifying them as a
modified work, as required by section 5(a).

### Changes to original INAV files

Apply [`patches/upstream-changes.patch`](patches/upstream-changes.patch)
(2026-09-12). It touches 3 files:

| File | Change |
| --- | --- |
| `src/main/drivers/max7456.c` | `max7456WaitUntilNoBusy()` and the clear-display wait in `max7456RefreshAll()` now time out after `MAX_RESET_TIMEOUT_MS`. Before, a missing or unresponsive MAX7456/AT7456E made them spin forever and hung the boot (confirmed over SWD). |
| `src/main/drivers/bus_spi_hal_ll.c` | STM32H7: on a TXP/RXP timeout the SPI peripheral is disabled before returning, and the EOT wait is bounded. Before, an abandoned transfer left the peripheral enabled and the next transfer hung forever. New optional `SPI_SLOW_BUS_STANDARD_DIV16` halves `SPI_CLOCK_STANDARD` on SPI2/3/4 (off for this target). |
| `src/main/drivers/accgyro/accgyro_bmi088.c` | New optional `BMI088_GYRO_MAX_SPI_SPEED` / `BMI088_ACC_MAX_SPI_SPEED` overrides. Defaults are the upstream values (FAST / STANDARD), so behaviour is unchanged unless a target defines them (this target does not). |

## Rebuilding

```bash
git clone https://github.com/iNavFlight/inav.git
cd inav
git checkout 4939a7ff7cd263b60718080b3655bfae7b589c93

PYXIS=/path/to/pyxis
git -C "$PYXIS" checkout 3e03018        # source of the released file
cp -r "$PYXIS/Inav/SAOLA_H743" src/main/target/
git apply "$PYXIS/Inav/patches/upstream-changes.patch"

# Fix the embedded build date to that of the release (2026-09-25 03:50:18 UTC)
export SOURCE_DATE_EPOCH=1790308218
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make SAOLA_H743          # -> build/inav_9.1.0_SAOLA_H743.hex
```

With these commands `inav_9.1.0_SAOLA_H743.hex` is **byte-identical** to
the released file. This was verified on 2026-09-25 with `arm-none-eabi-gcc`
13.2.1 (`13.2.rel1`, the toolchain INAV installs into `tools/`). Without
`SOURCE_DATE_EPOCH`, only the embedded build date and time differ.

Build instructions in detail: [Build-guided.md](Build-guided.md).

## Released artifacts

Release [`Inav-v0.0.3`](../../../releases/tag/Inav-v0.0.3),
archive `Inav-SAOLA_H743.zip`, built 2026-09-25 from this repository at
commit `3e03018`. The release tag itself points at an older commit
(`04d009c`); the commit given here is the authoritative source.

```
MD5                               Bytes     File
0213f2a93560392b1771908eecbe9e8e  1804681   inav_9.1.0_SAOLA_H743.hex
```

```
SHA-256                                                           File
6b2e579b4c0a3848d940774aa988d3aa6f96723fa71292052fa6386711fe9f56  inav_9.1.0_SAOLA_H743.hex
```

The archive also contains `LICENSE`, this `SOURCE.md`, `MD5SUMS.txt` and
`SHA256SUMS.txt`. Verify a download with `md5sum <file>` or
`sha256sum <file>` and compare against the tables, or run
`sha256sum -c SHA256SUMS.txt` inside the extracted folder.

Checksum of the archive itself (listed only in the repository copy of this
file, since the copy inside the archive cannot contain its own checksum):

```
MD5                               Bytes     File
900d60a121f2ae353c15bc50cb795528   727954   Inav-SAOLA_H743.zip
SHA-256  8b15a92f578ad6110030e6b755a1cebb185a782d4ebb07608ccb04173fcc074f
```

The first upload of `Inav-SAOLA_H743.zip` on 2026-09-25 (MD5 `71b3119dda8cefa1a7ba65f93bad4ddd`,
SHA-256 `2ee2362ff2bb51c1…`) had the same firmware files but no `LICENSE`; it
was replaced by the archive above.

## License

INAV is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE). Our
target files and driver changes are derivative works and carry the same license.

See [`../COMPLIANCE.md`](../COMPLIANCE.md) for the source offer and trademark
usage.
