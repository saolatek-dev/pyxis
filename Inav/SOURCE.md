# SOURCE — INAV for Pyxis (SaolaH743)

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
git submodule update --init --recursive
```

## Saolatek changes

| Path in the INAV tree | Source in this repository |
| --- | --- |
| `src/main/target/SaolaH743/target.c` | [`SaolaH743/target.c`](SaolaH743/target.c) |
| `src/main/target/SaolaH743/target.h` | [`SaolaH743/target.h`](SaolaH743/target.h) |
| `src/main/target/SaolaH743/CMakeLists.txt` | [`SaolaH743/CMakeLists.txt`](SaolaH743/CMakeLists.txt) |

Each of these files carries a GPLv3 header identifying it as a modified work,
as required by section 5(a).

> [!NOTE]
> Bringing the board up also required changes to shared INAV driver code
> (`accgyro_bmi088.c`, `bus_spi_hal_ll.c`, `max7456.c`). Those changes are not
> yet mirrored into this repository. They must be added here, listed
> individually and dated, before any INAV binary for this board is released.

## Rebuilding

```bash
git clone https://github.com/iNavFlight/inav.git
cd inav
git checkout 4939a7ff7cd263b60718080b3655bfae7b589c93
git submodule update --init --recursive

PYXIS=/path/to/pyxis
cp -r "$PYXIS/Inav/SaolaH743" src/main/target/

mkdir -p build && cd build
cmake ..
make SaolaH743
```

Build instructions in detail: [Build-guided.md](Build-guided.md).

## Released artifacts

Release [`Inav-v0.0.3`](../../../releases/tag/Inav-v0.0.3) currently carries
no binary assets. When firmware is published, record every artifact here with
its MD5 and size, in the form used by the other firmware directories.

## License

INAV is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE). Our
target files are derivative works and carry the same license.

See [`../COMPLIANCE.md`](../COMPLIANCE.md) for the source offer and trademark
usage.
