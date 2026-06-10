/*
 * target.h — INAV target cho SaolaH743
 *
 * MCU     : STM32H743
 * IMU     : BMI270 (SPI2)
 * Baro    : DPS310 (I2C2)
 * Mag     : IST8310 / QMC5883L (I2C2)
 * OSD     : AT7456E (SPI1)
 * BB      : SD card (SDMMC1)
 *
 * QUAN TRỌNG — KHÔNG CÓ THẠCH ANH NGOÀI:
 *   hwdef ArduPilot khai báo OSCILLATOR_HZ 0
 *   → Board dùng HSI nội bộ (64MHz) thay vì HSE crystal
 *   → USB clock lấy từ HSI48 (internal 48MHz oscillator riêng của H743)
 *   → INAV mặc định expect HSE 8MHz, phải override bằng 2 define bên dưới:
 *        USE_FAST_DATA
 *        STM32_HSE_VALUE không được define (hoặc để INAV tự dùng HSI path)
 *   → Nếu thiếu, USB VCP sẽ không ổn định, Configurator không kết nối được
 */

#pragma once

// ============================================================
// BOARD IDENTITY
// ============================================================
#define TARGET_BOARD_IDENTIFIER     "SAOL"
#define USBD_PRODUCT_STRING         "SaolaH743"

// ============================================================
// CLOCK — KHÔNG CÓ CRYSTAL NGOÀI
// ============================================================
// Board dùng HSI (internal 64MHz), không có HSE crystal.
// Bỏ define HSE_VALUE để INAV dùng đường clock HSI.
// USB hoạt động nhờ HSI48 (dedicated oscillator trong H743,
// đủ chính xác cho USB FS mà không cần crystal ngoài).
#ifndef HSE_VALUE
#ifndef HSE_VALUE
#define HSE_VALUE 0
#endif
#endif
#define USE_OVERCLOCK               // H743 HSI path cần flag này để PLL lock đúng

// ============================================================
// LED STATUS
// ============================================================
// Board có RGB 3 chân riêng (PE4/PE5/PE6), active LOW.
// INAV chỉ dùng 1 LED status — map vào LED Green.
// LED0_INVERTED vì active LOW (LED sáng khi GPIO = LOW).
#define LED0_PIN                    PE6     // LED Green
#define LED0_INVERTED
#define LED1_PIN                    PE5     // LED Red (tuỳ chọn, dùng cho warning)
#define LED1_INVERTED

// ============================================================
// USB VCP
// ============================================================
// PA11/PA12 là OTG_FS DM/DP — khớp với hwdef ArduPilot.
// Không cần VBUS sensing (BOARD_OTG_NOVBUSSENS trong ArduPilot).
#define USE_VCP
// Không define USB_DETECT_PIN vì board không có chân VBUS detect riêng

// ============================================================
// UART
// ============================================================
// Tổng: 8 UART + 1 VCP = 9 port → SERIAL_PORT_COUNT = 9
//
// UART1  — TELEM1        (PA9/PA10)
// UART2  — DJIO3         (PA2/PA3)   ← PA2/PA3 cũng là UART4 TX/RX trong H743
//                                        nếu conflict thì đổi sang USART2 alt pin
// UART3  — GPS           (PD8/PD9)
// UART4  — TELEM2        (PA0/PA1)
// UART6  — RC INPUT      (PC6/PC7)   + SBUS inverter PD0
// UART7  — ESC telemetry (PE7 RX only)
// UART8  — TELEM3        (PE0/PE1)

#define USE_UART1
#define UART1_TX_PIN                PA9
#define UART1_RX_PIN                PA10

#define USE_UART2
#define UART2_TX_PIN                PA2
#define UART2_RX_PIN                PA3

#define USE_UART3
#define UART3_TX_PIN                PD8
#define UART3_RX_PIN                PD9

#define USE_UART4
#define UART4_TX_PIN                PA0
#define UART4_RX_PIN                PA1

#define USE_UART6
#define UART6_TX_PIN                PC6
#define UART6_RX_PIN                PC7

#define USE_UART7
// UART7 chỉ có RX (ESC telemetry — half duplex / một chiều)
#define UART7_RX_PIN                PE7

#define USE_UART8
#define UART8_TX_PIN                PE1
#define UART8_RX_PIN                PE0

#define SERIAL_PORT_COUNT           9   // 8 UART + 1 VCP — phải đếm đúng, sai thì crash

// SBUS inverter — kéo LOW để enable inverter hardware trên board
// INAV tự toggle pin này khi set Serial RX = SBUS
#define SBUS_INVERTER_CONTROL_PIN   PD0

// ============================================================
// SPI
// ============================================================
// SPI1 — OSD (AT7456E): PA5/PA6/PA7 — pin chuẩn SPI1 AF5
// SPI2 — IMU (BMI270 + BMI088): PD3/PC2/PC3
//         Lưu ý: PD3/PC2/PC3 là alternate pin của SPI2 (AF5)
//         Verify trong datasheet H743 Table 9 rằng đúng AF5
//         trước khi build — nếu sai AF thì SPI im lặng, không báo lỗi

#define USE_SPI
#define USE_SPI_DEVICE_1
#define SPI1_SCK_PIN                PA5
#define SPI1_MISO_PIN               PA6
#define SPI1_MOSI_PIN               PA7

#define USE_SPI_DEVICE_2
#define SPI2_SCK_PIN                PD3
#define SPI2_MISO_PIN               PC2
#define SPI2_MOSI_PIN               PC3

// ============================================================
// IMU — BMI270
// ============================================================
// Board có cả BMI270 (PA15) và BMI088 (PD4/PD5) trên SPI2.
// INAV chỉ support 1 primary IMU — dùng BMI270, bỏ BMI088.
//
// ROTATION: hwdef ArduPilot dùng ROTATION_ROLL_180 cho BMI270
// → tương đương CW0_DEG_FLIP trong INAV
// → Có thể chỉnh lại trong Configurator sau khi flash nếu chưa chắc

#define USE_IMU_BMI270
#define IMU_BMI270_ALIGN            CW0_DEG_FLIP    // = ROTATION_ROLL_180 trong ArduPilot
#define BMI270_CS_PIN               PA15
#define BMI270_SPI_BUS        BUS_SPI2

// ============================================================
// I2C
// ============================================================
// I2C1 — connector ngoài (PB6/PB7) — external GPS mag, v.v.
// I2C2 — sensor nội bộ (PB10/PB11, có PULLUP) — Baro + Mag

#define USE_I2C
#define USE_I2C_DEVICE_1
#define I2C1_SCL                    PB6
#define I2C1_SDA                    PB7

#define USE_I2C_DEVICE_2
#define I2C2_SCL                    PB10
#define I2C2_SDA                    PB11

// ============================================================
// BAROMETER — DPS310 trên I2C2
// ============================================================
#define USE_BARO
#define USE_BARO_DPS310
#define BARO_I2C_INSTANCE           (I2CDEV_2)  // I2C2 = index 1 (0-based)

// ============================================================
// MAGNETOMETER — IST8310 / QMC5883L trên I2C2
// ============================================================
// Board có thể dùng IST8310 hoặc QMC5883L tuỳ lô linh kiện.
// Khai báo cả hai — INAV sẽ probe và nhận cái nào trả lời.
#define USE_MAG
#define USE_MAG_IST8310
#define USE_MAG_QMC5883
#define MAG_I2C_INSTANCE            (I2CDEV_2)

// ============================================================
// OSD — AT7456E trên SPI1
// ============================================================
#define USE_MAX7456
#define MAX7456_SPI_BUS       BUS_SPI1
#define MAX7456_CS_PIN          PB12
#define MAX7456_SPI_CLK             (SPI_CLOCK_STANDARD)

// ============================================================
// MOTOR / DSHOT
// ============================================================
// M1-M4: TIM1 (PE9/PE11/PE13/PE14)
// M5-M6: TIM3 (PB0/PB1)
// M7-M10: TIM4 (PD12/PD13/PD14/PD15)
//
// USE_DSHOT_DMAR — BẮT BUỘC với H743
// H743 dùng BDMA cho timer DMA request, khác hoàn toàn F4/F7.
// Thiếu define này DShot không phát tín hiệu dù build không lỗi.

#define USE_DSHOT
#define USE_DSHOT_DMAR

#define MOTOR1_PIN                  PE14    // TIM1_CH4  BIDIR
#define MOTOR2_PIN                  PE13    // TIM1_CH3
#define MOTOR3_PIN                  PE11    // TIM1_CH2  BIDIR
#define MOTOR4_PIN                  PE9     // TIM1_CH1
#define MOTOR5_PIN                  PB1     // TIM3_CH4  BIDIR
#define MOTOR6_PIN                  PB0     // TIM3_CH3
#define MOTOR7_PIN                  PD12    // TIM4_CH1
#define MOTOR8_PIN                  PD13    // TIM4_CH2
// M9/M10 dùng nếu cần (hex, octo):
// #define MOTOR9_PIN               PD14    // TIM4_CH3
// #define MOTOR10_PIN              PD15    // TIM4_CH4

// ============================================================
// ADC — đo pin / dòng
// ============================================================
// Scale lấy từ hwdef: volt=21.12, curr=40.2
// Đây là hệ số của voltage divider và current sensor phần cứng
// → Cần config lại đúng giá trị này trong Configurator (Power tab)

#define USE_ADC
#define ADC_INSTANCE                ADC1
#define VBAT_ADC_PIN                PC0
#define CURRENT_METER_ADC_PIN       PC1

// ============================================================
// SDCARD — blackbox qua SDMMC1
// ============================================================
#define USE_SDCARD
#define USE_SDCARD_SDIO
#define SDCARD_SDIO_DEVICE          SDIODEV_1
#define SDCARD_SDIO_4BIT
// Không có chân SDCARD_DETECT_PIN trên board này

// ============================================================
// CAN (khai báo để không conflict GPIO, không dùng trong INAV cơ bản)
// ============================================================
// PB8/PB9 là CAN1 — INAV không dùng CAN nhưng cần
// đảm bảo 2 pin này không bị gán chức năng khác
// #define USE_CAN
// #define CAN1_RX_PIN              PB8
// #define CAN1_TX_PIN              PB9

// ============================================================
// TARGET I/O PORT MASK
// ============================================================
// 0xffff = cho phép INAV dùng tất cả 16 pin của mỗi port.
// Với FC DIY không có lý do để hạn chế port nào.
#define TARGET_IO_PORTA             0xffff
#define TARGET_IO_PORTB             0xffff
#define TARGET_IO_PORTC             0xffff
#define TARGET_IO_PORTD             0xffff
#define TARGET_IO_PORTE             0xffff

// ============================================================
// OUTPUT COUNT
// ============================================================
#define MAX_PWM_OUTPUT_PORTS        10  // 10 motor output (M1-M10)

// ============================================================
// DEFAULT CONFIG — áp dụng lần đầu flash, có thể override trong Configurator
// ============================================================
#define DEFAULT_FEATURES            (FEATURE_TX_PROF_SEL | FEATURE_BLACKBOX | FEATURE_OSD)
#define DEFAULT_RX_TYPE             RX_TYPE_SERIAL          // Serial RX (SBUS/CRSF/...)
#define DEFAULT_VOLTAGE_METER_SOURCE  VOLTAGE_METER_ADC
#define DEFAULT_CURRENT_METER_SOURCE  CURRENT_METER_ADC