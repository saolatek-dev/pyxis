# SOURCE — Betaflight for Pyxis (SAOLAH743)

This file records how the released binaries were built, so that anyone can
reproduce them. It satisfies GPLv3 section 6. **Ship it with every release.**

## Upstream

```
Project: Betaflight
URL:     https://github.com/betaflight/betaflight
Version: 2026.12.0-alpha
Target:  SAOLAH743_BMI270
Build:   036eaa8
```

The build identifier is embedded in the released firmware:

```bash
strings betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.hex | grep -E "2026\.12|036eaa8"
```

> [!IMPORTANT]
> `036eaa8` does not resolve to a commit in the upstream Betaflight
> repository, so it identifies a Saolatek working tree rather than a public
> upstream revision. Until that tree is published, this file does **not** yet
> constitute complete Corresponding Source under GPLv3 section 6.
>
> Resolving this requires either publishing the tree that produced the
> release, or rebuilding from a published upstream commit and reissuing the
> release. Until then, requests for source should be directed to
> **contact@saolatek.vn**, which is honoured under the written offer in
> [`../COMPLIANCE.md`](../COMPLIANCE.md).

## Saolatek changes

The Pyxis target is not in upstream Betaflight. Porting it requires a target
definition plus changes to shared driver code; the procedure is documented in
[`guide.md`](guide.md).

Changes to upstream files must be listed here individually once the source
tree is published, because GPLv3 section 5(a) requires modified files to be
identified and dated.

## Released artifacts

Release [`BetaFlight-v0.0.2`](../../../releases/tag/BetaFlight-v0.0.2),
archive `bmi270.zip`, built 2026-09-15.

```
MD5                               Bytes     File
708adc45278747c8b02ed081080c7871   993836   bmi270.zip
bd94d9c090c44d43284c4f5ee073b50e   596194   betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.dfu
f78f00c6cbeb17eef0406f257e88b593  1676186  betaflight_2026.12.0-alpha_STM32H743_SAOLAH743_BMI270.hex
```

Verify a download with `md5sum <file>` and compare against the table.

## License

Betaflight is licensed under **GPL-3.0-or-later**; see [`LICENSE`](LICENSE).
Our target and driver changes are derivative works and carry the same license.

See [`../COMPLIANCE.md`](../COMPLIANCE.md) for the source offer and trademark
usage.
