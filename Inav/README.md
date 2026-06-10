# SaolaH743 INAV Firmware

Dự án này chứa cấu hình firmware INAV dành cho mạch Flight Controller tùy chỉnh **SaolaH743**. Mạch này sử dụng vi điều khiển STM32H743, được thiết kế để cung cấp hiệu suất cao và đầy đủ các tính năng ngoại vi cho flycam/drone.

## Tính Năng & Cấu Hình Phần Cứng

Dưới đây là thông số phần cứng cơ bản của mạch dựa trên bản đồ chân (pinout) được tích hợp trong target INAV:

*   **Vi điều khiển (MCU):** STM32H743
*   **Cảm biến gia tốc/góc (IMU):** BMI270 (Giao tiếp qua SPI2). 
*   **Cảm biến áp suất (Barometer):** DPS310 (Giao tiếp qua I2C2 nội bộ)
*   **La bàn (Compass):** Hỗ trợ IST8310 hoặc QMC5883L (Giao tiếp qua I2C2 nội bộ)
*   **OSD:** AT7456E (Giao tiếp qua SPI1)
*   **Ghi log bay (Blackbox):** Hỗ trợ SD Card qua giao thức SDMMC
*   **Cổng Giao Tiếp (UARTs):** Hỗ trợ 8 cổng UART vật lý + 1 USB VCP
    *   `UART1`: TELEM1
    *   `UART2`: DJIO3
    *   `UART3`: GPS
    *   `UART4`: TELEM2
    *   `UART6`: RC INPUT (Có tích hợp phần cứng SBUS Inverter trên chân `PD0`)
    *   `UART7`: ESC telemetry
    *   `UART8`: TELEM3
*   **Cổng I2C:** 
    *   `I2C1`: Dành cho các thiết bị ngoại vi gắn ngoài (External connector)
    *   `I2C2`: Dành cho cảm biến nội bộ (Internal Baro/Mag)
*   **Đầu ra Motor/Servo (PWM):** Lên đến 10 cổng, hỗ trợ cấu hình DSHOT và DMAR.
    *   `M1-M4`: Dùng TIM1
    *   `M5-M6`: Dùng TIM3
    *   `M7-M10`: Dùng TIM4
*   **Cảm biến Nguồn (ADC):**
    *   Điện áp (Voltage): Chân `PC0` (Scale: 21.12)
    *   Dòng điện (Current): Chân `PC1` (Scale: 40.2)

Chi tiết về sơ đồ chân (Pinout) cụ thể cho các ngoại vi, vui lòng tham khảo tệp [pinout.txt](./pinout.txt).

## Hướng Dẫn Build Firmware (Biên dịch)

Quá trình cài đặt môi trường và biên dịch mã nguồn được mô tả cực kỳ chi tiết trong tệp [Build-guided.md](./Build-guided.md).

### Tóm tắt các bước cơ bản:

1. **Cài đặt các công cụ cần thiết:** `gcc-arm-none-eabi`, `make`, `cmake`, `git`.
2. **Tải mã nguồn INAV:**
   ```bash
   git clone https://github.com/iNavFlight/inav.git
   cd inav
   git checkout master
   ```
3. **Chuẩn bị thư mục Target:** 
   Sử dụng thư mục cấu hình `SaolaH743` có trong dự án này (nếu có các file code) và sao chép vào thư mục `src/main/target/SaolaH743/` của mã nguồn INAV vừa tải.
4. **Bắt đầu Build:**
   ```bash
   mkdir -p build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make SaolaH743 -j$(nproc)
   ```
   Nếu thành công, tệp firmware sẽ được lưu tại: `build/inav_SaolaH743.hex`.

## Hướng Dẫn Flash (Nạp Firmware)

1. Kết nối board SaolaH743 với máy tính qua cổng USB Type-C.
2. Mở ứng dụng **INAV Configurator**.
3. Chuyển sang tab **Firmware Flasher** ở menu bên trái.
4. Bấm **Load firmware [Local]** và chọn tệp `inav_SaolaH743.hex` vừa được tạo.
5. Bấm **Flash Firmware** để bắt đầu nạp.
   *(Nếu đây là board trắng, bạn cần giữ nút **BOOT** trong lúc cắm USB để vào chế độ DFU)*.
