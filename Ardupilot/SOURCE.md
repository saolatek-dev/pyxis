# SOURCE — ArduPilot for Pyxis (Saolah743)

This file records how the released binaries were built, so that anyone can
reproduce them. It satisfies GPLv3 section 6. **Ship it with every release.**

## Upstream

```
Project: ArduPilot
URL:     https://github.com/ArduPilot/ardupilot
Commit:  1511f27194f1dcc3728270883047bdf022b3fd53
Subject: Plane: version to 4.7.0
Date:    2026-07-14
```

The commit hash is authoritative. "ArduPilot 4.7.0" alone is not sufficient —
a tag can be moved and does not identify a unique tree.

This hash is embedded in the released firmware and can be verified directly:

```bash
strings arducopter.bin | grep "ArduCopter V"
# ArduCopter V4.7.0 (1511f271)
```

Submodules must be checked out at the revisions recorded by that commit:

```bash
git checkout 1511f27194f1dcc3728270883047bdf022b3fd53
git submodule update --init --recursive
```

## Saolatek changes

### Added

All files go in `libraries/AP_HAL_ChibiOS/hwdef/Saolah743/`:

| File in the ArduPilot tree | Source in this repository |
| --- | --- |
| `hwdef.dat` | [`Saolah743/hwdef.dat`](Saolah743/hwdef.dat) |
| `hwdef-bl.dat` | [`Saolah743/hwdef-bl.dat`](Saolah743/hwdef-bl.dat) |
| `defaults.parm` | [`Saolah743/defaults.parm`](Saolah743/defaults.parm) |
| `README.md` | [`Saolah743/README.md`](Saolah743/README.md) |

### Changes to original ArduPilot files

Apply [`patches/upstream-changes.patch`](patches/upstream-changes.patch)
(2026-09-25). It touches 2 files:

| File | Change |
| --- | --- |
| `Tools/AP_Bootloader/board_types.txt` | `+AP_HW_Saolah743 6130`. `hwdef.dat` and `hwdef-bl.dat` refer to this name, so `waf configure` fails without it. |
| `Tools/AP_Bootloader/bl_protocol.cpp` | Before jumping to the application, the bootloader now switches SYSCLK back to HSI and stops HSE and PLL1–3 (STM32H7 only), stops SysTick, clears every enabled and pending NVIC interrupt, and clears the CONTROL register. ChibiOS firmware does not need this; NuttX firmware (PX4) hangs or crashes without it when started by the ArduPilot bootloader. |

## Rebuilding

```bash
git clone https://github.com/ArduPilot/ardupilot.git
cd ardupilot
git checkout 1511f27194f1dcc3728270883047bdf022b3fd53
git submodule update --init --recursive

PYXIS=/path/to/pyxis
cp -r "$PYXIS/Ardupilot/Saolah743" libraries/AP_HAL_ChibiOS/hwdef/
git apply "$PYXIS/Ardupilot/patches/upstream-changes.patch"

Tools/scripts/build_bootloaders.py Saolah743   # -> Tools/bootloaders/Saolah743_bl.bin
./waf configure --board Saolah743
./waf copter                                    # -> build/Saolah743/bin/
```

Build instructions in detail: [build-guided.md](build-guided.md).
Flashing: [boot-firmware-guided.md](boot-firmware-guided.md).

## Released artifacts

Release [`Ardupilot-v0.0.3`](../../../releases/tag/Ardupilot-v0.0.3),
archive `Saolah743-4.7.0-stable.zip`, built 2026-09-08.

```
MD5                               Bytes     File
431747b2372b66e765486704651b99f2  4642656   Saolah743-4.7.0-stable.zip
1071f5cafabb298f590b17b4d08562c2    18909   AP_Bootloader.apj
2f17e64f24021e733b737eecbcb02651    20300   AP_Bootloader.bin
49c378e717a0c561bc4c7bcdbb539d17    55856   AP_Bootloader.hex
f89fc891f60ab70e36c403c68b62598f  1365937   arducopter.apj
0734a8ba00d7c0bd7010ac9e51a317b0  1529612   arducopter.bin
eb95067696f5c72efae5564c55870740  1660684   arducopter_with_bl.bin
a263fd07f91a88d202df176a7673b6de  4567312   arducopter_with_bl.hex
```

Verify a download with `md5sum <file>` and compare against the table.

> [!WARNING]
> This release was built on 2026-09-08, before the board files above were
> updated on 2026-09-25. Rebuilding from the current files does **not**
> reproduce these checksums. The next release must be built from the current
> files and recorded here.

## License

ArduPilot is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE).
Our board support files are derivative works and carry the same license.

Corresponding source is offered from this repository under GPLv3 section
6(d), and by written offer under section 6(b) for boards supplied with
firmware pre-installed — see [`../COMPLIANCE.md`](../COMPLIANCE.md).
