# Betaflight — Saolatek SAOLAH743

Betaflight port for the Pyxis (SAOLAH743) flight controller (STM32H743VIT6, 2 MB flash).

## Contents

| Path | Description |
|---|---|
| `build-guided.md` | How to build the firmware from source |
| `flash-guided.md` | How to flash the firmware to the board |
| `SOURCE.md` | Pins the upstream commits and lists our changes (used for releases) |
| `configs/CUST/SAOLAH743_BMI270/config.h` | Board config, **BMI270-only** boards |
| `configs/CUST/SAOLAH743/config.h` | Board config, boards with **BMI088 + BMI270** on SPI2 |
| `patches/upstream-changes.patch` | Changes to 5 original Betaflight files (BMI270 driver, CLI) |
| `LICENSE` | GPL-3.0-or-later |

Current Betaflight no longer uses `src/main/target/<board>/target.h`. A board
is a single `config.h` in the
[betaflight/config](https://github.com/betaflight/config) repository, which
the main tree carries as the `src/config` submodule.

## Two variants

| Config | Use it when | Difference |
|---|---|---|
| `SAOLAH743_BMI270` | The board has **only a BMI270** | `BMI270_POLLING_STRICT 1`: the gyro is read by polling, since INT1/INT2 are not routed to the MCU |
| `SAOLAH743` | The board has **BMI088 + BMI270** | Drives PD4/PD5 (BMI088 chip selects) HIGH through PINIO so the BMI088 stays off the SPI2 bus |

Betaflight has no BMI088 driver. On both variants the BMI270 is the only IMU in use.

## Prebuilt firmware

Download from **[Releases](../../../releases)**. Pick the newest tag with the `BetaFlight-` prefix.

| File | Tool |
|---|---|
| `betaflight_<version>_STM32H743_<variant>.hex` | Betaflight Configurator, STM32CubeProgrammer |
| `betaflight_<version>_STM32H743_<variant>.dfu` | `dfu-util` |

Both variants are released. Use `SAOLAH743_BMI270` for a BMI270-only board
and `SAOLAH743` for a board that also carries a BMI088.

## Hardware summary

| Function | Resource |
|---|---|
| IMU | BMI270 on SPI2, CS `PA15`, no interrupt line |
| Barometer | DPS368 on I2C2 (`0x76`), read by the DPS310 driver (`USE_BARO_DPS310`) |
| Compass | IST8310 / QMC5883L on I2C2 |
| OSD | AT7456E (MAX7456) on SPI1, CS `PD11` |
| Blackbox | microSD over SDMMC1, 4-bit |
| Motors | M1–M4 TIM1, M5–M6 TIM3, M7–M10 TIM4 (DShot; bidirectional on M1, M3, M5) |
| UART1 | TELEM1 (MSP) |
| UART2 | DJI O3 (MSP DisplayPort) |
| UART3 | GPS |
| UART6 | RC input (SBUS inverted inside the UART; no hardware inverter) |
| UART7 | ESC telemetry |
| UART4, UART8 | Free |
| ADC | Voltage `PC0` (`vbat_scale` 110), current `PC1` (`ibata_scale` 402) |

Betaflight only assigns M1–M8 at compile time. For M9/M10, run this in the CLI:

```
resource motor 9 D14
resource motor 10 D15
save
```

Full pin map: [`../Docs/pinout.md`](../Docs/pinout.md).

## License

Betaflight is licensed under **GPL-3.0-or-later**. The configs and the patch
are derivative works and carry the same license; see [LICENSE](LICENSE) and
[`../COMPLIANCE.md`](../COMPLIANCE.md).
