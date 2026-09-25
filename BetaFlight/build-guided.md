# Building Betaflight for the SAOLAH743 board

Two build methods are given below:

- **Reproduce the release** builds the exact source that the
  `BetaFlight-*` release binaries were made from. Use it to audit or rebuild
  a release.
- **Build on a newer Betaflight** picks up upstream fixes. The patch may need
  small manual fixes if upstream has changed the BMI270 driver.

## Step 1 — Install the tools

**Ubuntu / WSL:**

```bash
sudo apt install git make python3 gcc-arm-none-eabi
```

Betaflight can also install its own pinned toolchain into `tools/`:

```bash
make arm_sdk_install
```

## Step 2 — Get the source

### Reproduce the release

```bash
git clone https://github.com/betaflight/betaflight.git
cd betaflight
git checkout 80b0bceb2f1231896c763cb04919f44007fe2e11
git submodule update --init src/config
git -C src/config checkout 036eaa86f69cd24d34c05dcdeb7a005e59c7ca29
```

For a byte-identical copy of the release binaries, use this repository at
commit `3e03018`, the Arm toolchain from `make arm_sdk_install`, and the
build dates shown in [SOURCE.md](SOURCE.md#rebuilding).

### Build on a newer Betaflight

```bash
git clone https://github.com/betaflight/betaflight.git
cd betaflight
make configs            # fetches src/config
```

## Step 3 — Add the Pyxis files

```bash
PYXIS=/path/to/pyxis

# Board configs -> src/config/configs/CUST/<board>/config.h
cp -r "$PYXIS/BetaFlight/configs/CUST/SAOLAH743"        src/config/configs/CUST/
cp -r "$PYXIS/BetaFlight/configs/CUST/SAOLAH743_BMI270" src/config/configs/CUST/

# BMI270 driver changes (polling mode, config upload check, CLI register dump)
git apply "$PYXIS/BetaFlight/patches/upstream-changes.patch"
```

> [!IMPORTANT]
> **Apply the patch before building `SAOLAH743_BMI270`.** Its config sets
> `BMI270_POLLING_STRICT`, which only the patched driver understands.
> Without the patch the build still succeeds, but the setting is silently
> ignored. The gyro is then read without a data-ready check, and a failed
> BMI270 config upload goes undetected. That firmware is not the released one.

If `git apply` fails on a newer Betaflight, run
`git apply --3way` or `git apply --reject` and resolve the conflicts by hand.
See [SOURCE.md](SOURCE.md) for what each change does.

## Step 4 — Build

Pick the variant for your board (see [README.md](README.md#two-variants)):

```bash
make -j$(nproc) CONFIG=SAOLAH743_BMI270   # BMI270-only board
make -j$(nproc) CONFIG=SAOLAH743          # BMI088 + BMI270 board
```

The output is `obj/betaflight_<version>_STM32H743_<CONFIG>.hex`.

For `dfu-util` you also need a `.dfu` file. Ask for it by name:

```bash
make CONFIG=SAOLAH743_BMI270 obj/betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
```

Change `2026.12.0-alpha` to the version printed by the previous build.

## Step 5 — Flash

See [flash-guided.md](flash-guided.md).

## Step 6 — Verify

Open Betaflight Configurator and check:

- **Setup** tab: the board model is shown as `SAOLAH743_BMI270` (or `SAOLAH743`) and the model moves when you tilt the board.
- **CLI**: `status` shows the right board name, and `gyroregisters` prints
  `INTERNAL_STATUS 0x1` and `CONFIG UPLOAD OK`. Any other value means the
  BMI270 did not load its config: the accelerometer works but the gyro only
  outputs noise.
- **Ports**: TELEM1, DJI O3, GPS and RC input send and receive.
- **Motors**: DShot output on M1–M8. For M9/M10, use the `resource` commands in [README.md](README.md#hardware-summary).
- **OSD**: the AT7456E shows the overlay.
