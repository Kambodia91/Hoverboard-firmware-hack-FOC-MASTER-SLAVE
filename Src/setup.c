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

/*
tim1 master, enable -> trgo
tim3, gated slave mode, trgo by tim1 trgo. overflow -> trgo
adc1,adc2 triggered by tim3 trgo
adc 1,2 dual mode

ADC1             ADC2
R_Blau PC4 CH14  R_Gelb PC5 CH15
L_Grün PA0 CH01  L_Blau PC3 CH13
R_DC PC1 CH11    L_DC PC0 CH10
BAT   PC2 CH12   L_TX PA2 CH02
BAT   PC2 CH12   L_RX PA3 CH03

pb10 usart1 dma1 channel2/3
*/

#include "defines.h"
#include "config.h"
#include "setup.h"

TIM_HandleTypeDef htim_Motor;
TIM_HandleTypeDef htim_Adc;
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart1;

I2C_HandleTypeDef hi2c2;

DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
volatile adc_buf_t adc_buffer;


#if defined(DEBUG_SERIAL_USART2) || defined(CONTROL_SERIAL_USART2) || defined(FEEDBACK_SERIAL_USART2) || defined(SIDEBOARD_SERIAL_USART2)
 /* USART2 init function */
 void UART2_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
  
  huart2.Instance = USART2;
  huart2.Init.BaudRate = USART2_BAUD;
  huart2.Init.WordLength = USART2_WORDLENGTH;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);
}
#endif

#if defined(DEBUG_SERIAL_USART1) || defined(CONTROL_SERIAL_USART1) || defined(FEEDBACK_SERIAL_USART1) || defined(SIDEBOARD_SERIAL_USART1)
/* USART1 init function */
void UART1_Init(void)
{
  /* Enable alternative pinout usart1 */
  __HAL_AFIO_REMAP_USART1_ENABLE();

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  
  huart1.Instance = USART1;
  huart1.Init.BaudRate = USART1_BAUD;
  huart1.Init.WordLength = USART1_WORDLENGTH;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
}
#endif

#if defined(DEBUG_SERIAL_USART2) || defined(CONTROL_SERIAL_USART2) || defined(FEEDBACK_SERIAL_USART2) || defined(SIDEBOARD_SERIAL_USART2) || \
    defined(DEBUG_SERIAL_USART1) || defined(CONTROL_SERIAL_USART1) || defined(FEEDBACK_SERIAL_USART1) || defined(SIDEBOARD_SERIAL_USART1)
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX 
    */
    
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_USART1_ENABLE();  // Change pinOut

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart1_rx);
    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart1_tx);
    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */
	__HAL_UART_ENABLE_IT (uartHandle, UART_IT_IDLE);  // Enable the USART IDLE line detection interrupt
  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart2_rx);
    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart2_tx);
    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */
	__HAL_UART_ENABLE_IT (uartHandle, UART_IT_IDLE);  // Enable the USART IDLE line detection interrupt
  /* USER CODE END USART2_MspInit 1 */
    
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 
#endif

DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

void I2C_Init(void)
{
  /* Initialise I2C2 GPIO pins
  *  I2C2 GPIO Configuration
  *  PB6     ------> I2C2_SCL
  *  PB7     ------> I2C2_SDA
  */
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Initialise I2C peripheral */
  __HAL_RCC_I2C2_CLK_ENABLE();
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 200000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  __HAL_RCC_I2C2_FORCE_RESET();
  __HAL_RCC_I2C2_RELEASE_RESET();
  HAL_I2C_Init(&hi2c2);

  /* Peripheral DMA init*/
/*  __HAL_RCC_DMA1_CLK_ENABLE();
*/  
  /* DMA1_Channel4_IRQn interrupt configuration */
/*  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 4);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
*/
  /* DMA1_Channel5_IRQn interrupt configuration */
/*  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 3);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
*/

/*  hdma_i2c2_rx.Instance = DMA1_Channel5;
  hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
  hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
  HAL_DMA_Init(&hdma_i2c2_rx);

  __HAL_LINKDMA(&hi2c2,hdmarx,hdma_i2c2_rx);
*/

/*  hdma_i2c2_tx.Instance = DMA1_Channel4;
  hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
  hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
  HAL_DMA_Init(&hdma_i2c2_tx);

  __HAL_LINKDMA(&hi2c2,hdmatx,hdma_i2c2_tx);
*/
  /* Peripheral interrupt init */
/*  HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
  HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
*/
}

void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
 
/* GPIO Remap */
  // __HAL_AFIO_REMAP_SWJ_NOJTAG();        // Disable = JNRST, JTDO, JTDI. Enable = PIN PB4, PB3, PA15.
  // __HAL_AFIO_REMAP_USART1_ENABLE();     // Change pinOut usart1
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;


  GPIO_InitStruct.Pin = MOTOR_HALL_U_PIN;
  HAL_GPIO_Init(MOTOR_HALL_U_PORT, &GPIO_InitStruct);   // HALL
  GPIO_InitStruct.Pin = MOTOR_HALL_V_PIN;
  HAL_GPIO_Init(MOTOR_HALL_V_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_HALL_W_PIN;
  HAL_GPIO_Init(MOTOR_HALL_W_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_BKIN_PIN;             // BKIN PIN
  HAL_GPIO_Init(MOTOR_TIM_BKIN_PORT, &GPIO_InitStruct); // BKIN PORT

  GPIO_InitStruct.Pull = GPIO_PULLUP;
  
  
  GPIO_InitStruct.Pin = CHARGER_PIN;                  // Charger
  HAL_GPIO_Init(CHARGER_PORT, &GPIO_InitStruct);
  

  GPIO_InitStruct.Pull = GPIO_NOPULL;


  GPIO_InitStruct.Pin = BUTTON_PIN;                   // Power Switch
  HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BUTTON1_PIN;                  // Button1 (closer to the speaker)
  HAL_GPIO_Init(BUTTON1_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BUTTON2_PIN;                  // Button2 
  HAL_GPIO_Init(BUTTON2_PORT, &GPIO_InitStruct);


  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;


  GPIO_InitStruct.Pin = LED_RED_PIN;                  // PB4
  HAL_GPIO_Init(LED_RED_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, RESET);
  GPIO_InitStruct.Pin = LED_GREEN_PIN;                // PB3
  HAL_GPIO_Init(LED_GREEN_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, RESET);
  #ifdef BOARD_SLAVE
  GPIO_InitStruct.Pin = LED_ORANGE_PIN;               // PA15
  HAL_GPIO_Init(LED_ORANGE_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(LED_ORANGE_PORT, LED_ORANGE_PIN, RESET);
  #endif  
  GPIO_InitStruct.Pin = LED_PIN;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
  #ifdef BOARD_MASTER
  GPIO_InitStruct.Pin = LED_PIN_1;
  HAL_GPIO_Init(LED_PORT_1, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = BUZZER_PIN;
  HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
  #endif
  GPIO_InitStruct.Pin = OFF_PIN;
  HAL_GPIO_Init(OFF_PORT, &GPIO_InitStruct);


  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;


  GPIO_InitStruct.Pin = MOTOR_DC_CUR_PIN;
  HAL_GPIO_Init(MOTOR_DC_CUR_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_U_CUR_PIN;
  HAL_GPIO_Init(MOTOR_U_CUR_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_V_CUR_PIN;
  HAL_GPIO_Init(MOTOR_V_CUR_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = DCLINK_PIN;
  HAL_GPIO_Init(DCLINK_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TEMP_PIN;
  HAL_GPIO_Init(MOTOR_TEMP_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;


  GPIO_InitStruct.Pin = MOTOR_TIM_UH_PIN;
  HAL_GPIO_Init(MOTOR_TIM_UH_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_VH_PIN;
  HAL_GPIO_Init(MOTOR_TIM_VH_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_WH_PIN;
  HAL_GPIO_Init(MOTOR_TIM_WH_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_UL_PIN;
  HAL_GPIO_Init(MOTOR_TIM_UL_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_VL_PIN;
  HAL_GPIO_Init(MOTOR_TIM_VL_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_WL_PIN;
  HAL_GPIO_Init(MOTOR_TIM_WL_PORT, &GPIO_InitStruct);
}

void MX_TIM_Init(void) {
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  TIM_SlaveConfigTypeDef sTimConfig;

  htim_Motor.Instance               = MOTOR_TIM;
  htim_Motor.Init.Prescaler         = 0;
  htim_Motor.Init.CounterMode       = TIM_COUNTERMODE_CENTERALIGNED1;
  htim_Motor.Init.Period            = 64000000 / 2 / PWM_FREQ;
  htim_Motor.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim_Motor.Init.RepetitionCounter = 0;
  htim_Motor.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&htim_Motor);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim_Motor, &sMasterConfig);

  sConfigOC.OCMode       = TIM_OCMODE_PWM1;
  sConfigOC.Pulse        = 0;
  sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_LOW;
  sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
  HAL_TIM_PWM_ConfigChannel(&htim_Motor, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim_Motor, &sConfigOC, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&htim_Motor, &sConfigOC, TIM_CHANNEL_3);

  sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_ENABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
  sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime         = DEAD_TIME;
  sBreakDeadTimeConfig.BreakState       = TIM_BREAK_DISABLE;            // PB12 EXTERNAL EMERGENCY STOP 
  sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW;        // PB12 Active Low
  sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&htim_Motor, &sBreakDeadTimeConfig);

  htim_Adc.Instance               = ADC_TIM;
  htim_Adc.Init.Prescaler         = 0;
  htim_Adc.Init.CounterMode       = TIM_COUNTERMODE_CENTERALIGNED1;
  htim_Adc.Init.Period            = 64000000 / 2 / PWM_FREQ;
  htim_Adc.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim_Adc.Init.RepetitionCounter = 0;
  htim_Adc.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&htim_Adc);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_ENABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim_Adc, &sMasterConfig);

  sTimConfig.InputTrigger = TIM_TS_ITR0;
  sTimConfig.SlaveMode    = TIM_SLAVEMODE_GATED;
  HAL_TIM_SlaveConfigSynchronization(&htim_Adc, &sTimConfig);

  // Start counting >0 to effectively offset timers by the time it takes for one ADC conversion to complete.
  // This method allows that the Phase currents ADC measurements are properly aligned with LOW-FET ON region for both motors
  ADC_TIM->CNT 		     = ADC_TOTAL_CONV_TIME;

  sConfigOC.OCMode       = TIM_OCMODE_PWM1;
  sConfigOC.Pulse        = 0;
  sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_LOW;
  sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
  HAL_TIM_PWM_ConfigChannel(&htim_Adc, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim_Adc, &sConfigOC, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&htim_Adc, &sConfigOC, TIM_CHANNEL_3);

  sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_ENABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
  sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime         = DEAD_TIME;
  sBreakDeadTimeConfig.BreakState       = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW;
  sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&htim_Adc, &sBreakDeadTimeConfig);

  ADC_TIM->BDTR &= ~TIM_BDTR_MOE;
  MOTOR_TIM->BDTR &= ~TIM_BDTR_MOE;

  HAL_TIM_PWM_Start(&htim_Adc, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim_Adc, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim_Adc, TIM_CHANNEL_3);
  HAL_TIMEx_PWMN_Start(&htim_Adc, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim_Adc, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim_Adc, TIM_CHANNEL_3);  

  HAL_TIM_PWM_Start(&htim_Motor, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim_Motor, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim_Motor, TIM_CHANNEL_3);
  HAL_TIMEx_PWMN_Start(&htim_Motor, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim_Motor, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim_Motor, TIM_CHANNEL_3);

  htim_Adc.Instance->RCR = 1;

  __HAL_TIM_ENABLE(&htim_Motor);
}

void MX_ADC1_Init(void) {
  ADC_MultiModeTypeDef multimode;
  ADC_ChannelConfTypeDef sConfig;
  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc1.Instance                   = ADC1;
  hadc1.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode    = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion       = 3;
  HAL_ADC_Init(&hadc1);
  

  /**Configure the ADC multi-mode*/
  multimode.Mode = ADC_DUALMODE_REGSIMULT;
  HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode);

  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.Channel = ADC_CHANNEL_5;                      // PA5 Current Master Batery
  sConfig.Rank    = 1;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  sConfig.Channel = ADC_CHANNEL_1;                      // PA1 BLDC Current Phase B
  sConfig.Rank    = 2;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  //temperature requires at least 17.1uS sampling time
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;             // Internal temp
  sConfig.Rank    = 3;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  hadc1.Instance->CR2 |= ADC_CR2_DMA | ADC_CR2_TSVREFE;

  __HAL_ADC_ENABLE(&hadc1);

  __HAL_RCC_DMA1_CLK_ENABLE();

  DMA1_Channel1->CCR   = 0;
  DMA1_Channel1->CNDTR = 3;
  DMA1_Channel1->CPAR  = (uint32_t) & (ADC1->DR);
  DMA1_Channel1->CMAR  = (uint32_t)&adc_buffer;
  DMA1_Channel1->CCR   = DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1 | DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_TCIE;
  DMA1_Channel1->CCR |= DMA_CCR_EN;

  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/* ADC2 init function */
void MX_ADC2_Init(void) {
  ADC_ChannelConfTypeDef sConfig;
  __HAL_RCC_ADC2_CLK_ENABLE();

  hadc2.Instance                   = ADC2;
  hadc2.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode    = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion       = 3;
  HAL_ADC_Init(&hadc2);

 
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.Channel = ADC_CHANNEL_4;                  // PA4 Voltage Batery
  sConfig.Rank    = 1;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  sConfig.Channel = ADC_CHANNEL_0;                  // PA0 BLDC Current Phase C
  sConfig.Rank    = 2;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  sConfig.Channel = ADC_CHANNEL_9;                  // PB1 White Wire from wheel
  sConfig.Rank    = 3;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  hadc2.Instance->CR2 |= ADC_CR2_DMA;
  __HAL_ADC_ENABLE(&hadc2);
}