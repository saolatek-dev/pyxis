# SOURCE — PX4 SAOLAH743

File này cho biết firmware trong release được dựng từ đâu, để bất kỳ ai
cũng tái tạo lại được đúng binary. **Kèm file này vào mỗi release.**

## Upstream

```
Dự án:  PX4-Autopilot
URL:    https://github.com/PX4/PX4-Autopilot
Commit: efd05431e8426d5a788ad814946bfba2f5da893e
Ngày:   2026-04-26
```

Phải dùng **đúng commit này**. Ghi "PX4 v1.17" là không đủ — không xác định
được duy nhất một cây mã.

## Thay đổi của Saolatek

### Thêm mới

| Đường dẫn trong cây PX4 | Nguồn trong repo này |
|---|---|
| `boards/saolah743/h743/` | `PX4/boards/saolah743/h743/` |
| `src/drivers/barometer/dps368/` | `PX4/src/drivers/barometer/dps368/` |

### Sửa file PX4 gốc

Áp `PX4/patches/upstream-changes.patch`, gồm 3 file:

| File | Thay đổi |
|---|---|
| `src/drivers/barometer/CMakeLists.txt` | `+add_subdirectory(dps368)` |
| `src/drivers/drv_sensor.h` | `+#define DRV_BARO_DEVTYPE_DPS368 0x64` |
| `src/drivers/barometer/dps310/DPS310.cpp` | Thử lại 5 lần khi đọc Product ID (một lần đọc I2C lỗi để lại `buf = 0`, trông y hệt "Product_ID mismatch" lúc boot); đếm `comm errors` khi `read()` fail thay vì nuốt im lặng |

**Thiếu 2 dòng đầu thì driver DPS368 không được biên dịch vào firmware** —
build vẫn chạy nhưng baro DPS368 im lặng, rất khó truy.

## Dựng lại

```bash
git clone https://github.com/PX4/PX4-Autopilot.git
cd PX4-Autopilot
git checkout efd05431e8426d5a788ad814946bfba2f5da893e
git submodule update --init --recursive

PYXIS=/duong/dan/toi/pyxis
cp -r "$PYXIS/PX4/boards/saolah743"            boards/
cp -r "$PYXIS/PX4/src/drivers/barometer/dps368" src/drivers/barometer/
git apply "$PYXIS/PX4/patches/upstream-changes.patch"

make saolah743_h743            # baro DPS310
make saolah743_h743_dps368     # baro DPS368
make saolah743_h743_bootloader # bootloader
```

Gộp ảnh factory (bootloader + firmware vào một file):

```bash
python3 - <<'EOF'
bl  = open('boards/saolah743/h743/extras/saolah743_h743_bootloader.bin','rb').read()
app = open('build/saolah743_h743_default/saolah743_h743_default.bin','rb').read()
open('factory.bin','wb').write(bl + b'\xff'*(0x20000-len(bl)) + app)
EOF
arm-none-eabi-objcopy -I binary -O ihex --change-addresses 0x08000000 \
  factory.bin factory.hex
```

## Checksum các file trong release

```
MD5                               Bytes     File
1b5f8f91c93c0385853bff630f154043  1767384   saolah743_h743_default_factory.bin
8c2ca672a87203f9e01133b9958949e0  4971267   saolah743_h743_default_factory.hex
0e0d5fc2ffe9cf551c11d6d27a050098  1767384   saolah743_h743_dps368_factory.bin
735afe4f3b01eb1eda3152813a36b80a  4971267   saolah743_h743_dps368_factory.hex
72ca2c0ecd3bc8b1628b517b1312f9f3    40944   saolah743_h743_bootloader.bin
8c0a8c71678908a38b426ea846f70646  1548314   saolah743_h743_default.px4
264b0341973f4238482e453ac10ca988  1548318   saolah743_h743_dps368.px4
```

Kiểm tra sau khi tải: `md5sum -c` hoặc `md5sum <file>` rồi đối chiếu.

## Giấy phép

PX4-Autopilot: **BSD 3-Clause**. Các file phái sinh giữ nguyên dòng bản
quyền của PX4 Development Team. Xem `PX4/LICENSE`.
