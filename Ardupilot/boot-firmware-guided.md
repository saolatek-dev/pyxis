# Hướng dẫn nạp firmware cho FC
* Phải vào DFU mode trên FC: nhấn giữ nút boot sau đó vào chế độ DFU
* Với Stm32 mới sản xuất cần nạp bootloader trước, sau đó nạp firmware sau.
* Lưu ý: phải trỏ đến đúng thư mục, bắt buộc nạp bằng ubuntu 24.

```bash
cd arupilot
```

```bash
sudo dfu-util -a 0 --dfuse-address 0x08000000 -D build/MyH754/bin/AP_Bootloader.bin
```

```bash
sudo dfu-util -a 0 --dfuse-address 0x08020000 -D build/MyH754/bin/arducopter.bin
```
