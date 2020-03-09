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
		// Toggle LED using XOR Operation
		GPIOG->ODR ^= (0xFUL << 13);
		
		// Call 1 sec Delay
		msDelay(1000);
	}
}


/* Function to Configure LED Pins */
void configureLEDPin(void){
	
	/*
		Enable GPIO Clock
		To Enable GPIOG, we need to set 6th bit as 1
		Ref.: 6.3.10 RCC AHB1 peripheral clock register (RCC_AHB1ENR)
	*/
	RCC->AHB1ENR |= (1<<6);
	
	/* 
		Clear GPIO's by setting the GPIO Port Mode Register to 
	  Reset State: 00
	*/
	GPIOG->MODER &= ~(0xFF<<12*2);
	
	/* 
		Set GPIO's as Ouput by setting the GPIO Port Mode Register 
		to General Purpose Output Mode: 01
						MODER15	MODER14	MODER13	MODER12
		0x14 =>    00 		01 			01 			00
		Ref.: 8.4.1 GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	*/
	GPIOG->MODER |= (0x14<<12*2);
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
