# Setting up a computer for flashing

Do this once, before following any of the flashing guides:
[ArduPilot](../Ardupilot/boot-firmware-guided.md) ·
[PX4](../PX4/flash-guided.md) ·
[Betaflight](../BetaFlight/flash-guided.md)

Pick the section for your computer:

- [Ubuntu / Linux](#1-ubuntu--linux)
- [Windows](#2-windows)
- [WSL2 (Linux inside Windows)](#3-wsl2)

The board uses two USB identities, and both must be reachable:

| Board state | USB ID | Seen as |
| --- | --- | --- |
| DFU mode (BOOT held while plugging in) | `0483:df11` | `STM32 BOOTLOADER` |
| Running firmware | depends on firmware (PX4: `1209:7743`) | Serial port: `/dev/ttyACM*` on Linux, `COMx` on Windows |

---

## 1. Ubuntu / Linux

### 1.1 Install the flashing tools

```bash
sudo apt update
sudo apt install dfu-util stlink-tools unzip
```

| Package | Provides | Needed for |
| --- | --- | --- |
| `dfu-util` | `dfu-util` | Flashing over USB in DFU mode (ArduPilot, PX4, Betaflight) |
| `stlink-tools` | `st-flash` | Flashing through the SWD header with an ST-Link (PX4, optional) |
| `unzip` | `unzip` | Extracting the release archives |

### 1.2 Allow USB access without `sudo`

Without this, `dfu-util` only works with `sudo`, and the Configurators and
QGroundControl **cannot see the board at all**, because they do not run as root.

**DFU mode (`0483:df11`):**

```bash
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE="0664", GROUP="plugdev", TAG+="uaccess"' \
  | sudo tee /etc/udev/rules.d/45-stm32-dfu.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

**Serial port (`/dev/ttyACM*`):**

```bash
sudo usermod -aG dialout,plugdev $USER
```

**Log out and log back in** (or reboot) for the group change to take effect.
Check with:

```bash
groups    # must list dialout and plugdev
```

Unplug and replug the board after adding the udev rule.

### 1.3 QGroundControl (PX4 and ArduPilot)

QGroundControl is shipped as an AppImage. On Ubuntu:

```bash
# ModemManager grabs /dev/ttyACM* and blocks QGC from connecting
sudo apt remove modemmanager

# Required to run AppImages (Ubuntu 24.04; on 22.04 the package is libfuse2)
sudo apt install libfuse2t64

# Video and UI libraries QGC depends on
sudo apt install gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl \
                 libxcb-xinerama0 libxkbcommon-x11-0 libxcb-cursor0
```

Then download `QGroundControl-x86_64.AppImage` from
[qgroundcontrol.com](https://qgroundcontrol.com) and run:

```bash
chmod +x QGroundControl-x86_64.AppImage
./QGroundControl-x86_64.AppImage
```

### 1.4 Check

Put the board in DFU mode (hold BOOT, plug in USB, release BOOT):

```bash
dfu-util -l    # without sudo: must list 0483:df11 "STM32 BOOTLOADER"
```

If it only works with `sudo`, the udev rule or group change from 1.2 is not
active yet: replug the board, and log out and in again.

---

## 2. Windows

### 2.1 Serial port

Nothing to install. Windows 10 and 11 recognise the board as a USB serial
device (`COMx`) automatically once firmware is running.

### 2.2 DFU driver

In DFU mode the board appears as **STM32 BOOTLOADER**. Which driver it needs
depends on the tool:

| Tool | Driver for STM32 BOOTLOADER |
| --- | --- |
| Betaflight Configurator, INAV Configurator | **WinUSB** |
| `dfu-util` for Windows | **WinUSB** |
| STM32CubeProgrammer | ST's own DFU driver, installed with CubeProgrammer |

**Install WinUSB (for the Configurators and dfu-util):**

*Option A: ImpulseRC Driver Fixer (easiest).* Put the board in DFU mode, run
the tool, and it installs the right driver automatically.

*Option B: Zadig.*

1. Put the board in DFU mode.
2. Run [Zadig](https://zadig.akeo.ie).
3. Menu **Options → List All Devices**.
4. Select **STM32 BOOTLOADER** from the list.
5. Select **WinUSB** as the target driver, then click **Replace Driver** (or **Install Driver**).

> [!WARNING]
> The WinUSB driver and ST's driver replace each other. Once STM32 BOOTLOADER
> uses WinUSB, STM32CubeProgrammer may stop detecting the board in DFU mode,
> and after installing CubeProgrammer's driver the Configurators may stop
> detecting it. Pick one tool and stay with it. To switch back, open
> **Device Manager**, right-click **STM32 BOOTLOADER** → **Uninstall device**
> (tick *Delete the driver software*), replug the board, then install the
> driver for the other tool.

### 2.3 Check

Put the board in DFU mode. In **Device Manager**, **STM32 BOOTLOADER** must
appear under *Universal Serial Bus devices* (WinUSB) or under *Universal Serial
Bus controllers* (ST driver), with no warning icon.

---

## 3. WSL2

WSL2 **cannot see USB devices by default**. `dfu-util -l` inside WSL lists
nothing until the device is forwarded from Windows with **usbipd-win**.

> [!TIP]
> Build in WSL, but flash from Windows with a Configurator or
> STM32CubeProgrammer (section 2). Copy the firmware file to a Windows folder,
> for example `cp firmware.hex /mnt/c/Users/<you>/Downloads/`. This avoids
> everything below.

### 3.1 Install usbipd-win (once)

In **PowerShell as Administrator**:

```powershell
winget install usbipd
```

Close and reopen PowerShell afterwards.

### 3.2 Forward the board to WSL

Keep a WSL terminal open, put the board in DFU mode, then in **PowerShell as Administrator**:

```powershell
usbipd list
# find the line "STM32 BOOTLOADER" (0483:df11) and note its BUSID, for example 2-3

usbipd bind   --busid 2-3
usbipd attach --wsl --busid 2-3 --auto-attach
```

Leave that PowerShell window open: `--auto-attach` re-forwards the device
whenever it reconnects. `bind` only needs to be done once per device.

In WSL:

```bash
sudo apt install dfu-util
sudo dfu-util -l    # must list 0483:df11
```

Use `sudo` for `dfu-util` in WSL.

### 3.3 After flashing

When flashing finishes, the board restarts as a **different USB device** (the
serial port of the running firmware). That device is still attached to Windows.

- To connect with a Windows ground station (QGroundControl, Mission Planner,
  Configurator), do nothing: the board is already available on Windows.
- To use it from WSL, run `usbipd list` again and `bind` / `attach` the new
  device the same way.

To give a device back to Windows:

```powershell
usbipd detach --busid 2-3
```
