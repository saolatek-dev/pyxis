# Flashing PX4 firmware to the SAOLAH743

> [!NOTE]
> First time flashing on this computer? Install the tools and USB drivers in
> [flashing-setup.md](../Docs/flashing-setup.md) first (Ubuntu, Windows and WSL2).

## 1. Download the firmware

1. Open **[Releases](../../../releases)** and pick the newest tag starting with `PX4-`.
2. Download the file you need from the table below. If the release only provides a zip, extract it first.
3. Optional: check the download with `md5sum <file>` and compare with the table in [SOURCE.md](SOURCE.md).

### Choose the variant: which barometer is on your board?

| Barometer | Files to use |
|---|---|
| **DPS310** | `saolah743_h743_default_*` |
| **DPS368** | `saolah743_h743_dps368_*` |

The IMU does not matter. Both variants detect BMI088 and BMI270 automatically.

### Choose the file: what is on your board now?

| Board state | File | Tool | Section |
|---|---|---|---|
| New / blank / running Betaflight, INAV or ArduPilot | `*_factory.hex` | STM32CubeProgrammer or INAV Configurator | 3 |
| | `*_factory.bin` | dfu-util or ST-Link | 3 |
| Already runs PX4 (has the PX4 bootloader) | `*.px4` | QGroundControl | 4 |
| Only the bootloader needs reflashing | `saolah743_h743_bootloader.bin` | dfu-util | 5 |

The `factory` files contain the bootloader **and** the firmware, so one flash is enough. Never flash a `.px4` file to a new board: it only works through the PX4 bootloader.

---

## 2. Enter DFU mode

Needed for sections 3 and 5.

1. Unplug the USB cable.
2. Press and hold the **BOOT** button.
3. Plug the USB cable in while holding the button, then **release it**.

Check that the board is visible:

```bash
sudo dfu-util -l    # must list a device with ID 0483:df11 ("STM32 BOOTLOADER")
```

If nothing is listed, repeat the steps above with another USB cable. Many Type-C cables only carry power.

---

## 3. Flash the factory image (new board)

Enter DFU mode first (section 2), then use **one** of the tools below. The examples use the DPS310 variant (`default`). For the DPS368 variant, replace `default` with `dps368`.

### Option A: STM32CubeProgrammer (Windows / Linux / macOS)

1. Select **USB** in the connection panel, click refresh, then **Connect**.
2. Open **Erasing & Programming**, and browse to `saolah743_h743_default_factory.hex`.
3. Click **Start Programming**.
4. When it finishes, unplug and replug USB.

No start address is needed, because the HEX file contains its own addresses.

### Option B: dfu-util (Linux / WSL)

Use the **`.bin`** file. dfu-util cannot read `.hex` files.

```bash
sudo dfu-util -a 0 -s 0x08000000:mass-erase:force:leave \
  -D saolah743_h743_default_factory.bin
```

Wait for `File downloaded successfully`. The board restarts by itself. If it does not, unplug and replug USB.

### Option C: INAV Configurator

1. Go to the **Firmware Flasher** tab.
2. Click **Load firmware [Local]** (do **not** pick a board from the online list), then select `saolah743_h743_default_factory.hex`.
3. Click **Flash Firmware**. The Configurator checks every byte after writing.
4. When it finishes, unplug and replug USB.

INAV Configurator does not check the board type of the file, so it accepts PX4 firmware.

### Option D: SWD (ST-Link)

No DFU mode needed. Connect the probe to the SWD header, then run:

```bash
st-flash --reset write saolah743_h743_default_factory.bin 0x08000000
```

Disconnect the probe afterwards (see Troubleshooting).

---

## 4. Update with QGroundControl (board already runs PX4)

1. Open QGroundControl. **Do not plug in the board yet.**
2. Vehicle Setup → **Firmware**.
3. Plug in the board over USB (no BOOT button).
4. In the dialog, tick **Advanced settings** → choose **Custom firmware file...** → **OK**.
5. Select `saolah743_h743_default.px4` (or `saolah743_h743_dps368.px4`).
6. Wait until QGC reports that the upgrade is complete. The board reboots by itself.

> QGroundControl **cannot flash the bootloader**. It only writes the firmware through the bootloader already on the board. A new board must go through section 3 once.

---

## 5. Reflash only the bootloader

Enter DFU mode (section 2), then run:

```bash
sudo dfu-util -a 0 -s 0x08000000:leave -D saolah743_h743_bootloader.bin
```

Then install the firmware through QGroundControl (section 4).

---

## 6. Check the result

The board comes up about 2 seconds after reset:

```bash
lsusb | grep 1209      # shows: 1209:7743 ... Saolah743
ls /dev/ttyACM*
```

Then open QGroundControl. It should connect automatically.

---

## ⚠️ The board ID must match

QGroundControl only accepts a `.px4` file whose `board_id` **matches** the
bootloader on the board.

The firmware in this repository uses `board_id = 6130` for both the bootloader
and the firmware. If the board carries a bootloader with a different ID, QGC
reports the wrong board and refuses. In that case, flash the factory image
(section 3) to bring them in line.

---

## Troubleshooting

**`dfu-util: No DFU capable USB device available`**: the board is not in DFU
mode, or the cable carries power only. Repeat section 2.

**`LIBUSB_ERROR_ACCESS`**: run the command with `sudo`.

**The board doesn't show up as a serial port after flashing**: check whether it
is still in DFU mode:

```bash
sudo dfu-util -l     # still seeing 0483:df11 means it never left DFU
```

The most common cause is that the BOOT button was still pressed when the board
reset. Unplug USB, make sure BOOT is not pressed, and plug it back in.

**The board is silent while SWD is attached**: this board **does not route
NRST** to the SWD header, so attaching a probe halts the CPU and it looks dead.
Run `st-flash reset` or disconnect the probe before concluding the firmware is
broken.

**Betaflight/INAV Configurator says "Successful" but the board doesn't run**:
that only means *the bytes were written*. It does not confirm that the firmware
boots. Reflash with STM32CubeProgrammer before suspecting the firmware.

**Barometer not detected**: you flashed the wrong variant. Check which part is
fitted (DPS310 or DPS368) and flash the matching file.
