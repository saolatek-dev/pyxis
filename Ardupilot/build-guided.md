# Hướng dẫn build firmware ArduPilot cho FC H743VIT Saolatek

> **Lưu ý:** Bắt buộc build trên Ubuntu 24.04. Có thể sử dụng WSL thay thế.

## Clone source code ArduPilot về máy

```bash
git clone https://github.com/ArduPilot/ardupilot.git
```

## Thiết lập môi trường build

```bash
cd ardupilot
```
## Cập nhật submodule:
```bash
git submodule update --init --recursive
```
## Chạy script cài đặt các package cần thiết cho môi trường build:
```bash
Tools/environment_install/install-prereqs-ubuntu.sh -y
```
## Sau khi cài đặt hoàn tất, nạp lại biến môi trường:
```bash
source ~/.profile
```
## Kiểm tra công cụ build đã được cài đặt thành công:
```bash
./waf --version
```
