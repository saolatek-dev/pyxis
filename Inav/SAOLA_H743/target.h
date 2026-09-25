/*
 * This file is part of INAV.
 *
 * Copyright (C) 2026 Saolatek — SAOLA_H743 target, added 2026.
 *
 * INAV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * INAV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with INAV.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// =============================================
// SAOLA H743 - Custom Flight Controller
// STM32H743 | HSE 8 MHz | USB OTG FS
// =============================================

#define TARGET_BOARD_IDENTIFIER "SH74"
#define USBD_PRODUCT_STRING     "Saola H743"

// =============================================
// CLOCK
// The board has an EXTERNAL 8 MHz crystal (HSE).
// The HSE value is declared in CMakeLists.txt: HSE_MHZ 8
// (which is also INAV's STM32_DEFAULT_HSE_MHZ).
//
// INAV's standard clock path (SystemClockHSE_Config in
// target/system_stm32h7xx.c) works out:
//   PLL1M = HSE/1MHz/2 = 4      → VCI  = 8/4   = 2 MHz  (VCIRANGE_1)
//   PLL1N = 400 (Rev.Y) / 480 (Rev.V)
//                                → VCO  = 800 / 960 MHz
//   PLL1P = 2                    → sys_ck = 400 / 480 MHz
//   HCLK  = sys_ck/2 = 200 / 240 MHz
//
//   PLL2 (SDMMC): PLL2M = HSE/1.6MHz = 5 → VCI = 1.6 MHz
//                 N=500 → VCO = 800 MHz, R=4 → 200 MHz (required by the HAL SD driver)
//
// Do NOT define USE_SYSTEM_HSI — that is only for boards without a
// crystal, and would break the SDMMC/USB clocks on this board.
// USB runs from the dedicated HSI48 and does not depend on HSE.
// =============================================

// =============================================
// USB
// INAV H7: vbus_sensing_enable = DISABLE is hardcoded
// in usbd_conf_stm32h7xx.c → nothing else to define.
// USE_VCP is all that is needed.
// =============================================
#define USE_VCP

// =============================================
// LED
// =============================================
#define LED0                    PE5   // RED
#define LED1                    PE6   // GREEN
#define LED2                    PE4   // BLUE
// The board's LEDs are ACTIVE-HIGH (MCU pin -> 1k -> anode, cathode to GND;
// see Docs/pinout.md). INAV assumes active-low by default: ledSet() writes
// `on ? inverted : !inverted`, so without INVERTED "LED on" pulls the pin LOW.
// -> INVERTED is required so that "LED on" drives the pin HIGH.
#define LED0_INVERTED
#define LED1_INVERTED
#define LED2_INVERTED

// =============================================
// UART
// SERIAL_PORT_COUNT = VCP + 7 UARTs = 8
// =============================================
#define USE_UART1
#define UART1_TX_PIN            PA9
#define UART1_RX_PIN            PA10

#define USE_UART2
#define UART2_TX_PIN            PA2
#define UART2_RX_PIN            PA3

#define USE_UART3
#define UART3_TX_PIN            PD8
#define UART3_RX_PIN            PD9

#define USE_UART4
#define UART4_TX_PIN            PA0
#define UART4_RX_PIN            PA1

#define USE_UART6
#define UART6_TX_PIN            PC6
#define UART6_RX_PIN            PC7

#define USE_UART7
#define UART7_RX_PIN            PE7   // ESC telemetry RX only

#define USE_UART8
#define UART8_TX_PIN            PE1
#define UART8_RX_PIN            PE0

#define SERIAL_PORT_COUNT       8     // VCP, UART1-4, UART6-8

// NO hardware SBUS inverter: PD0 is not connected on the schematic.
// SBUS uses the H7 UART's built-in RX inversion: rx/sbus.c opens the port with
// SERIAL_INVERTED and serial_uart_hal.c sets UART_ADVFEATURE_RXINVERT.
// (INAV does not read SBUS_INV_PIN either — that is a Betaflight convention.)

// =============================================
// SPI
// =============================================
#define USE_SPI

#define USE_SPI_DEVICE_1
#define SPI1_SCK_PIN            PA5
#define SPI1_MISO_PIN           PA6
#define SPI1_MOSI_PIN           PA7

#define USE_SPI_DEVICE_2
#define SPI2_SCK_PIN            PD3
#define SPI2_MISO_PIN           PC2
#define SPI2_MOSI_PIN           PC3

// =============================================
// I2C
// INAV sets I2C speed with the I2C_SPEED_xxxKHZ enum,
// not a raw number → I2C_SPEED is not defined here and the
// driver uses its 400 kHz default.
// Running I2C2 (on-board) at 100 kHz would need a patch via i2cConfig;
// otherwise accept 400 kHz (DPS310/IST8310 both handle 400 kHz in practice).
// =============================================
#define USE_I2C

#define USE_I2C_DEVICE_1
#define I2C1_SCL_PIN            PB6
#define I2C1_SDA_PIN            PB7

#define USE_I2C_DEVICE_2
#define I2C2_SCL_PIN            PB10
#define I2C2_SDA_PIN            PB11

// =============================================
// IMU — DUAL IMU AUTODETECT (one firmware for both board versions)
//
// The board exists in two hardware versions:
//   - Version A: BMI088 fitted (Accel CS PD4 / Gyro CS PD5, SPI2)
//   - Version B: BMI270 only (CS PA15, SPI2)
//
// INAV probes sensors in the ORDER OF THE CASES in gyroDetect()/accDetect()
// (src/main/sensors/gyro.c, src/main/sensors/acceleration.c) when
// gyro_hardware / acc_hardware = AUTO (the default):
//     MPU6000 → MPU6500 → MPU9250 → BMI160 → BMI088 →
//     ICM20689 → ICM42605 → BMI270 → LSM6DXX → ICM45686
//
// → BMI088 comes BEFORE BMI270, so defining both is enough:
//     * BMI088 fitted → BMI088 is the main IMU.
//     * Not fitted    → the chip-ID read fails and detection falls through to BMI270.
//
// Each sensor carries ITS OWN alignment (passed through
// BUSDEV_REGISTER_SPI in target/common_hardware.c), so
// autodetect still applies the right mounting for each chip.
//
// DEFINE NAMES: BMI088 must use BMI088_ACC_CS_PIN
// (not ACCEL_CS_PIN) to match BUSDEV_REGISTER.
// =============================================
// ---- ALIGNMENT: taken from ArduPilot + PX4 (verified on hardware) ----
// ArduPilot hwdef/Saolah743/hwdef.dat:
//     IMU BMI088 ... ROTATION_ROLL_180_YAW_270   -> (x,y,z) = (-y,-x,-z)
//     IMU BMI270 ... ROTATION_ROLL_180           -> (x,y,z) = ( x,-y,-z)
// PX4 boards/saolah743/h743/init/rc.board_sensors:
//     bmi088 -R 6 (YAW_270) + driver flips y,z itself  == ROLL_180_YAW_270  (matches AP)
//     bmi270 -R 0 (NONE)    + driver flips y,z itself  == ROLL_180          (matches AP)
//
// ArduPilot/PX4 use the FRD body frame (x forward, y RIGHT, z DOWN).
// INAV/Betaflight use FLU (x forward, y LEFT, z UP).
// => the two frames differ by exactly one ROLL_180 -> (x,-y,-z).
//
// Conversion (INAV = ROLL_180 ∘ AP):
//     BMI088: ROLL_180 ∘ (-y,-x,-z) = (-y, x, z) = CW270_DEG
//     BMI270: ROLL_180 ∘ ( x,-y,-z) = ( x, y, z) = CW0_DEG
//
// Cross-check within INAV — the BMI088 targets for which ArduPilot
// also declares ROTATION_ROLL_180_YAW_270 all use CW270_DEG:
//     MICOAIR743AIO, MICOAIR743, MICOAIR743V2, MICOAIR405V2, CORVON743V1
// and the BMI270 target that ArduPilot declares ROTATION_ROLL_180 uses CW0_DEG:
//     MICOAIR405MINI
// Betaflight src/config/configs/CUST/SAOLAH743 also sets BMI270 = CW0_DEG.
//
// NOTE: BMI088 must use BMI088_ACC_CS_PIN (not ACCEL_CS_PIN)
// to match BUSDEV_REGISTER in target/common_hardware.c.
// ---------------------------------------------------------------------
#define USE_IMU_BMI088
#define IMU_BMI088_ALIGN        CW270_DEG
#define BMI088_SPI_BUS          BUS_SPI2
#define BMI088_GYRO_CS_PIN      PD5
#define BMI088_ACC_CS_PIN       PD4   // ← ACC, not ACCEL

#define USE_IMU_BMI270
#define IMU_BMI270_ALIGN        CW0_DEG
#define BMI270_SPI_BUS          BUS_SPI2
#define BMI270_CS_PIN           PA15

// SPI2 SPEED — keep INAV's standard map, do NOT override.
//
// Measured directly over SWD on a board that flies smoothly (fw bbc05536, GYRO=BMI270):
//     SPI2_CFG1.MBR = 2  ->  /8  ->  120 MHz / 8 = 15 MHz
// i.e. it runs the BMI270 at 15 MHz, gyro OK, zero-calibration converges, flies normally.
// SPI2 kernel clock = pll1_q_ck: read from RCC_PLLCKSELR/PLL1DIVR = HSE 8 / M4 x N480
// -> VCO 960, Q8 -> 120 MHz; RCC_D2CCIP1R.SPI123SEL = 0 (PLL1Q). Confirmed on
// hardware, not inferred.
//
// So SPI_SLOW_BUS_STANDARD_DIV16 and BMI088_{GYRO,ACC}_MAX_SPI_SPEED are NO
// LONGER defined — the driver uses the upstream defaults
// (gyro FAST, then acc STANDARD overrides the whole bus -> 15 MHz), exactly like MICOAIR743.
//
// !! EXCEPTION SEEN ONCE: on ONE other sample board, 15 MHz on the BMI270 gave a gyro
// stdev of ~31 deg/s and zero-calibration never converged (at 7.5 MHz:
// 0.16-0.26 deg/s). If that symptom comes back, uncomment exactly the one line
// below to lower the slow bus's STANDARD tier to DIV16 = 7.5 MHz:
//#define SPI_SLOW_BUS_STANDARD_DIV16

// =============================================
// BARO — DPS310 / DPS368 (I2C2, addr 0x77 or 0x76)
//
// INAV's barometer_dps310.c driver accepts REG_ID (0x0D) =
// 0x10 or 0x11, and the DPS368 reports the same REV_AND_PROD_ID = 0x10 as
// the DPS310 and shares its register set / calibration coefficients.
// → the SAME driver runs both DPS310 and DPS368,
//   no separate define needed.
// =============================================
#define USE_BARO
#define USE_BARO_DPS310
#define BARO_I2C_BUS            BUS_I2C2

// =============================================
// MAG
// =============================================
#define USE_MAG
#define USE_MAG_IST8310
#define USE_MAG_QMC5883
#define MAG_I2C_BUS             BUS_I2C2
#define MAG_IST8310_ALIGN       CW90_DEG
#define MAG_QMC5883_ALIGN       CW90_DEG

// =============================================
// OSD (AT7456E / MAX7456)
// =============================================
#define USE_MAX7456
#define MAX7456_SPI_BUS         BUS_SPI1
#define MAX7456_CS_PIN          PD11   // schematic: AT7456E SPI1 CS = PD11

// =============================================
// SD CARD (SDMMC1 4-bit)
// Standard order: USE_SDCARD → USE_SDCARD_SDIO → SDIODEV → 4BIT
// =============================================
#define USE_SDCARD
#define USE_SDCARD_SDIO
#define SDCARD_SDIO_DEVICE      SDIODEV_1
#define SDCARD_SDIO_4BIT

// =============================================
// ADC
// DEFINE NAMES: INAV does NOT read VBAT_ADC_PIN / CURRENT_METER_ADC_PIN — those
// are Betaflight conventions. INAV uses ADC_CHANNEL_<n>_PIN for the pins, then maps
// functions through VBAT_ADC_CHANNEL / CURRENT_METER_ADC_CHANNEL (see adc.c, config.c).
// With the Betaflight names, adc.c takes the #else branch -> disableChannelMapping(),
// config.c gets VBAT_ADC_CHANNEL = ADC_CHN_NONE -> no channel is enabled ->
// the ADC1 clock is never turned on (measured over SWD: every ADC1 and
// DMA2_Stream0 register reads 0x00000000) -> vbat stays at 0, Configurator shows no voltage.
// ADC_CHANNEL_1_INSTANCE defaults to ADC_INSTANCE, so it need not be declared.
// Pattern taken from MICOAIR743 (same PC0/PC1, same H743).
// =============================================
#define USE_ADC
#define ADC_INSTANCE                ADC1
#define ADC_CHANNEL_1_PIN           PC0
#define ADC_CHANNEL_2_PIN           PC1
#define VBAT_ADC_CHANNEL            ADC_CHN_1
#define CURRENT_METER_ADC_CHANNEL   ADC_CHN_2
// VBAT_SCALE = divider ratio x 100 (settings.yaml: "1100 = 11:1 divider (10k:1k) x 100").
// Do NOT use 2112 from HAL_BATT_VOLT_SCALE 21.12 in the old hwdef-pin notes —
// that number was never measured (the same notes say "measure to get the right SCALE")
// and is almost 2x too high: with 2112 a 4S pack reads 30.16 V.
// Keep the NOMINAL value of the 11:1 divider. One sample board measured 10.93:1
// (adcValues[VBAT] = 1761 counts for 15.51 V on a meter, i.e. scale 1093) —
// 0.6% off, within resistor tolerance, so that board's figure is not baked
// into the shared target. For better accuracy calibrate each board:
//   set vbat_scale = <old_scale * V_meter / V_shown_in_configurator>
#define VBAT_SCALE_DEFAULT          1100
#define CURRENT_METER_SCALE         4020

// =============================================
// MOTOR / DSHOT
// MOTOR_PIN defines are not used by INAV H7 →
// motor output goes entirely through timerHardware[] in target.c
//
// USE_DSHOT_BITBANG: INAV main branch does NOT have this feature
// (it is a Betaflight feature) → removed, it would break the build
//
// USE_DSHOT_DMAR: REMOVED. Measured over SWD on a board that flies smoothly:
//     TIM1_DCR      = 0x00000000   (no DMA burst configured)
//     DMA1_S0PAR    = 0x40010040   = &TIM1->CCR4   (not &TIM1->DMAR)
//     DMA1_S1PAR    = 0x4001003C   = &TIM1->CCR3
// -> it runs per-channel DMA DSHOT, the path MICOAIR743/743V2/743AIO use.
// DMAR changes the motor output code path completely (timer_impl_hal.c: burst via
// TIM1->DMAR, triggered by the first channel's CC) and no reference board with
// the same pinout runs it. Removed to match the smooth-flying build exactly.
//
// Consequence: M10 (TIM4_CH4/PD15) loses DSHOT — the H743 has no DMA_REQUEST_TIM4_CH4,
// so DEF_TIM_DMA__BTCH_TIM4_CH4 = NONE. M10 still runs plain PWM. A quad frame only
// uses M1-M4 (TIM1), so it is unaffected.
// =============================================
#define USE_DSHOT
#define MAX_PWM_OUTPUT_PORTS    10   // M1-M10 (TIM1×4 + TIM3×2 + TIM4×4)

// =============================================
// CAN
// =============================================
#define USE_CAN
#define CAN1_RX_PIN             PB8
#define CAN1_TX_PIN             PB9

// =============================================
// ESC TELEMETRY
// =============================================
// NOTE: ESC_SENSOR_UART is NOT a define INAV reads (it is a Betaflight convention) —
// nothing outside target/ uses it. Kept only to document the wiring; to assign
// it for real, set FUNCTION_ESCSERIAL on UART7 in the Ports tab.
#define USE_ESC_SENSOR
#define ESC_SENSOR_UART         SERIAL_PORT_UART7

// Present in the firmware of the smooth-flying board (the string "BLHeli" appears
// in its flash dump); earlier versions of this target lacked it. MICOAIR743 declares it too.
#define USE_SERIAL_4WAY_BLHELI_INTERFACE

// =============================================
// RX INPUT
// =============================================
// KEEP UART6 + SBUS — this is the board's designed RC_INPUT pad
// (hwdef: "USART6 -> RC_INPUT (PC6 TX / PC7 RX) + PD0 SBUS_INV").
//
// REMAINING DIFFERENCE from the smooth-flying board, deliberately not synced: that
// one runs `serial 1 64` (index 1 = USART2) + `serialrx_provider = CRSF`, i.e. the
// receiver is on UART2 (PA2/PA3, the AIR UNIT connector) without the inverter. The two
// boards therefore take different RC paths — keep that in mind when comparing flight feel.
//
// If this board also uses a CRSF receiver: change the line below to SERIALRX_CRSF
// (CRSF is not inverted; the CRSF driver opens the UART non-inverted by itself),
// or just `set serialrx_provider = CRSF` in the CLI — this default only applies
// when the config is reset.
#define DEFAULT_RX_TYPE         RX_TYPE_SERIAL
#define SERIALRX_PROVIDER       SERIALRX_SBUS
#define SERIALRX_UART           SERIAL_PORT_USART6

// =============================================
// EZ TUNE — no longer blocked (reverted 2026-09-14)
//
// An earlier version defined EZ_TUNE_APPLY_DISABLED, believing EZ Tune caused jerky flight.
// WRONG. `diff all` read directly from the SMOOTH-FLYING FC (fw bbc05536) shows it runs
// EZ Tune ENABLED, with exactly the values that were blamed:
//     ez_enabled = ON, ez_response/damping/stability/aggressiveness/rate/expo
//       = 92/108/110/80/134/118, ez_filter_hz = 110
//     -> mc_i_roll 82, mc_i_pitch 90, mc_p_yaw 43, mc_i_yaw 84,
//        mc_cd 80/88/90, rate 70/70/60, rc_yaw_expo 80,
//        gyro_main_lpf_hz 110, dterm_lpf_hz 105, smith_predictor_delay 1.447,
//        dynamic_gyro_notch q250/min73/3D, setpoint_kalman_q 200
//
// Blocking EZ Tune is WORSE: the craft-type preset still writes antigravity=2,
// tpa 20@1200, d_boost 1/1 straight into EEPROM, but PID/filters fall back to defaults
// (gyro_main_lpf_hz = 60, no smith predictor, notch q 120) -> an untested mix.
// Let EZ Tune run normally, as on the smooth-flying build.
// ==============================================

// =============================================
// DEFAULT FEATURES
// =============================================
// FEATURE_PWM_OUTPUT_ENABLE: by default INAV DISABLES motor/servo output as a safety
// layer; it has to be ticked in Configurator before ESCs get a signal. Enabled here on
// request -> the board outputs from the first boot, remove props before plugging in a battery.
// FEATURE_AIRMODE: enabled on the smooth-flying board (`feature` lists ... BLACKBOX AIRMODE
// PWM_OUTPUT_ENABLE OSD) while its `diff` only shows PWM_OUTPUT_ENABLE ->
// AIRMODE is in that build's DEFAULT_FEATURES. Earlier versions of this target lacked it.
// Without airmode a quad loses control authority at low throttle (mixer clipping),
// felt as sagging/jerking when entering or leaving hover.
#define DEFAULT_FEATURES        (FEATURE_OSD | FEATURE_TELEMETRY | FEATURE_CURRENT_METER | FEATURE_VBAT | FEATURE_BLACKBOX | FEATURE_AIRMODE | FEATURE_PWM_OUTPUT_ENABLE)
// NOTE: "DEFAULT_BLACKBOX_DEVICE" is NOT a define the target controls — blackbox.c
// re-#defines it from ENABLE_BLACKBOX_LOGGING_ON_*_BY_DEFAULT, and that definition
// comes AFTER target.h is included, so it overrides this value (no error, just a
// "redefined" warning that is easy to miss) → sdcard_init() would never be called.
// Use the define blackbox.c actually checks:
#define ENABLE_BLACKBOX_LOGGING_ON_SDCARD_BY_DEFAULT

// =============================================
// USE_HAL_DRIVER: defined globally by cmake/stm32h7.cmake
// USE_TIMER_MGMT: does not exist in the INAV source
// → neither is needed in target.h
// =============================================

// =============================================
// GPIO PORT ENABLE — REQUIRED
// io_def_generated.h uses these defines to know which pins
// are valid on the target. Without them every IO_TAG(...)
// fails with 'defio_error_Pxx_is_not_supported_on_TARGET'.
//
// Bitmask: 1 bit = 1 pin (bit0=pin0, bit15=pin15)
// 0xffff = all 16 pins of the port are valid
//
// Ports used on SAOLA_H743:
//   PA: PA0-PA3, PA5-PA10, PA15  (UART1/2/4, SPI1, BMI270_CS)
//   PB: PB0-PB1, PB6-PB11        (TIM3, I2C1, CAN)
//   PC: PC0-PC3, PC6-PC7         (ADC, SPI2, UART6)
//   PD: PD3-PD5, PD8-PD9, PD11-PD15  (SPI2, UART3, OSD_CS, TIM4)
//   PE: PE0-PE1, PE4-PE7, PE9, PE11, PE13-PE14  (UART7/8, LED, TIM1)
// =============================================
#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         0xffff
#define TARGET_IO_PORTD         0xffff
#define TARGET_IO_PORTE         0xffff
