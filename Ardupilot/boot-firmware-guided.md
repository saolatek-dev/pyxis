# Flashing firmware to the FC
* Put the FC into DFU mode: press and hold the BOOT button, then enter DFU mode.
* A newly manufactured STM32 needs the bootloader flashed first, then the firmware.
* Note: you must point to the correct directory, and flashing must be done on Ubuntu 24.

```bash
cd arupilot
```

```bash
sudo dfu-util -a 0 --dfuse-address 0x08000000 -D build/MyH754/bin/AP_Bootloader.bin
```

```bash
sudo dfu-util -a 0 --dfuse-address 0x08020000 -D build/MyH754/bin/arducopter.bin
```
