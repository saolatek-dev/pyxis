# Hướng dẫn build firmware PX4 cho SAOLAH743

Không muốn tự build thì tải bản dựng sẵn ở **[Releases](../../releases)** (tag `PX4-*`) và xem [flash-guided.md](flash-guided.md).

## 1. Cài môi trường

```bash
sudo apt update
sudo apt install git python3-pip cmake ninja-build \
                 gcc-arm-none-eabi binutils-arm-none-eabi
```

## 2. Lấy source PX4 đúng phiên bản

```bash
git clone https://github.com/PX4/PX4-Autopilot.git
cd PX4-Autopilot
git checkout efd05431e8426d5a788ad814946bfba2f5da893e
git submodule update --init --recursive
```

Phải đúng commit này. Bản khác có thể không build được với port hiện tại.

## 3. Áp port SAOLAH743

Giả sử repo `pyxis` nằm ở `~/pyxis`:

```bash
PYXIS=~/pyxis

cp -r "$PYXIS/PX4/boards/saolah743"             boards/
cp -r "$PYXIS/PX4/src/drivers/barometer/dps368"  src/drivers/barometer/
git apply "$PYXIS/PX4/patches/upstream-changes.patch"
```

**Bước `git apply` là bắt buộc.** Patch thêm 2 dòng để driver DPS368 được
biên dịch vào firmware. Bỏ qua thì build vẫn chạy, nhưng baro DPS368 sẽ
không hoạt động và rất khó tìm ra nguyên nhân.

## 4. Build

Chọn theo cảm biến áp suất gắn trên board:

```bash
make saolah743_h743            # baro DPS310
make saolah743_h743_dps368     # baro DPS368
```

IMU không cần chọn — cả hai bản tự dò BMI088/BMI270 lúc khởi động.

Kết quả:

```
build/saolah743_h743_default/saolah743_h743_default.px4
build/saolah743_h743_dps368/saolah743_h743_dps368.px4
```

File `.px4` nạp qua QGroundControl, với điều kiện board **đã có sẵn**
bootloader PX4.

## 5. Build bootloader (chỉ khi cần)

```bash
make saolah743_h743_bootloader
```

Kết quả ghi đè vào `boards/saolah743/h743/extras/saolah743_h743_bootloader.bin`
(CMake tự làm việc này).

## 6. Gộp ảnh factory (tuỳ chọn)

Dùng cho board mới/trống — một file chứa cả bootloader lẫn firmware:

```bash
python3 - <<'EOF'
bl  = open('boards/saolah743/h743/extras/saolah743_h743_bootloader.bin','rb').read()
app = open('build/saolah743_h743_default/saolah743_h743_default.bin','rb').read()
assert len(bl) <= 0x20000
open('factory.bin','wb').write(bl + b'\xff'*(0x20000-len(bl)) + app)
EOF

arm-none-eabi-objcopy -I binary -O ihex --change-addresses 0x08000000 \
  factory.bin factory.hex
```

File `.hex` mang sẵn địa chỉ nên STM32CubeProgrammer không cần nhập Start Address.

## Xử lý sự cố

**Lỗi CMake/Kconfig lạ** (ví dụ `redefinition of 'get_latency'`, hoặc thiếu
file `Kconfig` trong `platforms/nuttx/NuttX/apps/...`): thường do cache
CMake hỏng.

```bash
rm -rf build/saolah743_h743_<config>
make saolah743_h743_<config>
```

**Build xong nhưng baro DPS368 không hoạt động**: gần như chắc chắn quên
bước `git apply` ở mục 3.

**Cảnh báo flash gần đầy**: bình thường. Firmware hiện dùng ~89% của 1792 KiB.
