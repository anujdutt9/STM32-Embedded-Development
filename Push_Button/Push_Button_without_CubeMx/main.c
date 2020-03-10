/*
	Program to Toggle LED on User Button Press
*/

/* Include Libraries */
// STM32 HAL Library
#include "stm32f4xx_hal.h"


// Pin Configuration Function Declaration
void configurePins(void);
// Delay Function Declaration
void msDelay(uint32_t msTime);
// LED Toggle Function Declaration
void toggleLED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin_Name);

// Variable to store Push Button State: Boolean [Set/Reset] 
GPIO_PinState myPushButton;

int main(void){
	// Configure LED and Push Button Pins
	configurePins();
	
	while(1){
		// Step-1: Call ReadPin Button function
		myPushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
		
		// Step-2: Toggle LED's
		toggleLED(GPIOG, GPIO_PIN_13|GPIO_PIN_14);
	}
}


/* Function to Configure LED and Push Button Pins */
void configurePins(void){
	
	// Enable GPIO Clock
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	// GPIO Type Def
	GPIO_InitTypeDef myLEDConfig, myPushButtonConfig;
	
	// Set GPIO Pin Mode i.e. Output for LED's and Input for Push Button
	myLEDConfig.Mode = GPIO_MODE_OUTPUT_PP;
	myPushButtonConfig.Mode = GPIO_MODE_INPUT;
	
	// Select GPIO Pins to control
	myLEDConfig.Pin = GPIO_PIN_13|GPIO_PIN_14;
	myPushButtonConfig.Pin = GPIO_PIN_0;
	
	// GPIO Init Function Call
	// Pass in the GPIO Port and GPIO Config (as Pointer)
	HAL_GPIO_Init(GPIOG, &myLEDConfig);
	HAL_GPIO_Init(GPIOA, &myPushButtonConfig);
}


// Delay Function
// Function for Delay in "ms"
void msDelay(uint32_t msTime){
	/*
		Each for loop takes 4 clock cycles
		Clock Speed for STM32F429ZI board is 16 MHz
		16 MHz / 4 clock cycles = 4 MHz
		4 MHz / 1000 ms = 4000
	  That's why "msTime*4000"
	*/
	for(uint32_t i=0; i<msTime*4000; i++);
}


/*
	Function to Toggle LED
*/
void toggleLED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin_Name){
	
	// If the User Button is Pressed, Toggle LED's
	if (myPushButton == GPIO_PIN_SET){
		for(uint32_t i=0; i<5; i++){
			// Toggle LED's
			HAL_GPIO_TogglePin(GPIOx, GPIO_Pin_Name);
			msDelay(1000);
		}
	}
	
	// Else turn Off the LED's
	else {
		// Write User Button State to LED's
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14, myPushButton);
	}
}
