# PX4 — Saolatek SAOLAH743

Port PX4 cho board bay SAOLAH743 (STM32H743VIT6, flash 2 MB).

## Nội dung thư mục

| Đường dẫn | Mô tả |
|---|---|
| `build-guided.md` | Hướng dẫn build firmware từ source |
| `flash-guided.md` | Hướng dẫn nạp firmware lên board |
| `SOURCE.md` | Ghim commit upstream + danh sách thay đổi (dùng cho release) |
| `boards/saolah743/h743/` | Board port PX4 (25 file) |
| `src/drivers/barometer/dps368/` | Driver baro Infineon DPS368 (tự viết) |
| `patches/upstream-changes.patch` | Sửa đổi cần áp vào 3 file PX4 gốc |
| `LICENSE` | BSD 3-Clause |

## Firmware dựng sẵn

Không muốn tự build thì tải ở **[Releases](../../releases)** — chọn tag có tiền tố `PX4-`.

Mỗi release gồm:

| File | Dùng khi nào |
|---|---|
| `*_factory.hex` / `.bin` | **Board mới/trống** — gộp sẵn bootloader + firmware, nạp một phát là chạy |
| `*.px4` | Board **đã có** bootloader PX4 — cập nhật qua QGroundControl |
| `saolah743_h743_bootloader.bin` | Chỉ nạp riêng bootloader |

Hai biến thể theo cảm biến áp suất gắn trên board:

- `default` → baro **DPS310**
- `dps368` → baro **DPS368**

Cảm biến IMU **không cần chọn** — cả hai bản tự dò BMI088/BMI270 lúc khởi động, nên dùng chung được cho board 1 IMU lẫn board 2 IMU.

## Thông số định danh

| | |
|---|---|
| Board ID | `6130` (bootloader và firmware phải khớp nhau) |
| USB VID:PID | `0x1209:0x7743` |
| Bootloader | `0x08000000`, 128 KiB |
| Firmware | `0x08020000`, tối đa 1792 KiB |
| Tham số | sector 15, `0x081E0000` |

## Giấy phép

PX4-Autopilot dùng **BSD 3-Clause**. Port này là tác phẩm phái sinh nên cũng theo BSD 3-Clause — xem [LICENSE](LICENSE). Các file giữ nguyên dòng bản quyền của PX4 Development Team.

> Lưu ý: khác với ArduPilot/Betaflight/INAV trong repo này (đều là **GPLv3**). Không sao chép mã giữa hai vùng — code GPL lọt vào thư mục `PX4/` sẽ buộc toàn bộ port này thành GPLv3.
