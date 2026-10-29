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

Release [`Ardupilot-v0.0.4`](../../../releases/tag/Ardupilot-v0.0.4),
archive `Saolah743-4.7.0-stable.zip`, built 2026-10-27 from this repository
at commit `31973c7` with the board files and patch listed above.

```
MD5                               Bytes     File
fc450c22a8644603c1bd966a3ad35d76  4871900   Saolah743-4.7.0-stable.zip
b87ad62bd1a6406370a4b384b527a7eb    18173   AP_Bootloader.apj
989ad5008760ded3dce8a6efbab04aed    19556   AP_Bootloader.bin
b6f20f687847130f6980341c0c591de4    53816   AP_Bootloader.hex
8f930e868e9b5645bb00729a1b4af351  1365545   arducopter.apj
b191717d98466cbdf53cd09507a50702  1529132   arducopter.bin
3426b90645f25898f23a9fbb9cf5d989  1660204   arducopter_with_bl.bin
a1c517aa6e031f88db2c876abab0909b  4565992   arducopter_with_bl.hex
```

Verify a download with `md5sum <file>` and compare against the table.
`arducopter_with_bl.bin` is not produced by waf: it is
`arducopter_with_bl.hex` converted to a flat binary from `0x08000000`,
padding gaps with `0xFF`.

## License

ArduPilot is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE).
Our board support files are derivative works and carry the same license.

Corresponding source is offered from this repository under GPLv3 section
6(d), and by written offer under section 6(b) for boards supplied with
firmware pre-installed — see [`../COMPLIANCE.md`](../COMPLIANCE.md).
