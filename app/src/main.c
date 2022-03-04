/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>

int main(void) {

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	// Enable GPIOB and GPIOC blocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// Enable button A
	GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;
	// Enable button B
	GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;

	// Enable pull-up resistors for buttons
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

	//

	// Main loop
	while (1) {

	}
}

void Systick(void){
	updateDisplay();
}

void updateDisplay() {

}
