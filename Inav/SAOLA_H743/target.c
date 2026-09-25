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

/*
 * target.c — SAOLA_H743
 *
 * Verified against INAV main branch (2025):
 *   - dmavar pattern: increases continuously 0→7 across all timers
 *     (does not reset to 0 when moving to another timer — a reset = conflict!)
 *   - TIM4_CH4 (PD15): the H743 has NO DMA_REQUEST_TIM4_CH4
 *     → DEF_TIM_DMA__BTCH_TIM4_CH4 = NONE when USE_DSHOT_DMAR is not enabled
 *     → M10 runs plain PWM, not DSHOT. Accepted: the smooth-flying board does not
 *       use DMAR either (measured over SWD: TIM1_DCR=0, DMA1_S0PAR=&TIM1->CCR4), and a
 *       quad frame only uses M1-M4 on TIM1.
 *   - BUSDEV_REGISTER_SPI_TAG: BMI088 needs 2 separate entries (GYRO + ACC),
 *     BMI270 needs only 1
 *   - Reference: MICOAIR743AIO (same TIM1/TIM3/TIM4 pin layout)
 *                KAKUTEH7WING  (same BMI088 dual CS)
 *                AEDROXH7      (same TIM1 CH1-4 on PE9/11/13/14)
 */

#include <stdint.h>

#include "platform.h"

#include "drivers/bus.h"
#include "drivers/io.h"
#include "drivers/pwm_mapping.h"
#include "drivers/timer.h"
#include "drivers/pinio.h"
#include "drivers/sensor.h"

// ============================================================
// IMU DEVICE REGISTRATION
// ============================================================
//
// BMI088: accel and gyro are 2 separate chips on the same SPI2 bus
//   - DEVHW_BMI088_GYRO → CS = GYRO_CS  (PD5)
//   - DEVHW_BMI088_ACC  → CS = ACCEL_CS (PD4)
//   - tagIndex: 0 and 1 (distinguishes the 2 instances)
//   - Define names MUST match target.h: BMI088_SPI_BUS, BMI088_GYRO_CS_PIN, BMI088_ACC_CS_PIN
//
// BMI270: single chip, 1 CS pin
//   - tagIndex: 0

// IMU device registration is handled automatically by common_hardware.c
// when USE_IMU_BMI088 and USE_IMU_BMI270 are defined in target.h.
// Do NOT register them again here → it causes a "multiple definition" linker error.

// ============================================================
// TIMER MAP — 10 motor outputs
// ============================================================
//
// IMPORTANT — dmavar (last DEF_TIM parameter):
//   The H743 has a DMAMUX, so any stream can serve any request.
//   INAV uses dmavar as an INDEX into the DMA stream table (0..15):
//     [0..7]  = DMA1_ST0..DMA1_ST7
//     [8..15] = DMA2_ST0..DMA2_ST7
//
//   RULE: every motor channel needs a DIFFERENT dmavar.
//   If 2 channels share a dmavar → only 1 gets DSHOT,
//   the other stays silent (this was the bug in the old version).
//
//   Correct pattern: increase continuously 0→7 across all timers,
//   do NOT reset to 0 when moving to a new timer.
//   See: MICOAIR743AIO (same pin layout), AEDROXH7 (same TIM1 pins)
//
// NOTE ON TIM4_CH4 (PD15 — M10):
//   The STM32H743 has NO DMA_REQUEST_TIM4_CH4 (silicon limitation), so
//   timer_def_stm32h7xx.h sets DEF_TIM_DMA__BTCH_TIM4_CH4 = NONE without
//   USE_DSHOT_DMAR. This target does NOT enable DMAR (see target.h) → M10 = DMA_NONE,
//   plain PWM only. The dmavar on the M10 line therefore has no effect.
//
//   DSHOT on M10 would need USE_DSHOT_DMAR (INAV uses TIM4_UP burst),
//   but that switches the motor output code path of ALL THREE timers to burst DMA —
//   different from the firmware that flies smoothly. Not worth the trade.

timerHardware_t timerHardware[] = {
    // --- TIM1: M1-M4 on PE14/PE13/PE11/PE9 (AF1) ---
    // dmavar 0..3 → DMA1_ST0..DMA1_ST3
    DEF_TIM(TIM1, CH4, PE14, TIM_USE_OUTPUT_AUTO, 0, 0), // M1  BIDIR  DMA1_ST0
    DEF_TIM(TIM1, CH3, PE13, TIM_USE_OUTPUT_AUTO, 0, 1), // M2         DMA1_ST1
    DEF_TIM(TIM1, CH2, PE11, TIM_USE_OUTPUT_AUTO, 0, 2), // M3  BIDIR  DMA1_ST2
    DEF_TIM(TIM1, CH1, PE9,  TIM_USE_OUTPUT_AUTO, 0, 3), // M4         DMA1_ST3

    // --- TIM3: M5-M6 on PB1/PB0 (AF2) ---
    // dmavar 4..5 → DMA1_ST4..DMA1_ST5 (continuing, no reset)
    DEF_TIM(TIM3, CH4, PB1,  TIM_USE_OUTPUT_AUTO, 0, 4), // M5  BIDIR  DMA1_ST4
    DEF_TIM(TIM3, CH3, PB0,  TIM_USE_OUTPUT_AUTO, 0, 5), // M6         DMA1_ST5

    // --- TIM4: M7-M10 on PD12/PD13/PD14/PD15 (AF2) ---
    // dmavar 6..7 → DMA1_ST6..DMA1_ST7
    // dmavar 9    → DMA2_ST1 (avoids a conflict with DMA1)
    // M10 (TIM4_CH4): no DMA_REQUEST of its own → uses dmavar=0,
    //   which is DMA_NONE without USE_DSHOT_DMAR
    DEF_TIM(TIM4, CH1, PD12, TIM_USE_OUTPUT_AUTO, 0, 6), // M7         DMA1_ST6
    DEF_TIM(TIM4, CH2, PD13, TIM_USE_OUTPUT_AUTO, 0, 7), // M8         DMA1_ST7
    DEF_TIM(TIM4, CH3, PD14, TIM_USE_OUTPUT_AUTO, 0, 9), // M9         DMA2_ST1
    DEF_TIM(TIM4, CH4, PD15, TIM_USE_OUTPUT_AUTO, 0, 0), // M10        DMA_NONE (PWM only)
};

const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);
