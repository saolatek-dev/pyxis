# Hướng dẫn Build INAV Firmware

Tài liệu này cung cấp các bước cơ bản để cài đặt môi trường và cấu hình build firmware INAV cho một mạch Flight Controller (FC) tùy chỉnh.

## 1. Cài đặt các công cụ cần thiết (Dependencies)

Trước tiên, bạn cần cài đặt các công cụ biên dịch (build tools) và công cụ quản lý mã nguồn. Chạy lệnh sau trong Terminal:

```bash
sudo apt update
sudo apt install gcc-arm-none-eabi make git dfu-util
```

## 2. Tải mã nguồn INAV

Tiến hành tải mã nguồn của INAV từ kho lưu trữ GitHub chính thức và chuyển sang nhánh `master` (hoặc nhánh phiên bản bạn muốn):

```bash 
git clone https://github.com/iNavFlight/inav.git
cd inav 
git checkout master
```

## 3. Tạo thư mục cấu hình cho Flight Controller (Target)

Để biên dịch firmware cho một mạch FC cụ thể (ví dụ: `SaolaH743`), bạn cần tạo một thư mục target mới chứa các thiết lập phần cứng cho mạch đó:

```bash
mkdir -p src/main/target/SaolaH743
cd src/main/target/SaolaH743/
touch target.h target.c CMakeLists.txt
```

> **Lưu ý:** Tên thư mục target thường là tên viết liền của Flight Controller và sẽ được dùng làm tên lệnh build sau này.

### Cấu trúc thư mục của một Target

Bên trong thư mục target vừa tạo (`src/main/target/SaolaH743/`), bạn sẽ cần tạo các file cấu hình. Dưới đây là cấu trúc các file thiết yếu và chức năng của chúng:

```text
src/main/target/SaolaH743/
├── CMakeLists.txt    # Khai báo loại chip (MCU) và các cờ biên dịch (build flags)
├── target.h          # Khai báo, định nghĩa tất cả các chân (pin) và thiết bị ngoại vi (peripheral)
├── target.c          # Code khởi tạo đặc biệt dành riêng cho mạch (thường ở mức tối giản - minimal)
└── config.c          # (Tùy chọn) Chứa cấu hình mặc định (default config) khi flash firmware lần đầu tiên
```


## 4. Hướng dẫn Build (Biên dịch) Firmware

Để bắt đầu quá trình biên dịch (build), chúng ta sẽ sử dụng công cụ CMake. Những thứ kiện quyết bạn cần là: `gcc-arm-none-eabi`, `make` và `cmake`.

Chạy chuỗi lệnh sau trong cửa sổ Terminal:

```bash
# 1. Quay lại thư mục gốc của project INAV
cd /đường/dẫn/tới/thư/mục/inav

# 2. Tạo và di chuyển vào thư mục build
mkdir -p build && cd build

# 3. Tạo cấu hình Makefiles (bản Release cho hiệu năng tốt nhất)
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. Tiến hành Build (Thay thế 'SaolaH743' bằng tên Target của bạn)
make SaolaH743
```

> **Ghi chú quá trình Build:** Sau khi chạy lệnh `make`, CMake sẽ gọi bộ công cụ biên dịch ARM GCC. Nếu thành công, INAV sẽ tạo một bản thực thi `.elf` tại `build/bin/SaolaH743.elf` và ngay sau đó được tự động convert thành tệp Hex: **`build/inav_SaolaH743.hex`**.

## 5. Nạp Firmware (Flash) vào Board

Sau khi đã có file `.hex`, bạn có thể flash nó vào Flight Controller:
1. Cắm cáp USB kết nối board với máy tính.
2. Mở ứng dụng **INAV Configurator**.
3. Chuyển sang tab **Firmware Flasher** ở menu bên trái.
4. Nhấn nút **Load firmware [Local]** (Tải tệp firmware từ máy).
5. Chọn tệp `inav_SaolaH743.hex` vừa được build thành công.
6. Bấm **Flash Firmware**.
   *Nếu cổng COM (VCP) chưa hoạt động do board mới tinh, hãy nhấn giữ nút **BOOT** vật lý trên board khi cắm USB để vào chế độ **DFU mode** và thực hiện flash.*


Kiểm tra trước khi build:

Kiểm tra INAV có support BMI270 chưa
cd inav 
grep -r "BMI270" src/main/ --include="*.h" --include="*.c" -l

 Kiểm tra SystemClock_Config không bị override
 grep -r "SystemClock_Config" src/main/ --include="*.c" -l


# Build và Flash

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make SaolaH743 -j$(nproc) 2>&1 | tee ../build.log
tail -20 ../build.log
