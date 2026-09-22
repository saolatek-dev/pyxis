# Open source license compliance

This repository aggregates board support for **four independent flight
control firmware projects**. Each is a separate program: built separately,
flashed separately, never linked into a common binary.

Licensing therefore applies **per directory**. There is no repository-wide
license, and none should be added.

| Directory | Upstream project | License | License file |
| --- | --- | --- | --- |
| `Ardupilot/` | [ArduPilot](https://github.com/ArduPilot/ardupilot) | GPL-3.0-or-later | [`Ardupilot/LICENSE`](Ardupilot/LICENSE) |
| `BetaFlight/` | [Betaflight](https://github.com/betaflight/betaflight) | GPL-3.0-or-later | [`BetaFlight/LICENSE`](BetaFlight/LICENSE) |
| `Inav/` | [INAV](https://github.com/iNavFlight/inav) | GPL-3.0-or-later | [`Inav/LICENSE`](Inav/LICENSE) |
| `PX4/` | [PX4-Autopilot](https://github.com/PX4/PX4-Autopilot) | BSD-3-Clause | [`PX4/LICENSE`](PX4/LICENSE) |

Placing GPL-licensed and BSD-licensed programs in one repository is
permitted under the *mere aggregation* provision of GPLv3 (section 5).

> **Do not copy code between directories.** GPL-licensed code moved into
> `PX4/` would place the entire PX4 port under GPLv3, irreversibly. Sharing
> **hardware facts** — GPIO assignments, resistor values, oscillator
> frequencies, measurements — is not restricted; facts are not copyrightable.

## What we must provide, and where

Our board support files are derivative works of the upstream projects. The
upstream licenses grant the right to distribute them, subject to conditions.

### GPLv3 directories

| Obligation | Satisfied by |
| --- | --- |
| §4 — supply a copy of the license | `LICENSE` in each directory |
| §5(a) — state that the work is modified, with a date | Copyright headers in modified files; `SOURCE.md` |
| §5(b)(c) — license the whole work under GPLv3 | Inherited; no conflicting terms are imposed |
| §6 — supply Corresponding Source for released binaries | `SOURCE.md` in each directory, shipped with each release |
| §10 — impose no further restrictions | No EULA, NDA, resale restriction or device lock is applied |

### `PX4/` (BSD-3-Clause)

Retain the copyright notice, the list of conditions and the disclaimer in
both source and binary distributions. Do not use the names of the copyright
holders to endorse derived products. Derived files keep the original PX4
Development Team copyright lines.

## Corresponding Source

"Corresponding Source" means the source that builds **the exact binary that
was released** — not merely a nearby version. A release note saying
"ArduPilot 4.7.0" is not sufficient: tags can be moved, and a version number
does not identify a unique tree.

Each firmware directory therefore carries a `SOURCE.md` recording:

- the upstream repository and the **full commit hash** the binary was built from
- every change made by Saolatek, as files or as a patch
- the exact build commands
- checksums of every released artifact

This satisfies GPLv3 §6(d): source is offered from a network location with
clear directions. We undertake to keep that source available for as long as
the corresponding binaries are distributed.

## Written offer for physical products

Where a board is supplied with firmware pre-installed, GPLv3 §6(a)/(b)
applies rather than §6(d). Saolatek accordingly makes this offer:

> For any GPL-licensed firmware distributed by Saolatek, whether pre-installed
> on hardware or downloaded from this repository, Saolatek will supply the
> complete corresponding machine-readable source code for a period of **three
> years** from the date of distribution, for no more than the cost of
> physically performing the distribution. Requests: **contact@saolatek.vn**.

This offer must be reproduced in product documentation shipped with any board
that has GPL-licensed firmware pre-installed.

## Device lock-down

GPLv3 treats a consumer flight controller as a *User Product*. If object code
is conveyed in a User Product and installation of modified versions is
restricted, the vendor must also supply **Installation Information** —
whatever is needed for the purchaser to install their own modified build,
including signing keys.

Pyxis boards ship **unlocked**: firmware is installable over USB DFU and SWD,
flash readout protection is not set, and firmware signatures are not enforced.
No Installation Information is therefore required.

Any future proposal to lock the bootloader, enable RDP or require signed
firmware must be reviewed against this section first.

## Trademarks

The upstream licenses grant rights in **copyright only**. They convey no
rights in names or logos. "ArduPilot", "Betaflight", "INAV" and "PX4" are
the marks of their respective projects, and BSD-3-Clause section 3 expressly
withholds endorsement rights.

Describe compatibility; do not imply sponsorship or affiliation.

- Acceptable: "Pyxis FC — runs ArduPilot, Betaflight, INAV and PX4"
- Acceptable: "Firmware based on ArduPilot"
- Not acceptable: naming the product after an upstream project
- Not acceptable: using upstream logos as certification marks on packaging or marketing

## Contributing changes upstream

Submitting board targets to the upstream projects is encouraged. Once a target
is merged, the corresponding source is public in the upstream repository, which
substantially reduces the burden of section 6 and removes the need to host and
maintain the source ourselves.

---

Questions about licensing of this repository: **contact@saolatek.vn**
