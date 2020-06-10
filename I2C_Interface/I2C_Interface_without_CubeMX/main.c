/* Include HAL Library Header File */
#include "stm32f4xx_hal.h"


// Function Declaration for GPIO & I2C Configuration
void GPIO_Config(void);
void I2C_Config(void);
void SysTick_Handler(void);

/* Define I2C Device Address as a Private Variable */
#define mpu9265Address 0xD0

// I2C HandleTypeDef
static I2C_HandleTypeDef myI2CTypeDef;

/* Define an Empty buffer to store Data Read/Write from/to I2C */
static uint8_t i2cBuffer[8];

/* Define Variables to hold x,y,z values of Accel */
static uint16_t ax, ay, az;
static float X_accel, Y_accel, Z_accel;

/* Main Function */
int main(void){
	
	// Initialize HAL Library
	HAL_Init();
	
	// Call Config Functions
	GPIO_Config();
	I2C_Config();
	
	// Since we don't know the I2C address for the device,
	// loop through all the addresses
	for (uint8_t i=0; i < 255; i++){
		// This function accepts inputs as:
		// (I2C_HandleTypeDef, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
		// Since, we want to check for all addresses i.e. i, for 1 trial each and for 10 ms
		if (HAL_I2C_IsDeviceReady(&myI2CTypeDef, i, 1, 10) == HAL_OK){
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
	HAL_I2C_Master_Transmit(&myI2CTypeDef, mpu9265Address, i2cBuffer, 2, 10);
	
	
	
	/* I2C Read Operation */
	
	// Request to Read Data from (reg28)
	// Register Address on byte 0
	i2cBuffer[0] = 28;
	// Transmit Data to Accel over I2C from Master
	// (HandleTypeDef, slave Address, Data, Number of Bytes, Timeout)
	// This command transmits which register we want to read from the Accelerometer
	HAL_I2C_Master_Transmit(&myI2CTypeDef, mpu9265Address, i2cBuffer, 1, 10);
	// Clear the first byte of i2cBuffer to store read data
	i2cBuffer[1] = 0x00;
	// Read Data back from the Accelerometer
	// (HandleTypeDef, slave Address, buffer into which you want to write data after reading, number of bytes to read, timeout)
	HAL_I2C_Master_Receive(&myI2CTypeDef, mpu9265Address, &i2cBuffer[1], 1, 10);
	
	while(1){
		
		/* Continuously read x,y,z data from the Accelerometer */
		// Request to Read Data from (reg28)
		// Register Address on byte 0
		i2cBuffer[0] = 0x3B;
		// Transmit Data to Accel over I2C from Master
		// (HandleTypeDef, slave Address, Data, Number of Bytes, Timeout)
		// This command transmits which register we want to read from the Accelerometer
		HAL_I2C_Master_Transmit(&myI2CTypeDef, mpu9265Address, i2cBuffer, 1, 10);
		// Clear the first byte of i2cBuffer to store read data
		i2cBuffer[1] = 0x00;
		// Read Data back from the Accelerometer
		// (HandleTypeDef, slave Address, buffer into which you want to write data after reading, number of bytes to read, timeout)
		// Number of bytes to read is 6 coz each axis gives 2 bytes of data
		HAL_I2C_Master_Receive(&myI2CTypeDef, mpu9265Address, &i2cBuffer[1], 6, 10);
		
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
}


/* Function to Configure GPIO Pins for LED and I2C */
void GPIO_Config(void){
	
	// Enable Port Clocks
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	
	// Create Init Type Def
	GPIO_InitTypeDef myGPIOTypeDef;
	
	
	// LED Pin Configuration
	// Set LED Pin Number
	myGPIOTypeDef.Pin = GPIO_PIN_13;
	// Set LED GPIO Mode -> Output
	myGPIOTypeDef.Mode = GPIO_MODE_OUTPUT_PP;
	// Set LED GPIO Pull
	myGPIOTypeDef.Pull = GPIO_NOPULL;
	// Set LED GPIO Speed
	myGPIOTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
	// Initialize LED GPIO Pins and apply Configuration
	HAL_GPIO_Init(GPIOG, &myGPIOTypeDef);
	
	
	// I2C Pin Configuration
	// I2C GPIO Pins
	myGPIOTypeDef.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	// Set I2C GPIO Mode -> Alternate Function Open Drain(own external pullup)
	myGPIOTypeDef.Mode = GPIO_MODE_AF_OD;
	// Set I2C GPIO Speed
	myGPIOTypeDef.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// Set Alternal Function, AF4 connects to I2C1 using Multiplexer
	myGPIOTypeDef.Alternate = GPIO_AF4_I2C1;
	// Initialize I2C GPIO Pins and apply Configuration
	HAL_GPIO_Init(GPIOB, &myGPIOTypeDef);
	
	
	// Enable Systick Interrupt for HAL_Delay Function
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


/* Function to Configure I2C Functionality */
void I2C_Config(void){
	
	// Enable I2C Peripheral Clock
	__HAL_RCC_I2C1_CLK_ENABLE();
	
	// Set I2C Instance
	myI2CTypeDef.Instance = I2C1;
	// Set I2C Addressing Mode to 7-bit
	myI2CTypeDef.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	// Set I2C Clock Speed -> 100 KHz
	myI2CTypeDef.Init.ClockSpeed = 100000;
	// Disable I2C Dual Addressing Mode
	myI2CTypeDef.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	// Set I2C Duty Cycle -> 2
	myI2CTypeDef.Init.DutyCycle = I2C_DUTYCYCLE_2;
	// Disable I2C General Call Mode
	myI2CTypeDef.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	// Disable I2C No Streach Mode
	myI2CTypeDef.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	// Set I2C Own Address 1
	myI2CTypeDef.Init.OwnAddress1 = 0;
	// Set I2C Own Address 2
	myI2CTypeDef.Init.OwnAddress2 = 0;
	// Apply I2C Pin Configuration
	HAL_I2C_Init(&myI2CTypeDef);
}


/* Systick Handler Function */
void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

