# SOURCE — ArduPilot for Pyxis (SaolaH743)

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

| Path in the ArduPilot tree | Source in this repository |
| --- | --- |
| `libraries/AP_HAL_ChibiOS/hwdef/Saolah743/hwdef.dat` | [`hwdef.dat`](hwdef.dat) |

The board is registered by adding an `AP_HW_Saolah743` entry to
`Tools/AP_Bootloader/board_types.txt`.

## Rebuilding

```bash
git clone https://github.com/ArduPilot/ardupilot.git
cd ardupilot
git checkout 1511f27194f1dcc3728270883047bdf022b3fd53
git submodule update --init --recursive

PYXIS=/path/to/pyxis
mkdir -p libraries/AP_HAL_ChibiOS/hwdef/Saolah743
cp "$PYXIS/Ardupilot/hwdef.dat" libraries/AP_HAL_ChibiOS/hwdef/Saolah743/

./waf configure --board Saolah743
./waf copter
./waf bootloader
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

## License

ArduPilot is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE).
Our board support files are derivative works and carry the same license.

Corresponding source is offered from this repository under GPLv3 section
6(d), and by written offer under section 6(b) for boards supplied with
firmware pre-installed — see [`../COMPLIANCE.md`](../COMPLIANCE.md).
