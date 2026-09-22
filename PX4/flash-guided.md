# Flashing PX4 firmware to the SAOLAH743

## Which file to use

| Board state | File | Tool |
|---|---|---|
| New / blank / running Betaflight, INAV or ArduPilot | `*_factory.hex` | CubeProgrammer, dfu-util, INAV Configurator, SWD |
| Already has the PX4 bootloader | `*.px4` | QGroundControl |
| Only the bootloader needs reflashing | `saolah743_h743_bootloader.bin` | dfu-util, SWD |

Choose the variant according to the barometer on the board: `default` = **DPS310**, `dps368` = **DPS368**.

---

## Method 1 — Factory image (new/blank board)

Bootloader and firmware combined, so a single flash gets the board running.

### STM32CubeProgrammer

1. Hold the **BOOT** button and plug in USB → the board enters ROM DFU (`0483:df11`)
2. Select `saolah743_h743_default_factory.hex`
3. Download

**No Start Address is needed** — the HEX file carries its own addresses
(the first line `:020000040800F2` sets base `0x08000000`).

### dfu-util

Use the `.bin` file, and give the address explicitly:

```bash
dfu-util -a 0 -s 0x08000000:mass-erase:force \
  -D saolah743_h743_default_factory.bin
```

Wait for `File downloaded successfully` → **release the BOOT button** → then unplug and replug USB.

> Don't add `:leave` while you are still holding BOOT. The MCU resets
> immediately, sees BOOT0 still high and returns to ROM DFU instead of running
> the firmware you just flashed — it looks exactly like a failed flash.

### INAV Configurator

Works. **Firmware Flasher** tab → **"Load firmware [local]"** (don't pick a
board from the online list) → select the `.hex` file. It verifies
byte-for-byte after writing.

It flashes as a standard DfuSe client — it reads the flash layout from the
MCU's own USB descriptor, writes to the addresses in the HEX file, and does
not check the target/board_id, so it does not reject PX4 firmware.

### SWD (ST-Link)

```bash
st-flash --reset write saolah743_h743_default_factory.bin 0x08000000
```

---

## Method 2 — QGroundControl (board already has the PX4 bootloader)

Vehicle Setup → Firmware → Advanced settings → **Custom firmware file...** →
select `saolah743_h743_default.px4`.

Unplug and replug USB when QGC says it is waiting for the device.

> QGC **cannot flash the bootloader** — it only writes the firmware through the
> bootloader already on the board. A blank board must go through Method 1 once.

---

## Checking after flashing

The board comes up after about 2 seconds:

```bash
lsusb | grep 1209      # shows: 1209:7743 Generic Saolah743
ls /dev/ttyACM*
```

---

## ⚠️ The board ID must match

QGroundControl only accepts a `.px4` file whose `board_id` **matches** the
bootloader on the board.

The firmware in this repository uses `board_id = 6130` for both the bootloader
and the firmware. If the board carries a bootloader with a different ID, QGC
reports the wrong board and refuses — in that case reflash the factory image
with Method 1 to bring them in line.

---

## Troubleshooting

**The board doesn't show up as a COM port after flashing** — check whether it
went back into ROM DFU:

```bash
dfu-util -l          # still seeing 0483:df11 means it never left DFU
```

The most common cause: BOOT0 was still held high when the MCU reset. Release
the BOOT button, then power-cycle the board.

**The board is silent while SWD is attached** — this board **does not route
NRST** to the SWD header, so attaching a probe halts the CPU and it looks dead.
Run `st-flash reset` or disconnect the probe before concluding the firmware is
broken.

**Betaflight/INAV Configurator says "Successful" but the board doesn't run** —
that only means *the bytes were written*; it does not confirm the firmware
boots. Reflash with CubeProgrammer before suspecting the firmware.
