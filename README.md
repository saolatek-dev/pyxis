# H743VIT FC build guided

Hướng dẫn build và nạp firmware cho board bay **SAOLAH743** của Saolatek
(STM32H743VIT6, flash 2 MB).

Repo chứa **4 firmware**, mỗi thư mục là một dự án độc lập:

| Thư mục | Firmware | Giấy phép |
|---|---|---|
| [`Ardupilot/`](Ardupilot/) | ArduPilot | GPL-3.0 |
| [`BetaFlight/`](BetaFlight/) | Betaflight | GPL-3.0 |
| [`Inav/`](Inav/) | INAV | GPL-3.0 |
| [`PX4/`](PX4/) | PX4-Autopilot | BSD-3-Clause |

[`Docs/`](Docs/) — tài liệu phần cứng dùng chung (pinout, sơ đồ).

## Firmware dựng sẵn

Tải ở **[Releases](../../releases)**. Tag đặt theo tiền tố firmware:
`Ardupilot-*`, `BetaFlight-*`, `Inav-*`, `PX4-*`.

Không muốn tự build thì lấy file trong release; muốn tự build thì xem hướng
dẫn trong thư mục firmware tương ứng.

## Giấy phép

Bốn firmware là **bốn chương trình riêng biệt** — build riêng, nạp riêng,
không bao giờ link chung vào một binary. Giấy phép áp dụng **theo từng thư
mục**, không có giấy phép chung cho cả repo.

Ba firmware GPL-3.0 (ArduPilot, Betaflight, INAV) và PX4 BSD-3-Clause cùng
nằm trong một repo là hợp lệ theo điều khoản *mere aggregation* của GPLv3
(Điều 5).

> **Không sao chép mã giữa các thư mục.** Code GPL lọt vào `PX4/` sẽ buộc
> toàn bộ port PX4 thành GPLv3. Chia sẻ **số liệu phần cứng** (chân GPIO,
> giá trị điện trở, tần số thạch anh, kết quả đo) thì không vấn đề gì — dữ
> kiện không có bản quyền.

Mỗi thư mục có file `LICENSE` riêng. Mỗi release kèm file `SOURCE.md` ghim
commit upstream và danh sách thay đổi, để tái tạo lại đúng binary đã phát
hành.
