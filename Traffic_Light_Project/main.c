/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t ledModes[2] = {1, 2};
uint16_t cnt_mode = 0;

uint8_t green_period = 5;
uint8_t yellow_period = 3;
uint8_t red_period = 5;

char rxByte = 0;
uint8_t rx_buff[14] = {0};
uint8_t rx_buff_idx = 0;
char array[30] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
/* delay in second : second < 100*/
void delay(uint8_t second) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < (second * 100));
}

/* change mode when press button*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == change_mode_button_Pin) {
			cnt_mode = (cnt_mode + 1) % 2;

			uint32_t pre = HAL_GetTick();
			HAL_Delay(50);
			while (HAL_GPIO_ReadPin(change_mode_button_GPIO_Port, change_mode_button_Pin) == 0) {
				if ((HAL_GetTick() - pre) > 2000)
					break;
			}
			HAL_Delay(50);
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(rxByte != '\r') {
		rx_buff[rx_buff_idx++] = rxByte;
		if(rx_buff_idx == 14) {
			rx_buff_idx = 0;
		}
	} else {
		/* Config time */
		if(!strncmp(rx_buff, (char*)"CFGTIME", 7)) {
		    uint8_t greenTimeStr[3] = {rx_buff[7], rx_buff[8], '\0'};
		    uint8_t yellowTimeStr[3] = {rx_buff[9], rx_buff[10], '\0'};
		    uint8_t redTimeStr[3] = {rx_buff[11], rx_buff[12], '\0'};

		    green_period = atoi((char*)greenTimeStr);
		    yellow_period = atoi((char*)yellowTimeStr);
		    red_period = atoi((char*)redTimeStr);

		    if(green_period > 4 && green_period < 46 &&
		    	yellow_period > 2 && yellow_period < 6 &&
				red_period > 4 && green_period < 91) {
		    	HAL_UART_Transmit(&huart1, (uint8_t *) "CFGTIME OK\r\n", 12, 1000);
		    } else {
		    	HAL_UART_Transmit(&huart1, (uint8_t *) "CFGTIME FAIL\r\n", 14, 1000);
		    }
		}
		/* Config mode*/
		else if (!strncmp(rx_buff, (char*)"CFGMODE", 7)) {
			uint8_t cnt_mode_str[2] = {rx_buff[7], '\0'};
		    cnt_mode = atoi((char*)cnt_mode_str) - 1;

		    if(cnt_mode == 0 || cnt_mode == 1) {
		    	sprintf(array, "MODE%i SELECTED\r\n", cnt_mode + 1);
		    	HAL_UART_Transmit(&huart1, (uint8_t *) array, 16, 1000);
		    } else {
		    	HAL_UART_Transmit(&huart1, (uint8_t *) "CFGMODE FAIL\r\n", 14, 1000);
		    }
		}
		for(uint8_t i = 0; i < rx_buff_idx; i++) {
			rx_buff[i] = 0;
		}
		rx_buff_idx = 0;
	}
	HAL_UART_Receive_IT(&huart1, &rxByte, 1);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  HAL_UART_Transmit(&huart1, (uint8_t *) "OK\r\n", 4, 1000);
  HAL_UART_Receive_IT(&huart1, &rxByte, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	switch (ledModes[cnt_mode]) {
	case 1:
		HAL_GPIO_TogglePin(Yellow_GPIO_Port, Yellow_Pin);
		HAL_GPIO_WritePin(Green_GPIO_Port, Green_Pin, 0);
		HAL_GPIO_WritePin(Red_GPIO_Port, Red_Pin, 0);
		delay(1);
		break;
	case 2:
		HAL_GPIO_WritePin(Green_GPIO_Port, Green_Pin, 1);
		HAL_GPIO_WritePin(Yellow_GPIO_Port, Yellow_Pin, 0);
		HAL_GPIO_WritePin(Red_GPIO_Port, Red_Pin, 0);
		delay(green_period);

		HAL_GPIO_WritePin(Green_GPIO_Port, Green_Pin, 0);
		HAL_GPIO_WritePin(Yellow_GPIO_Port, Yellow_Pin, 1);
		HAL_GPIO_WritePin(Red_GPIO_Port, Red_Pin, 0);
		delay(yellow_period);

		HAL_GPIO_WritePin(Green_GPIO_Port, Green_Pin, 0);
		HAL_GPIO_WritePin(Yellow_GPIO_Port, Yellow_Pin, 0);
		HAL_GPIO_WritePin(Red_GPIO_Port, Red_Pin, 1);
		delay(red_period);
		break;
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 59999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 9999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Green_Pin|Yellow_Pin|Red_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : change_mode_button_Pin */
  GPIO_InitStruct.Pin = change_mode_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(change_mode_button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Green_Pin Yellow_Pin Red_Pin */
  GPIO_InitStruct.Pin = Green_Pin|Yellow_Pin|Red_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
