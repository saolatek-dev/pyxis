# Hướng dẫn build Betaflight cho board SaolaH743

## Bước 1 — Clone source code

```bash
git clone https://github.com/betaflight/betaflight.git
cd betaflight
```

## Bước 2 — Tạo thư mục target

```bash
mkdir src/main/target/SAOLAH743
```

Tạo 2 file bên trong: `CMakeLists.txt` và `target.h`.

**`CMakeLists.txt`**:
```cmake
target_stm32h743xx(SAOLAH743)
```

## Bước 3 — Viết `target.h`

Map các pin theo hwdef gốc (UART, SPI, I2C, IMU, baro, mag, OSD, motor, ADC...). Xem nội dung đầy đủ ở phần trả lời trước.

## Bước 4 — Cài toolchain & build

**Cài ARM toolchain (Ubuntu/WSL):**
```bash
sudo apt install gcc-arm-none-eabi
```

**Build:**
```bash
make SAOLAH743
```

File output: `obj/betaflight_SAOLAH743.hex`

**Hoặc build bằng Docker (không cần cài toolchain):**
```bash
docker run --rm -v $(pwd):/betaflight betaflight/build SAOLAH743
```

## Bước 5 — Flash firmware

Vào DFU mode: giữ nút **BOOT0** rồi cắm USB.

**Cách 1 — qua Betaflight Configurator:**
Tab Firmware Flasher → Load Firmware (Local) → chọn file `.hex`

**Cách 2 — qua dfu-util:**
```bash
dfu-util -D obj/betaflight_SAOLAH743.hex
```

## Bước 6 — Verify

Mở Betaflight Configurator, kiểm tra:
- IMU hoạt động (BMI088 / BMI270)
- UART nhận/gửi đúng (TELEM1, TELEM2, GPS, RC input)
- Motor output (DSHOT)
- OSD hiển thị (MAX7456)

---

## Lưu ý quan trọng

File `.hwdef` ban đầu là định dạng **ArduPilot**, không phải Betaflight. Hai firmware dùng hệ thống build và cấu hình hoàn toàn khác nhau:

| | ArduPilot | Betaflight |
|---|---|---|
| File cấu hình | `hwdef.dat` | `target.h` + `CMakeLists.txt` |
| Build system | waf | make / CMake |
| Vị trí | `libraries/AP_HAL_ChibiOS/hwdef/` | `src/main/target/` |

Pin assignment trong `target.h` ở Bước 3 được dịch trực tiếp từ file hwdef ArduPilot bạn cung cấp, áp dụng cho MCU **STM32H743** (dòng H7).
