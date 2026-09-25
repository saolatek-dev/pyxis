# Flashing Betaflight to the SAOLAH743 board

> [!NOTE]
> First time flashing on this computer? Install the tools and USB drivers in
> [flashing-setup.md](../Docs/flashing-setup.md) first (Ubuntu, Windows and WSL2).

## Step 1 — Get the firmware file

**Prebuilt (recommended):** open **[Releases](../../../releases)**, pick the
newest tag starting with `BetaFlight-`, download the zip (for example
`bmi270.zip`) and extract it. It contains:

| File | Tool |
|---|---|
| `betaflight_<version>_STM32H743_SAOLAH743_BMI270.hex` | Betaflight Configurator, STM32CubeProgrammer |
| `betaflight_<version>_STM32H743_SAOLAH743_BMI270.dfu` | dfu-util |

Optional: check the download with `md5sum <file>` and compare with the table in
[SOURCE.md](SOURCE.md#released-artifacts).

> [!IMPORTANT]
> The release is the **`SAOLAH743_BMI270`** variant, for boards with only a
> BMI270. For a board that also carries a BMI088, build `SAOLAH743` yourself
> ([build-guided.md](build-guided.md)). That variant holds the BMI088 off the
> SPI bus so it cannot corrupt the BMI270 readings.

**Built yourself:** the files are in the `obj/` folder of the Betaflight tree.

## Step 2 — Enter DFU mode

1. Unplug the USB cable.
2. Press and hold the **BOOT** button.
3. Plug the USB cable in while holding the button, then **release it**.

A board that is already running Betaflight can also be put into DFU mode by
typing `bl` in the CLI.

Check that the board is visible (Linux):

```bash
sudo dfu-util -l    # must list a device with ID 0483:df11 ("STM32 BOOTLOADER")
```

## Step 3 — Flash

**Method 1 — Betaflight Configurator (Windows / Linux / macOS):**

1. Open the **Firmware Flasher** tab. The port selector shows **DFU**.
2. Click **Load Firmware [Local]** and select the `.hex` file.
3. Optional: enable **Full chip erase** when coming from another firmware (ArduPilot, INAV, PX4).
4. Click **Flash Firmware** and wait for *Programming: SUCCESSFUL*.

**Method 2 — dfu-util (Linux / WSL):** use the **`.dfu`** file. dfu-util cannot read `.hex` files.

```bash
sudo dfu-util -a 0 -s :leave -D betaflight_<version>_STM32H743_SAOLAH743_BMI270.dfu
```

Wait for `File downloaded successfully`. `-s :leave` restarts the board into
the new firmware. If it does not restart, unplug and replug USB.

`Cannot open DFU device` means missing USB permissions. Use `sudo`, or install
the udev rule described in [flashing-setup.md](../Docs/flashing-setup.md).

## Step 4 — First connection

After flashing, the board shows up as a serial port (`/dev/ttyACM*` on Linux, a
COM port on Windows). Connect with Betaflight Configurator and run in the CLI:

```
status
```

Check that the board name matches the variant you flashed.

To use motors 9 and 10:

```
resource motor 9 D14
resource motor 10 D15
save
```

Then run the checks in [build-guided.md](build-guided.md#step-6--verify).

**If the board does not show up:** unplug USB, make sure BOOT is **not** pressed,
and plug it back in. If `dfu-util -l` still lists `0483:df11`, the board is still
in DFU mode.
