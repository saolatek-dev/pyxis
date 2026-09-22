# Hướng dẫn nạp firmware PX4 lên SAOLAH743

## Chọn file nào

| Tình huống board | File dùng | Công cụ |
|---|---|---|
| Mới / trống / đang chạy Betaflight–INAV–ArduPilot | `*_factory.hex` | CubeProgrammer, dfu-util, INAV Configurator, SWD |
| Đã có bootloader PX4 rồi | `*.px4` | QGroundControl |
| Chỉ cần nạp lại bootloader | `saolah743_h743_bootloader.bin` | dfu-util, SWD |

Chọn biến thể theo cảm biến áp suất trên board: `default` = **DPS310**, `dps368` = **DPS368**.

---

## Cách 1 — Ảnh factory (board mới/trống)

Gộp sẵn bootloader + firmware, nạp một phát là board chạy được.

### STM32CubeProgrammer

1. Giữ nút **BOOT**, cắm USB → board vào ROM DFU (`0483:df11`)
2. Chọn file `saolah743_h743_default_factory.hex`
3. Download

**Không cần nhập Start Address** — file HEX đã mang sẵn địa chỉ (dòng đầu
`:020000040800F2` = base `0x08000000`).

### dfu-util

Dùng bản `.bin`, phải chỉ rõ địa chỉ:

```bash
dfu-util -a 0 -s 0x08000000:mass-erase:force \
  -D saolah743_h743_default_factory.bin
```

Đợi `File downloaded successfully` → **thả nút BOOT** → rồi mới rút/cắm lại USB.

> Đừng thêm `:leave` khi tay còn giữ BOOT. MCU reset ngay lúc đó, đọc thấy
> BOOT0 vẫn ở mức cao nên quay lại ROM DFU thay vì chạy firmware vừa nạp —
> trông y hệt như nạp hỏng.

### INAV Configurator

Dùng được. Tab **Firmware Flasher** → **"Load firmware [local]"** (không chọn
board từ danh sách online) → trỏ vào file `.hex`. Có bước verify byte-for-byte
sau khi ghi.

Cơ chế nạp của nó là client DfuSe chuẩn — đọc layout flash từ chính descriptor
USB của MCU, ghi theo địa chỉ có sẵn trong file HEX, không kiểm tra
target/board_id nên không từ chối firmware PX4.

### SWD (ST-Link)

```bash
st-flash --reset write saolah743_h743_default_factory.bin 0x08000000
```

---

## Cách 2 — QGroundControl (board đã có bootloader PX4)

Vehicle Setup → Firmware → Advanced settings → **Custom firmware file...** →
chọn `saolah743_h743_default.px4`.

Rút/cắm lại USB khi QGC báo chờ thiết bị.

> QGC **không nạp được bootloader** — nó chỉ ghi phần firmware thông qua
> bootloader đã có sẵn. Board trống bắt buộc phải qua Cách 1 một lần.

---

## Kiểm tra sau khi nạp

Board lên sau khoảng 2 giây:

```bash
lsusb | grep 1209      # thấy: 1209:7743 Generic Saolah743
ls /dev/ttyACM*
```

---

## ⚠️ Board ID phải khớp

QGroundControl chỉ nhận file `.px4` có `board_id` **trùng** với bootloader
đang nằm trên board.

Firmware trong repo này dùng `board_id = 6130` cho cả bootloader lẫn
firmware. Nếu board đang mang bootloader có ID khác, QGC sẽ báo sai board và
từ chối — khi đó nạp lại ảnh factory qua Cách 1 để đồng bộ.

---

## Xử lý sự cố

**Nạp xong board không lên COM** — kiểm tra xem có phải board quay lại ROM DFU
không:

```bash
dfu-util -l          # còn thấy 0483:df11 nghĩa là chưa rời DFU
```

Nguyên nhân phổ biến nhất: chân BOOT0 vẫn giữ mức cao lúc MCU reset. Thả nút
BOOT rồi cúp nguồn cắm lại.

**Đang gắn SWD mà board im** — board này **không nối chân NRST** ra header SWD,
nên gắn đầu dò vào là CPU bị halt đứng im, trông như chết. Chạy `st-flash reset`
hoặc rút đầu dò ra trước khi kết luận firmware hỏng.

**Betaflight/INAV Configurator báo "Successful" nhưng board không chạy** — chữ
đó chỉ nghĩa là *đã ghi xong byte*, không xác nhận firmware khởi động được.
Nạp lại bằng CubeProgrammer trước khi nghi firmware lỗi.
