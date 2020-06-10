/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* Define I2C Device Address as a Private Variable */
#define mpu9265Address 0xD0

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Define an Empty buffer to store Data Read/Write from/to I2C */
uint8_t i2cBuffer[8];

/* Define Variables to hold x,y,z values of Accel */
uint16_t ax, ay, az;
float X_accel, Y_accel, Z_accel;

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	// Since we don't know the I2C address for the device,
	// loop through all the addresses
	for (uint8_t i=0; i < 255; i++){
		// This function accepts inputs as:
		// (I2C_HandleTypeDef, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
		// Since, we want to check for all addresses i.e. i, for 1 trial each and for 10 ms
		if (HAL_I2C_IsDeviceReady(&hi2c1, i, 1, 10) == HAL_OK){
			// If the I2C address is found, toggle the LED and Break out of the loop
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
			break;
		}
	}
	
	
	/* I2C Data Write from Master to Slave */
	
	// a) Set Accelerometer Range (reg28)
	// Register Address on byte 0
	i2cBuffer[0] = 28;
	// Data to Write to Register on byte 1
	// Sets the Accelerometer range to +-4g
	i2cBuffer[1] = 0x08;
	// Transmit Data to Accel over I2C from Master
	// (HandleTypeDef, slave Address, Data, Number of Bytes, Timeout)
	// This command writes data 0x08 to register number 28 on the IMU sensor.
	HAL_I2C_Master_Transmit(&hi2c1, mpu9265Address, i2cBuffer, 2, 10);
	
	
	
	/* I2C Read Operation */
	
	// Request to Read Data from (reg28)
	// Register Address on byte 0
	i2cBuffer[0] = 28;
	// Transmit Data to Accel over I2C from Master
	// (HandleTypeDef, slave Address, Data, Number of Bytes, Timeout)
	// This command transmits which register we want to read from the Accelerometer
	HAL_I2C_Master_Transmit(&hi2c1, mpu9265Address, i2cBuffer, 1, 10);
	// Clear the first byte of i2cBuffer to store read data
	i2cBuffer[1] = 0x00;
	// Read Data back from the Accelerometer
	// (HandleTypeDef, slave Address, buffer into which you want to write data after reading, number of bytes to read, timeout)
	HAL_I2C_Master_Receive(&hi2c1, mpu9265Address, &i2cBuffer[1], 1, 10);
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		/* Continuously read x,y,z data from the Accelerometer */
		// Request to Read Data from (reg28)
		// Register Address on byte 0
		i2cBuffer[0] = 0x3B;
		// Transmit Data to Accel over I2C from Master
		// (HandleTypeDef, slave Address, Data, Number of Bytes, Timeout)
		// This command transmits which register we want to read from the Accelerometer
		HAL_I2C_Master_Transmit(&hi2c1, mpu9265Address, i2cBuffer, 1, 10);
		// Clear the first byte of i2cBuffer to store read data
		i2cBuffer[1] = 0x00;
		// Read Data back from the Accelerometer
		// (HandleTypeDef, slave Address, buffer into which you want to write data after reading, number of bytes to read, timeout)
		// Number of bytes to read is 6 coz each axis gives 2 bytes of data
		HAL_I2C_Master_Receive(&hi2c1, mpu9265Address, &i2cBuffer[1], 6, 10);
		
		// Convert Accelrometer x,y,z data to g unit of Gravity
		// Since each axis has 2 bytes of data, we combine it together to form one 16 bits of data
		ax = -(i2cBuffer[1]<<8 | i2cBuffer[2]);
		ay = -(i2cBuffer[3]<<8 | i2cBuffer[4]);
		az = -(i2cBuffer[5]<<8 | i2cBuffer[6]);
		
		// Scale all Axis Value data to g unit of Gravity
		X_accel = ax/8192.0;
		Y_accel = ay/8192.0;
		Z_accel = az/8192.0;
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PG13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
