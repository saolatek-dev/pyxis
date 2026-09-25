/*
 * This file is part of Betaflight.
 *
 * Copyright (C) 2026 Saolatek — SAOLAH743_BMI270 board config, added 2026.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define FC_TARGET_MCU   STM32H743

#define BOARD_NAME      SAOLAH743_BMI270
#define MANUFACTURER_ID CUST

#define SYSTEM_HSE_MHZ  8        // external 8MHz crystal (HSE), confirmed on hardware

#define USE_ACC
#define USE_GYRO
#define USE_ACCGYRO_BMI270
// PCB layout does not route BMI270 INT1/INT2 to the MCU (no EXTI available).
// Force the driver to poll the STATUS register over SPI instead of relying
// on the data-ready interrupt.
#define BMI270_POLLING_STRICT 1
#define USE_BARO
#define USE_BARO_DPS310
#define USE_SDCARD
#define USE_MAX7456

#ifndef USE_MAG
#define USE_MAG
#define USE_MAG_IST8310
#define USE_MAG_QMC5883
#endif

// Betaflight's motor config only wires up MOTOR1_PIN..MOTOR8_PIN at compile
// time. M9/M10 (PD14/PD15, TIM4_CH3/CH4) are DMA-mapped below so they are
// ready to use, but must be assigned at runtime:
//   resource motor 9 D14
//   resource motor 10 D15
//   save
#define MAX_SUPPORTED_MOTORS 10

#define MOTOR1_PIN PE14         // TIM1_CH4 - BIDIR capable
#define MOTOR2_PIN PE13         // TIM1_CH3
#define MOTOR3_PIN PE11         // TIM1_CH2 - BIDIR capable
#define MOTOR4_PIN PE9          // TIM1_CH1
#define MOTOR5_PIN PB1          // TIM3_CH4 - BIDIR capable
#define MOTOR6_PIN PB0          // TIM3_CH3
#define MOTOR7_PIN PD12         // TIM4_CH1
#define MOTOR8_PIN PD13         // TIM4_CH2
#define MOTOR9_PIN PD14         // TIM4_CH3 (assign via `resource`, see above)
#define MOTOR10_PIN PD15        // TIM4_CH4 (assign via `resource`, see above)

#define UART1_TX_PIN PA9
#define UART2_TX_PIN PA2
#define UART3_TX_PIN PD8
#define UART4_TX_PIN PA0
#define UART6_TX_PIN PC6
#define UART7_TX_PIN PE8
#define UART8_TX_PIN PE1
#define UART1_RX_PIN PA10
#define UART2_RX_PIN PA3
#define UART3_RX_PIN PD9
#define UART4_RX_PIN PA1
#define UART6_RX_PIN PC7
#define UART7_RX_PIN PE7
#define UART8_RX_PIN PE0

#define INVERTER_PIN_UART6 PD0  // SBUS hardware inverter control

#define I2C1_SCL_PIN PB6        // external connector
#define I2C2_SCL_PIN PB10       // onboard baro/mag
#define I2C1_SDA_PIN PB7
#define I2C2_SDA_PIN PB11

#define SPI1_SCK_PIN PA5        // OSD
#define SPI2_SCK_PIN PD3        // IMU
#define SPI1_SDI_PIN PA6
#define SPI2_SDI_PIN PC2
#define SPI1_SDO_PIN PA7
#define SPI2_SDO_PIN PC3

#define SDIO_CK_PIN  PC12
#define SDIO_CMD_PIN PD2
#define SDIO_D0_PIN  PC8
#define SDIO_D1_PIN  PC9
#define SDIO_D2_PIN  PC10
#define SDIO_D3_PIN  PC11

#define ADC_VBAT_PIN PC0
#define ADC_CURR_PIN PC1

#define LED0_PIN PE4             // Blue
#define LED1_PIN PE6             // Green
#define LED2_PIN PE5             // Red
#define LED0_INVERTED
#define LED1_INVERTED
#define LED2_INVERTED

#define GYRO_1_CS_PIN      PA15
#define MAX7456_SPI_CS_PIN PB12

// BMI270-only variant: SPI2 carries only the BMI270 (PA15).
// No PINIO is declared for PD4/PD5 - on this board they are not BMI088 chip
// selects, so they keep their default state and remain free for other uses.

#define TIMER_PIN_MAPPING \
    TIMER_PIN_MAP( 0, MOTOR1_PIN,  1,  0 ) \
    TIMER_PIN_MAP( 1, MOTOR2_PIN,  1,  1 ) \
    TIMER_PIN_MAP( 2, MOTOR3_PIN,  1,  2 ) \
    TIMER_PIN_MAP( 3, MOTOR4_PIN,  1,  3 ) \
    TIMER_PIN_MAP( 4, MOTOR5_PIN,  2,  4 ) \
    TIMER_PIN_MAP( 5, MOTOR6_PIN,  2,  5 ) \
    TIMER_PIN_MAP( 6, MOTOR7_PIN,  1,  6 ) \
    TIMER_PIN_MAP( 7, MOTOR8_PIN,  1,  7 ) \
    TIMER_PIN_MAP( 8, MOTOR9_PIN,  1, 10 ) \
    TIMER_PIN_MAP( 9, MOTOR10_PIN, 1, 11 )

#define ADC1_DMA_OPT   8
#define TIMUP1_DMA_OPT 0
#define TIMUP3_DMA_OPT 0
#define TIMUP4_DMA_OPT 0

#define GYRO_1_SPI_INSTANCE   SPI2
#define MAX7456_SPI_INSTANCE  SPI1
#define SDIO_USE_4BIT         1
#define SDIO_DEVICE           SDIODEV_1
#define BARO_I2C_INSTANCE     I2CDEV_2
#define MAG_I2C_INSTANCE      I2CDEV_2
#define MAG_I2C_ADDRESS       14
// Gyro (BMI270) assumed mounted with no rotation (CW0_DEG, matches the
// nearest hardware-identical reference board). Mag assumed rotated 90 deg
// yaw relative to the gyro, same as that reference board. Verify both
// against the actual silkscreen / Configurator before flying.
#define MAG_ALIGN             CW90_DEG
#define MAG_ALIGN_YAW         900

#define DEFAULT_BLACKBOX_DEVICE      BLACKBOX_DEVICE_SDCARD
#define DEFAULT_CURRENT_METER_SOURCE CURRENT_METER_ADC
#define DEFAULT_VOLTAGE_METER_SOURCE VOLTAGE_METER_ADC
#define DEFAULT_CURRENT_METER_SCALE  402
#define DEFAULT_VOLTAGE_METER_SCALE  213

#define MSP_UART             SERIAL_PORT_USART1  // TELEM1
#define MSP_DISPLAYPORT_UART SERIAL_PORT_USART2  // DJI O3
#define GPS_UART             SERIAL_PORT_USART3  // GPS
#define SERIALRX_UART        SERIAL_PORT_USART6  // RC INPUT
#define ESC_SENSOR_UART      SERIAL_PORT_USART7  // ESC telemetry (RX only)
