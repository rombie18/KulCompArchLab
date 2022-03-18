#include <stdint.h>
#include <stm32l4xx.h>

/*** GLOBAL VARIABLES ***/
char displayMux = 0;
char display[4] = { 0 };
volatile char tick = 0;
unsigned int debounce = 0;
unsigned int timecounter = 0;

/*** FUNCTION PROTOTYPES ***/
void init();
void readButtons();
void tickTime(char *display, char i);
void delay(const unsigned int);
void convertNumber(char *display, const int number);
void convertTime(char *display, const int hours, const int minutes);
void multiplexSegments(char *multiplexer, const char *value);
void clearSegment();
void showSegment(char character, char dot);

int main(void) {

	init();

	while (1) {
		multiplexSegments(&displayMux, display);
		readButtons();
		if (tick) {
			if (debounce > 0) {
				debounce--;
			}
			if (timecounter > 0) {
				timecounter--;
			} else {
				timecounter = 1000;
				tickTime(display, 1);
			}
		}
		tick = 0;
	}

}

void init() {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	//7segmentjes
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

	//mux + punt
	GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE15_Msk
			| GPIO_MODER_MODE6_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE15_0
			| GPIO_MODER_MODE6_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT15 | GPIO_OTYPER_OT6);

	//knoppen
	// GPIOB Pin 14 configureren als input
	GPIOB->MODER &= ~(GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk | GPIO_PUPDR_PUPD14_Msk);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD14_0);

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void) {
	tick = 1;
}

void readButtons() {
	if (debounce == 0) {
		if (!(GPIOB->IDR & GPIO_IDR_ID13)) {
			tickTime(display, 1);
			debounce = 500;
		}
		if (!(GPIOB->IDR & GPIO_IDR_ID14)) {
			tickTime(display, 60);
			debounce = 500;
		}
	}
}

void tickTime(char *n, char i) {
	char hour = n[0] * 10 + n[1];
	char min = n[2] * 10 + n[3];
	min += i;
	if (min >= 60) {
		min -= 60;
		hour += 1;
		if (hour > 23) {
			hour -= 24;
		}
	}
	n[0] = hour / 10;
	n[1] = hour - n[0] * 10;
	n[2] = min / 10;
	n[3] = min - n[2] * 10;
}

/*** FUNCTIONS ***/
void delay(volatile unsigned int i) {
	while (i > 0) {
		i--;
	}
}

void convertNumber(char *display, const int number) {
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

void convertTime(char *display, const int hours, const int minutes) {
	convertNumber(display, hours * 100 + minutes);
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
