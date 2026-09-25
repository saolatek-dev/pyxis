# Flashing ArduPilot firmware to the Pyxis FC

> [!NOTE]
> First time flashing on this computer? Install the tools and USB drivers in
> [flashing-setup.md](../Docs/flashing-setup.md) first (Ubuntu, Windows and WSL2).

## 1. Download the firmware

1. Open **[Releases](../../../releases)** and pick the newest tag starting with `Ardupilot-`.
2. Download the zip archive (for example `Saolah743-4.7.0-stable.zip`) and extract it.
3. Optional: check the download with `md5sum <file>` and compare with the table in [SOURCE.md](SOURCE.md).

The archive contains:

| File | When to use it |
| --- | --- |
| `arducopter_with_bl.bin` | **New board, or board running Betaflight / INAV / PX4.** Bootloader and firmware in one file, flash once with `dfu-util`. |
| `arducopter_with_bl.hex` | Same as above, for STM32CubeProgrammer. |
| `arducopter.apj` | Board **already runs ArduPilot**. Update through Mission Planner or QGroundControl. |
| `AP_Bootloader.bin`, `arducopter.bin` | Bootloader and firmware separately. Only needed for the manual method in section 4. |

> [!NOTE]
> This build is written for the **DPS310** barometer. Boards fitted with a DPS368 are not supported by this firmware.

## 2. Enter DFU mode

1. Unplug the USB cable.
2. Press and hold the **BOOT** button.
3. Plug the USB cable in while holding the button, then **release it**.

Check that the board is visible:

```bash
sudo dfu-util -l    # must list a device with ID 0483:df11 ("STM32 BOOTLOADER")
```

If nothing is listed, repeat the steps above with another USB cable. Many Type-C cables only carry power.

## 3. Flash (new board)

Open a terminal in the folder where you extracted the zip:

```bash
sudo dfu-util -a 0 -s 0x08000000:leave -D arducopter_with_bl.bin
```

Wait for `File downloaded successfully`. The board restarts by itself. If it does not, unplug and replug USB.

**Using STM32CubeProgrammer instead:** connect over **USB** (DFU), open `arducopter_with_bl.hex`, and click **Download**. No start address is needed because the HEX file contains its own addresses.

## 4. Flash bootloader and firmware separately (manual method)

Use this method only if you need to replace the bootloader or firmware on its own. Enter DFU mode (section 2), then run:

```bash
sudo dfu-util -a 0 -s 0x08000000 -D AP_Bootloader.bin
sudo dfu-util -a 0 -s 0x08020000:leave -D arducopter.bin
```

The firmware must go to `0x08020000`, because the first 128 KB are reserved for the bootloader.

If you built the firmware yourself, the files are in `build/Saolah743/bin/` inside the ArduPilot tree.

## 5. Check the result

About 3 seconds after reset, the board shows up as a serial port:

```bash
ls /dev/ttyACM*
```

Connect with **Mission Planner** or **QGroundControl**.

## 6. Updating later

Once the ArduPilot bootloader is on the board, DFU is no longer needed:

- **Mission Planner:** Install Firmware → **Load custom firmware** → select `arducopter.apj`.
- **QGroundControl:** Vehicle Setup → Firmware → ArduPilot → Advanced settings → **Custom firmware file...** → select `arducopter.apj`.

## Troubleshooting

**`dfu-util: No DFU capable USB device available`**: the board is not in DFU mode, or the cable carries power only. Repeat section 2.

**`LIBUSB_ERROR_ACCESS`**: run the command with `sudo`.

**Flashing succeeded but the board does not show up**: unplug USB, make sure BOOT is **not** pressed, then plug it back in. If `dfu-util -l` still lists `0483:df11`, the board is still in DFU mode.
