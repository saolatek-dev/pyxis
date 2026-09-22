# SOURCE — PX4 SAOLAH743

This file states what the released firmware was built from, so that anyone
can reproduce the exact binaries. **Include this file in every release.**

## Upstream

```
Project: PX4-Autopilot
URL:     https://github.com/PX4/PX4-Autopilot
Commit:  efd05431e8426d5a788ad814946bfba2f5da893e
Date:    2026-04-26
```

You must use **exactly this commit**. Writing "PX4 v1.17" is not enough — it
does not identify a unique source tree.

## Saolatek changes

### Added

| Path in the PX4 tree | Source in this repository |
|---|---|
| `boards/saolah743/h743/` | `PX4/boards/saolah743/h743/` |
| `src/drivers/barometer/dps368/` | `PX4/src/drivers/barometer/dps368/` |

### Changes to original PX4 files

Apply `PX4/patches/upstream-changes.patch`, which touches 3 files:

| File | Change |
|---|---|
| `src/drivers/barometer/CMakeLists.txt` | `+add_subdirectory(dps368)` |
| `src/drivers/drv_sensor.h` | `+#define DRV_BARO_DEVTYPE_DPS368 0x64` |
| `src/drivers/barometer/dps310/DPS310.cpp` | Retry the Product ID read up to 5 times (a single failed I2C read leaves `buf = 0`, which looks exactly like "Product_ID mismatch" at boot); count `comm errors` when `read()` fails instead of swallowing it silently |

**Without the first 2 lines, the DPS368 driver is not compiled into the
firmware** — the build still succeeds but the DPS368 barometer stays silent,
which is very hard to trace.

## Rebuilding

```bash
git clone https://github.com/PX4/PX4-Autopilot.git
cd PX4-Autopilot
git checkout efd05431e8426d5a788ad814946bfba2f5da893e
git submodule update --init --recursive

PYXIS=/path/to/pyxis
cp -r "$PYXIS/PX4/boards/saolah743"            boards/
cp -r "$PYXIS/PX4/src/drivers/barometer/dps368" src/drivers/barometer/
git apply "$PYXIS/PX4/patches/upstream-changes.patch"

make saolah743_h743            # DPS310 barometer
make saolah743_h743_dps368     # DPS368 barometer
make saolah743_h743_bootloader # bootloader
```

Combine the factory image (bootloader + firmware in one file):

```bash
python3 - <<'EOF'
bl  = open('boards/saolah743/h743/extras/saolah743_h743_bootloader.bin','rb').read()
app = open('build/saolah743_h743_default/saolah743_h743_default.bin','rb').read()
open('factory.bin','wb').write(bl + b'\xff'*(0x20000-len(bl)) + app)
EOF
arm-none-eabi-objcopy -I binary -O ihex --change-addresses 0x08000000 \
  factory.bin factory.hex
```

## Checksums of the release files

```
MD5                               Bytes     File
1b5f8f91c93c0385853bff630f154043  1767384   saolah743_h743_default_factory.bin
8c2ca672a87203f9e01133b9958949e0  4971267   saolah743_h743_default_factory.hex
0e0d5fc2ffe9cf551c11d6d27a050098  1767384   saolah743_h743_dps368_factory.bin
735afe4f3b01eb1eda3152813a36b80a  4971267   saolah743_h743_dps368_factory.hex
72ca2c0ecd3bc8b1628b517b1312f9f3    40944   saolah743_h743_bootloader.bin
8c0a8c71678908a38b426ea846f70646  1548314   saolah743_h743_default.px4
264b0341973f4238482e453ac10ca988  1548318   saolah743_h743_dps368.px4
```

After downloading, check with `md5sum -c`, or run `md5sum <file>` and compare.

## License

PX4-Autopilot: **BSD 3-Clause**. Derived files keep the PX4 Development Team
copyright lines. See `PX4/LICENSE`.
