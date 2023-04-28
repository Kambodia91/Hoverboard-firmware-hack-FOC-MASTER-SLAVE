/*
* This file implements FOC motor control.
* This control method offers superior performanace
* compared to previous cummutation method. The new method features:
* ► reduced noise and vibrations
* ► smooth torque output
* ► improved motor efficiency -> lower energy consumption
*
* Copyright (C) 2019-2020 Emanuel FERU <aerdronix@gmail.com>
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

#include "stm32f1xx_hal.h"
#include "defines.h"
#include "setup.h"
#include "config.h"
#include "util.h"

// Matlab includes and defines - from auto-code generation
// ###############################################################################
#include "BLDC_controller.h"           /* Model's header file */
#include "rtwtypes.h"

extern RT_MODEL *const rtM_Motor;

extern DW             rtDW_Motor;                 /* Observable states */
extern ExtU           rtU_Motor;                  /* External inputs */
extern ExtY           rtY_Motor;                  /* External outputs */
extern P              rtP_Motor;                  /* Block parameters (auto storage) */

// ###############################################################################

static int16_t        pwm_margin;                 /* This margin allows to have a window in the PWM signal for proper FOC Phase currents measurement */

extern uint8_t        ctrlModReq;
static int16_t        curDC_max       = (I_DC_MAX * A2BIT_CONV);
int16_t               cur_phaB        = 0; 
int16_t               cur_phaC        = 0;
int16_t               cur_DC          = 0;
volatile int          pwm             = 0;

extern volatile adc_buf_t adc_buffer;             // adc.

uint8_t               buzzerFreq      = 0;
uint8_t               buzzerPattern   = 0;
uint8_t               buzzerCount     = 0;
volatile uint32_t     buzzerTimer     = 0;
static uint8_t        buzzerPrev      = 0;
static uint8_t        buzzerIdx       = 0;

int16_t               errCode_Master;
int16_t               errCode_Slave;

uint8_t               enable          = 0;        // initially motors are disabled for SAFETY
uint16_t              enableMotors    = 0;        // message ennable from arduino (uart1)
uint16_t              enableFinMaster = 0;        // final permission to start the engine from Master
uint16_t              enableFinSlave  = 0;        // final permission to start the engine From Slave
int16_t               chargeStatus    = 0;        // Status connection charge.
static uint8_t        enableFin       = 0;        // final permission to start the engine

static const uint16_t pwm_res         = 64000000 / 2 / PWM_FREQ; // = 2000

static uint16_t       offsetcount     = 0;
static int16_t        offsetrB        = 2000;
static int16_t        offsetrC        = 2000;
static int16_t        offsetdc        = 2000;

int16_t               batVoltage      = (400 * BAT_CELLS * BAT_CALIB_ADC) / BAT_CALIB_REAL_VOLTAGE;
#ifdef BOARD_MASTER
static int32_t        batVoltageFixdt = (400 * BAT_CELLS * BAT_CALIB_ADC) / BAT_CALIB_REAL_VOLTAGE << 16;  // Fixed-point filter output initialized at 400 V*100/cell = 4 V/cell converted to fixed-point
#endif
// =================================
// DMA interrupt frequency =~ 16 kHz
// =================================
void DMA1_Channel1_IRQHandler(void) {

  DMA1->IFCR = DMA_IFCR_CTCIF1;
  // HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
  // HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

  if(offsetcount < 2000) {  // calibrate ADC offsets
    offsetcount++;
    offsetrB = (adc_buffer.curB + offsetrB) / 2;
    offsetrC = (adc_buffer.curC + offsetrC) / 2;
    offsetdc = (adc_buffer.curDc + offsetdc) / 2;
    return;
  }
#ifdef BOARD_MASTER
  if (buzzerTimer % 1000 == 0) {  // Filter battery voltage at a slower sampling rate
    filtLowPass32(adc_buffer.batt, BAT_FILT_COEF, &batVoltageFixdt);
    batVoltage = (int16_t)(batVoltageFixdt >> 16);  // convert fixed-point to integer
  }
#endif
  // Get Motor currents
  cur_phaB = (int16_t)(offsetrB - adc_buffer.curB);
  cur_phaC = (int16_t)(offsetrC - adc_buffer.curC);
  cur_DC   = (int16_t)(offsetdc - adc_buffer.curDc);

  // Disable PWM when current limit is reached (current chopping)
  // This is the Level 2 of current protection. The Level 1 should kick in first given by I_MOT_MAX
  if(ABS(cur_DC)  > curDC_max || enable == 0 || enableMotors == 0 || chargeStatus == 1) {
    ADC_TIM->BDTR &= ~TIM_BDTR_MOE;
    MOTOR_TIM->BDTR &= ~TIM_BDTR_MOE;
  } else {
    ADC_TIM->BDTR |= TIM_BDTR_MOE;
    MOTOR_TIM->BDTR |= TIM_BDTR_MOE;
  }

  // Create square wave for buzzer
  buzzerTimer++;
  if (buzzerFreq != 0 && (buzzerTimer / 5000) % (buzzerPattern + 1) == 0) {
    if (buzzerPrev == 0) {
      buzzerPrev = 1;
      if (++buzzerIdx > (buzzerCount + 2)) {    // pause 2 periods
        buzzerIdx = 1;
      }
    }
    if (buzzerTimer % buzzerFreq == 0 && (buzzerIdx <= buzzerCount || buzzerCount == 0)) {
      #ifdef BOARD_MASTER
      HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
      #endif
    }
  } else if (buzzerPrev) {
      #ifdef BOARD_MASTER
      HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
      #endif
      buzzerPrev = 0;
  }

  // Adjust pwm_margin depending on the selected Control Type
  if (rtP_Motor.z_ctrlTypSel == FOC_CTRL) {
    pwm_margin = 110;
  } else {
    pwm_margin = 0;
  }

  // ############################### MOTOR CONTROL ###############################

  int u, v, w;
  static boolean_T OverrunFlag = false;

  /* Check for overrun */
  if (OverrunFlag) {
    return;
  }
  OverrunFlag = true;

  /* Make sure to stop motor in case of an error */
  enableFin = enable && !errCode_Slave && !errCode_Master && enableMotors;
    #ifdef BOARD_MASTER
    enableFinMaster = enableFin;
    #endif
    #ifdef BOARD_SLAVE
    enableFinSlave = enableFin;
    #endif
  

  // ========================= MOTOR ===========================  
    // Get hall sensors values
    uint8_t hall_u = !(MOTOR_HALL_U_PORT->IDR & MOTOR_HALL_U_PIN);
    uint8_t hall_v = !(MOTOR_HALL_V_PORT->IDR & MOTOR_HALL_V_PIN);
    uint8_t hall_w = !(MOTOR_HALL_W_PORT->IDR & MOTOR_HALL_W_PIN);

    /* Set motor inputs here */
    rtU_Motor.b_motEna      = enableFin;
    rtU_Motor.z_ctrlModReq  = ctrlModReq;
    rtU_Motor.r_inpTgt      = pwm;
    rtU_Motor.b_hallA       = hall_u;
    rtU_Motor.b_hallB       = hall_v;
    rtU_Motor.b_hallC       = hall_w;
    rtU_Motor.i_phaAB       = cur_phaB;
    rtU_Motor.i_phaBC       = cur_phaC;
    rtU_Motor.i_DCLink      = cur_DC;
    // rtU_Motor.a_mechAngle   = ...; // Angle input in DEGREES [0,360] in fixdt(1,16,4) data type. If `angle` is float use `= (int16_t)floor(angle * 16.0F)` If `angle` is integer use `= (int16_t)(angle << 4)`

    /* Step the controller */
    #ifdef MOTOR_ENA
    BLDC_controller_step(rtM_Motor);
    #endif

    /* Get motor outputs here */
    u = rtY_Motor.DC_phaA;
    v = rtY_Motor.DC_phaB;
    w = rtY_Motor.DC_phaC;
    
    #ifdef BOARD_MASTER
    errCode_Master = rtY_Motor.z_errCode;
    #endif
    #ifdef BOARD_SLAVE
    errCode_Slave = rtY_Motor.z_errCode;
    #endif
 
 // motSpeedMotor = rtY_Motor.n_mot;
 // motAngleMotor = rtY_Motor.a_elecAngle;

    /* Apply commands */
    MOTOR_TIM->MOTOR_TIM_U  = (uint16_t)CLAMP(u + pwm_res / 2, pwm_margin, pwm_res-pwm_margin);
    MOTOR_TIM->MOTOR_TIM_V  = (uint16_t)CLAMP(v + pwm_res / 2, pwm_margin, pwm_res-pwm_margin);
    MOTOR_TIM->MOTOR_TIM_W  = (uint16_t)CLAMP(w + pwm_res / 2, pwm_margin, pwm_res-pwm_margin);
  // =================================================================

  /* Indicate task complete */
  OverrunFlag = false;
 
 // ###############################################################################

}
