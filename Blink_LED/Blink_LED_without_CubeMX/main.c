/* 
	Blinking LED without using CubeMX
*/

/* Include Libraries */

// STM32 HAL Library
#include "stm32f4xx_hal.h"

/* Configure LED Pins Function Declaration */
void configureLEDPin(void);
/* Configure Delay Function Declaration */
void msDelay(uint32_t msTime);

/* Main Function */
int main(void){
	
	// Step-1: Configure LED Pins
	configureLEDPin();
	// Step-2: Define Delay Function
	
	while(1){
		// Call function to Toggle GPIO's
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14);
		
		// Call 1 sec Delay
		msDelay(1000);
	}
}


/* Function to Configure LED Pins */
void configureLEDPin(void){
	
	// Enable GPIO Clock
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	// GPIO Type Def
	GPIO_InitTypeDef myLEDConfig;
	
	// Set GPIO Pin Mode i.e. Output
	myLEDConfig.Mode = GPIO_MODE_OUTPUT_PP;
	
	// Select GPIO Pins to control
	myLEDConfig.Pin = GPIO_PIN_13|GPIO_PIN_14;
	
	// GPIO Init Function Call
	// Pass in the GPIO Port and GPIO Config (as Pointer)
	HAL_GPIO_Init(GPIOG, &myLEDConfig);
}


// Step-2: Define Delay Function
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