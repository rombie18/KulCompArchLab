#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>

/*** GLOBAL VARIABLES ***/
char displayMux = 0;
char display[4] = { 0 };
volatile char tick = 0;

/*** FUNCTION PROTOTYPES ***/
void init();
void delay(const unsigned int);
void convertInt(char *display, const int number);
void convertFloat(char *display, const float number);
void convertTime(char *display, const int hours, const int minutes);
void multiplexSegments(char *multiplexer, const char *value);
void clearSegment();
void showSegment(char character, char dot);

int main(void) {

	init();

	while (1) {

		multiplexSegments(&displayMux, display);
		if (tick) {

			convertFloat(display, 20.32f);

		}
		tick = 0;
	}
}

void init() {

	//systick configureren en interupt aanzetten
	SysTick_Config(48000);
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	// 7 segmenten aanzetten (als output declareren) en laag maken
	GPIOA->MODER &= ~(GPIO_MODER_MODE7_Msk | GPIO_MODER_MODE5_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE5_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT5);

	GPIOB->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE12_Msk
			| GPIO_MODER_MODE15_Msk | GPIO_MODER_MODE1_Msk
			| GPIO_MODER_MODE2_Msk);
	GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE12_0
			| GPIO_MODER_MODE15_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT12 | GPIO_OTYPER_OT15
			| GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2);

	//mux en punt aansturen
	GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE15_Msk
			| GPIO_MODER_MODE6_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE15_0
			| GPIO_MODER_MODE6_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT15 | GPIO_OTYPER_OT6);

	// I2C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	// GPIO alternative mode
	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);

	// I2C 100kHz clock
	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;

	I2C1->CR2 |=



}


void write_register() {

}

void read_register() {

}

void SysTick_Handler(void) {
	tick = 1;
}


/*** FUNCTIONS ***/
void delay(volatile unsigned int i) {
	while (i > 0) {
		i--;
	}
}

void convertInt(char *display, const int number) {
	int temp = number;
	char thousand = 0;
	char hunderd = 0;
	char ten = 0;
	char one = 0;

	thousand = temp / 1000;
	temp -= thousand * 1000;
	hunderd = temp / 100;
	temp -= hunderd * 100;
	ten = temp / 10;
	temp -= ten * 10;
	one = temp;

	display[0] = thousand;
	display[1] = hunderd;
	display[2] = ten;
	display[3] = one;
}

void convertFloat(char *display, const float number) {
	convertInt(display, (int) (number * 100.0f));
}

void convertTime(char *display, const int hours, const int minutes) {
	convertInt(display, hours * 100 + minutes);
}

void multiplexSegments(char *mux, const char *value) {
	if (*mux == 1) {
		// 00 - 10 - 01 - 11
		GPIOA->ODR &= ~GPIO_ODR_OD8;
		GPIOA->ODR &= ~GPIO_ODR_OD15;
		showSegment(value[0], 0);
	} else if (*mux == 2) {
		// 00 - 10 - 01 - 11
		GPIOA->ODR |= GPIO_ODR_OD8;
		GPIOA->ODR &= ~GPIO_ODR_OD15;
		showSegment(value[1], 1);
	} else if (*mux == 3) {
		// 00 - 10 - 01 - 11
		GPIOA->ODR &= ~GPIO_ODR_OD8;
		GPIOA->ODR |= GPIO_ODR_OD15;
		showSegment(value[2], 0);
	} else if (*mux == 4) {
		// 00 - 10 - 01 - 11
		GPIOA->ODR |= GPIO_ODR_OD8;
		GPIOA->ODR |= GPIO_ODR_OD15;
		showSegment(value[3], 0);
		*mux = 0;
	}

	delay(100);
	clearSegment();
	*mux += 1;
}

void clearSegment() {
	//reset all segments
	GPIOA->ODR &= ~GPIO_ODR_OD6;
	GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1
			| GPIO_ODR_OD2);
}

void showSegment(char character, char dot) {

	if (dot == 1) {
		GPIOA->ODR |= GPIO_ODR_OD6;
	}

	//segment  E
	if (character != 1 && character != 3 && character != 4 && character != 5
			&& character != 7 && character != 9) {
		GPIOB->ODR |= GPIO_ODR_OD15;
	}

	//segment F
	if (character != 1 && character != 2 && character != 3 && character != 7) {
		GPIOB->ODR |= GPIO_ODR_OD1;
	}

	//segment A
	if (character != 1 && character != 4) {
		GPIOB->ODR |= GPIO_ODR_OD0;
	}

	//segment B
	if (character != 5 && character != 6) {
		GPIOA->ODR |= GPIO_ODR_OD7;
	}

	//segment C
	if (character != 2) {
		GPIOA->ODR |= GPIO_ODR_OD5;
	}

	//segment D
	if (character != 1 && character != 4 && character != 7) {
		GPIOB->ODR |= GPIO_ODR_OD12;
	}

	//segment G
	if (character != 1 && character != 7 && character != 0) {
		GPIOB->ODR |= GPIO_ODR_OD2;
	}
}
