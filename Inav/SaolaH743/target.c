/*
 * target.c — SaolaH743
 *
 * Với board có crystal ngoài (HSE), file này thường để trống.
 * Board này KHÔNG có crystal → phải override SystemClock_Config()
 * để INAV dùng HSI (64MHz internal) thay vì chờ HSE lock.
 *
 * Nếu không có file này, INAV sẽ:
 *   1. Bật HSE → chờ HSERDY flag
 *   2. Chờ mãi vì không có crystal → timeout → chip treo ở boot
 *   3. USB không hoạt động, Configurator không kết nối được
 */

#include <stdint.h>
#include "platform.h"
#include "target.h"
#include "drivers/bus.h"
#include "drivers/io.h"
#include "drivers/pwm_mapping.h"
#include "drivers/timer.h"
#include "drivers/pinio.h"
#include "drivers/sensor.h"

// ============================================================
// SYSTEM CLOCK — HSI path (không có crystal ngoài)
// ============================================================
//
// STM32H743 clock tree khi dùng HSI:
//
//   HSI (64MHz)
//     └── PLL1
//           ├── DIVM1 = /4  → PLL input = 16MHz  (phải 1–16MHz)
//           ├── MULN1 = x25 → VCO = 400MHz
//           ├── DIVP1 = /2  → SYSCLK = 200MHz    ← CPU clock
//           ├── DIVQ1 = /4  → PLL1Q = 100MHz     → SPI/UART
//           └── DIVR1 = /2  → PLL1R = 200MHz     → để trống
//
//   HSI48 (48MHz, dedicated oscillator)
//     └── USB FS clock                           ← USB hoạt động nhờ đây
//         (không cần crystal, đủ chính xác cho USB)
//
// Kết quả: SYSCLK = 200MHz, AHB = 200MHz, APB1/2 = 100MHz
// Đây là cấu hình giống MATEKH743 (board reference của INAV cho H743)

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    // --- Bật HSI + HSI48, tắt HSE (không có crystal) ---
    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI |    // 64MHz internal — nguồn cho PLL1
        RCC_OSCILLATORTYPE_HSI48 |  // 48MHz internal — nguồn cho USB
        RCC_OSCILLATORTYPE_HSE;     // khai báo để tắt tường minh

    RCC_OscInitStruct.HSEState       = RCC_HSE_OFF;     // TẮT HSE — không có crystal
    RCC_OscInitStruct.HSIState       = RCC_HSI_DIV1;    // HSI on, không chia (64MHz)
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI48State     = RCC_HSI48_ON;    // Bật HSI48 cho USB

    // --- Cấu hình PLL1 từ HSI ---
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSI; // nguồn = HSI 64MHz
    RCC_OscInitStruct.PLL.PLLM       = 4;    // 64MHz / 4 = 16MHz (PLL input)
    RCC_OscInitStruct.PLL.PLLN       = 25;   // 16MHz * 25 = 400MHz (VCO)
    RCC_OscInitStruct.PLL.PLLP       = 2;    // 400MHz / 2 = 200MHz → SYSCLK
    RCC_OscInitStruct.PLL.PLLQ       = 4;    // 400MHz / 4 = 100MHz → SPI/UART
    RCC_OscInitStruct.PLL.PLLR       = 2;    // 400MHz / 2 = 200MHz → không dùng
    RCC_OscInitStruct.PLL.PLLRGE     = RCC_PLL1VCIRANGE_2; // VCI range cho 8–16MHz input
    RCC_OscInitStruct.PLL.PLLVCOSEL  = RCC_PLL1VCOWIDE;    // VCO wide range (192–960MHz)
    RCC_OscInitStruct.PLL.PLLFRACN   = 0;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    // --- Cấu hình bus clock divider ---
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK   |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1  |
        RCC_CLOCKTYPE_PCLK2  |
        RCC_CLOCKTYPE_D3PCLK1|
        RCC_CLOCKTYPE_D1PCLK1;

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK; // SYSCLK = PLL1P = 200MHz
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;  // D1 domain = 200MHz
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;    // AHB = 100MHz
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;    // APB3 = 50MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;    // APB1 = 50MHz (UART/SPI)
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;    // APB2 = 50MHz (TIM/SPI)
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;    // APB4 = 50MHz

    // Flash latency 2WS phù hợp với VOS1 + 200MHz
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

    // --- USB clock: HSI48 ---
    // HSI48 đủ chính xác cho USB FS (±0.25% tolerance)
    // Không cần crystal ngoài, không cần PLL2/PLL3 cho USB
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_HSI48;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

// ============================================================
// targetPreInit()
// ============================================================
// Chạy rất sớm, trước khi INAV init bất kỳ peripheral nào.
// Dùng để set trạng thái ban đầu cho GPIO đặc biệt.
//
// SBUS_INV (PD0): kéo LOW khi boot để enable hardware inverter
// trên board — cần thiết để UART6 nhận SBUS đúng cực tính.
// INAV toggle pin này tự động khi set Serial RX = SBUS,
// nhưng set sớm ở đây đảm bảo trạng thái đúng ngay từ đầu.

void targetPreInit(void)
{
    // Enable clock cho GPIOD nếu chưa được bật
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = GPIO_PIN_0;          // PD0 = SBUS_INV
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &gpio);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET); // LOW = inverter enabled
}

// ============================================================
// TIMER MAP
// ============================================================
timerHardware_t timerHardware[] = {
    DEF_TIM(TIM1, CH4, PE14, TIM_USE_OUTPUT_AUTO, 0, 0), // M1
    DEF_TIM(TIM1, CH3, PE13, TIM_USE_OUTPUT_AUTO, 0, 0), // M2
    DEF_TIM(TIM1, CH2, PE11, TIM_USE_OUTPUT_AUTO, 0, 0), // M3
    DEF_TIM(TIM1, CH1, PE9,  TIM_USE_OUTPUT_AUTO, 0, 0), // M4

    DEF_TIM(TIM3, CH4, PB1,  TIM_USE_OUTPUT_AUTO, 0, 0), // M5
    DEF_TIM(TIM3, CH3, PB0,  TIM_USE_OUTPUT_AUTO, 0, 0), // M6

    DEF_TIM(TIM4, CH1, PD12, TIM_USE_OUTPUT_AUTO, 0, 0), // M7
    DEF_TIM(TIM4, CH2, PD13, TIM_USE_OUTPUT_AUTO, 0, 0), // M8
    DEF_TIM(TIM4, CH3, PD14, TIM_USE_OUTPUT_AUTO, 0, 0), // M9
    DEF_TIM(TIM4, CH4, PD15, TIM_USE_OUTPUT_AUTO, 0, 0), // M10
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);