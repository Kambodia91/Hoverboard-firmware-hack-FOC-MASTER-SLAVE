/*
* This file is part of the hoverboard-firmware-hack project.
*
* Copyright (C) 2017-2018 Rene Hopf <renehopf@mac.com>
* Copyright (C) 2017-2018 Nico Stute <crinq@crinq.de>
* Copyright (C) 2017-2018 Niklas Fauth <niklas.fauth@kit.fail>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Define to prevent recursive inclusion
#ifndef DEFINES_H
#define DEFINES_H

#include "stm32f1xx_hal.h"
#include "config.h"

#define MOTOR_HALL_U_PIN    GPIO_PIN_0                      // Hall Sensor A
#define MOTOR_HALL_V_PIN    GPIO_PIN_6                      // Hall Sensor B
#define MOTOR_HALL_W_PIN    GPIO_PIN_7                      // Hall Sensor C

#define MOTOR_HALL_U_PORT   GPIOB
#define MOTOR_HALL_V_PORT   GPIOA
#define MOTOR_HALL_W_PORT   GPIOA

// TIM3 FOR ADC

#define ADC_TIM            TIM3                            // TIM3

// TIM1 OUTPUT FOR MOTOR

#define MOTOR_TIM           TIM1                            // TIM1
#define MOTOR_TIM_U         CCR1                            // Phase 1 (Yellow)
#define MOTOR_TIM_UH_PIN    GPIO_PIN_8                      
#define MOTOR_TIM_UH_PORT   GPIOA
#define MOTOR_TIM_UL_PIN    GPIO_PIN_13
#define MOTOR_TIM_UL_PORT   GPIOB
#define MOTOR_TIM_V         CCR2                            // Phase 2 (Green)
#define MOTOR_TIM_VH_PIN    GPIO_PIN_9
#define MOTOR_TIM_VH_PORT   GPIOA
#define MOTOR_TIM_VL_PIN    GPIO_PIN_14
#define MOTOR_TIM_VL_PORT   GPIOB
#define MOTOR_TIM_W         CCR3                            // Phase 3 (Blue)
#define MOTOR_TIM_WH_PIN    GPIO_PIN_10
#define MOTOR_TIM_WH_PORT   GPIOA
#define MOTOR_TIM_WL_PIN    GPIO_PIN_15
#define MOTOR_TIM_WL_PORT   GPIOB

// TIM1 INPUT

#define MOTOR_TIM_BKIN_PIN  GPIO_PIN_12
#define MOTOR_TIM_BKIN_PORT GPIOB

// ADC INPUT

#define MOTOR_DC_CUR_PIN    GPIO_PIN_5                        // DC Current 
#define MOTOR_DC_CUR_PORT   GPIOA
#define MOTOR_U_CUR_PIN     GPIO_PIN_0                        // Phase B Current (Yellow Phase)
#define MOTOR_U_CUR_PORT    GPIOA
#define MOTOR_V_CUR_PIN     GPIO_PIN_1                        // Phase C Current (Green Phase)
#define MOTOR_V_CUR_PORT    GPIOA
#define MOTOR_TEMP_PIN      GPIO_PIN_1                        // White wire from wheel
#define MOTOR_TEMP_PORT     GPIOB
#define DCLINK_PIN          GPIO_PIN_4                        // V Bat
#define DCLINK_PORT         GPIOA

// OUTPUT 

#define LED_PIN             GPIO_PIN_5                        //  Output Mosfet LED SLOT_1
#define LED_PORT            GPIOB

#ifdef BOARD_MASTER
#define LED_PIN_1           GPIO_PIN_8                        //  Output Mosfet LED SLOT_2
#define LED_PORT_1          GPIOB
#endif

#define LED_RED_PIN         GPIO_PIN_4                        //  Output Mosfet LED RED
#define LED_RED_PORT        GPIOB

#define LED_GREEN_PIN       GPIO_PIN_3                        //  Output Mosfet LED GREEN
#define LED_GREEN_PORT      GPIOB

#ifdef BOARD_SLAVE
#define LED_ORANGE_PIN      GPIO_PIN_15                       //  Output Mosfet LED ORANGE
#define LED_ORANGE_PORT     GPIOA
#endif

#ifdef BOARD_MASTER
#define BUZZER_PIN          GPIO_PIN_9                        // Buzzer 
#define BUZZER_PORT         GPIOB
#endif

#define OFF_PIN             GPIO_PIN_2                        // Mosfet Latch
#define OFF_PORT            GPIOB                             // BOARD_SLAVE pin no connect

#define CHARGER_PIN         GPIO_PIN_15                       // Charger
#define CHARGER_PORT        GPIOC

#if defined(CONTROL_PPM_LEFT)
#define PPM_PIN             GPIO_PIN_3                        // Reciver pin USART2
#define PPM_PORT            GPIOA
#elif defined(CONTROL_PPM_RIGHT)
#define PPM_PIN             GPIO_PIN_7                        // Reciver pin USART1
#define PPM_PORT            GPIOB
#endif

#if defined(CONTROL_PWM_LEFT)
#define PWM_PIN_CH1         GPIO_PIN_2                        // Transmit pin USART2
#define PWM_PORT_CH1        GPIOA
#define PWM_PIN_CH2         GPIO_PIN_3                        // Reciver pin USART2
#define PWM_PORT_CH2        GPIOA
#elif defined(CONTROL_PWM_RIGHT)
#define PWM_PIN_CH1         GPIO_PIN_6                        // Transmit pin USART1
#define PWM_PORT_CH1        GPIOB
#define PWM_PIN_CH2         GPIO_PIN_7                        // Recived pin USART1
#define PWM_PORT_CH2        GPIOB
#endif
// INPUT BUTTON
#define BUTTON_PIN          GPIO_PIN_10                       // Power Button
#define BUTTON_PORT         GPIOB

#define BUTTON1_PIN         GPIO_PIN_11                       // Button_1
#define BUTTON1_PORT        GPIOA

#define BUTTON2_PIN         GPIO_PIN_13                       // Button_2
#define BUTTON2_PORT        GPIOC

// END

#define DELAY_TIM_FREQUENCY_US 1000000

#define MILLI_R (R * 1000)
#define MILLI_PSI (PSI * 1000)
#define MILLI_V (V * 1000)

#define NO 0
#define YES 1
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define LIMIT(x, lowhigh) (((x) > (lowhigh)) ? (lowhigh) : (((x) < (-lowhigh)) ? (-lowhigh) : (x)))
#define SAT(x, lowhigh) (((x) > (lowhigh)) ? (1.0f) : (((x) < (-lowhigh)) ? (-1.0f) : (0.0f)))
#define SAT2(x, low, high) (((x) > (high)) ? (1.0f) : (((x) < (low)) ? (-1.0f) : (0.0f)))
#define STEP(from, to, step) (((from) < (to)) ? (MIN((from) + (step), (to))) : (MAX((from) - (step), (to))))
#define DEG(a) ((a)*M_PI / 180.0f)
#define RAD(a) ((a)*180.0f / M_PI)
#define SIGN(a) (((a) < 0) ? (-1) : (((a) > 0) ? (1) : (0)))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define IN_RANGE(x, low, high) (((x) >= (low)) && ((x) <= (high)))
#define SCALE(value, high, max) MIN(MAX(((max) - (value)) / ((max) - (high)), 0.0f), 1.0f)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN3(a, b, c) MIN(a, MIN(b, c))
#define MAX3(a, b, c) MAX(a, MAX(b, c))
#define ARRAY_LEN(x) (uint32_t)(sizeof(x) / sizeof(*(x)))
#define MAP(x, in_min, in_max, out_min, out_max) (((((x) - (in_min)) * ((out_max) - (out_min))) / ((in_max) - (in_min))) + (out_min))

#if defined(PRINTF_FLOAT_SUPPORT) && (defined(DEBUG_SERIAL_USART2) || defined(DEBUG_SERIAL_USART1)) && defined(__GNUC__)
    asm(".global _printf_float");     // this is the magic trick for printf to support float. Warning: It will increase code considerably! Better to avoid!
#endif


typedef struct {
  uint16_t curDc; // ADC1
  uint16_t batt;  // ADC2
  uint16_t curB; // ADC1
  uint16_t curC; // ADC2
  uint16_t temp; // ADC1
  uint16_t AdcIn; // ADC2
} adc_buf_t;

typedef enum {
  NUNCHUK_CONNECTING,
  NUNCHUK_DISCONNECTED,
  NUNCHUK_RECONNECTING,
  NUNCHUK_CONNECTED
} nunchuk_state;

// Define I2C, Nunchuk, PPM, PWM functions
void I2C_Init(void);
nunchuk_state Nunchuk_Read(void);
void PPM_Init(void);
void PPM_ISR_Callback(void);
void PWM_Init(void);
void PWM_ISR_CH1_Callback(void);
void PWM_ISR_CH2_Callback(void);

// Leds definitions                                                           M   |   S   
#define LED1_SET            (0x01)  // Włącznik                             | L8  | L9  |
#define LED2_SET            (0x02)  // Enable Slave                         | L7  | L6  |
#define LED3_SET            (0x04)  // Enable Master                        | L4  | L5  |
#define LED4_SET            (0x08)  // Critical error Master                | L3  | L2  |
#define LED5_SET            (0x10)  // Critical error Slave                 |SWITCH L1  |

#define LED6_SET            (0x20)  // TimeoutFlgADC Slave
#define LED7_SET            (0x40)  // TimeoutFlgADC Master
#define LED8_SET            (0x80)  // timeoutFlgSerial Master
#define LED9_SET            (0x100) // timeoutFlgSerial Slave

#define SENSOR1_SET         (0x01)
#define SENSOR2_SET         (0x02)
#define SENSOR_MPU          (0x04)

// RC iBUS switch definitions. Flysky FS-i6S has [SWA, SWB, SWC, SWD] = [2, 3, 3, 2] positions switch
#define SWA_SET             (0x0100)   //  0000 0001 0000 0000
#define SWB_SET             (0x0600)   //  0000 0110 0000 0000
#define SWC_SET             (0x1800)   //  0001 1000 0000 0000
#define SWD_SET             (0x2000)   //  0010 0000 0000 0000

#endif // DEFINES_H