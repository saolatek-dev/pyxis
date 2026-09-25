# Building PX4 firmware for the SAOLAH743

If you don't want to build it yourself, download a prebuilt release from **[Releases](../../../releases)** (tag `PX4-*`) and see [flash-guided.md](flash-guided.md).

## 1. Install the environment

```bash
sudo apt update
sudo apt install git python3-pip cmake ninja-build \
                 gcc-arm-none-eabi binutils-arm-none-eabi
```

## 2. Get the right version of the PX4 source

```bash
git clone https://github.com/PX4/PX4-Autopilot.git
cd PX4-Autopilot
git checkout efd05431e8426d5a788ad814946bfba2f5da893e
git submodule update --init --recursive
```

It must be this exact commit. Other versions may not build with the current port.

## 3. Apply the SAOLAH743 port

Assuming the `pyxis` repository is at `~/pyxis`:

```bash
PYXIS=~/pyxis

cp -r "$PYXIS/PX4/boards/saolah743" boards/
git apply "$PYXIS/PX4/patches/upstream-changes.patch"
```

**Apply the patch.** It makes the DPS310 driver retry the Product ID read at
boot and count I2C read errors. Without it the build still succeeds, but a
single failed I2C read at boot is reported as `Product_ID mismatch` and the
barometer does not start.

## 4. Build

```bash
make saolah743_h743
```

One firmware covers every board: the DPS310 driver handles both the DPS310
and the DPS368 barometer (same register map and Product ID), and the IMU
(BMI088 or BMI270) is detected at boot.

Output:

```
build/saolah743_h743_default/saolah743_h743_default.px4
```

The `.px4` file is flashed through QGroundControl, provided the board
**already has** the PX4 bootloader.

## 5. Build the bootloader (only when needed)

```bash
make saolah743_h743_bootloader
```

The result overwrites `boards/saolah743/h743/extras/saolah743_h743_bootloader.bin`
(CMake does this automatically).

## 6. Combine a factory image (optional)

For new/blank boards — one file containing both the bootloader and the firmware:

```bash
python3 - <<'EOF'
bl  = open('boards/saolah743/h743/extras/saolah743_h743_bootloader.bin','rb').read()
app = open('build/saolah743_h743_default/saolah743_h743_default.bin','rb').read()
assert len(bl) <= 0x20000
open('saolah743_h743_default_factory.bin','wb').write(bl + b'\xff'*(0x20000-len(bl)) + app)
EOF

arm-none-eabi-objcopy -I binary -O ihex --change-addresses 0x08000000 \
  saolah743_h743_default_factory.bin saolah743_h743_default_factory.hex
```

These are the same kind of `*_factory.bin` / `*_factory.hex` files published in
the releases; flash them as described in [flash-guided.md](flash-guided.md).

The `.hex` file carries its own addresses, so STM32CubeProgrammer does not need a Start Address.

## Troubleshooting

**Strange CMake/Kconfig errors** (for example `redefinition of 'get_latency'`,
or a missing `Kconfig` file under `platforms/nuttx/NuttX/apps/...`): usually a
corrupted CMake cache.

```bash
rm -rf build/saolah743_h743_<config>
make saolah743_h743_<config>
```

**The barometer is not detected, and the boot log shows `Product_ID mismatch`**:
check that the `git apply` step in section 3 was done.

**Flash nearly full warning**: normal. The firmware currently uses ~89% of 1792 KiB.
