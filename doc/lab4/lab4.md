# Lab 4: De buzzer

## Opzet

Het doel van dit labo is om de PWM mogelijkheden te leren gebruiken.

## TIMER

📌 **RM373**

### Implementatie

	// TIMER
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;


	GPIOB->MODER &= ~GPIO_MODER_MODE8_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE8_1;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT8;
	GPIOB->AFR[1] = (GPIOB->AFR[1] & (~GPIO_AFRH_AFSEL8_Msk)) | (0xE << GPIO_AFRH_AFSEL8_Pos);


	TIM16->PSC = 0;
	TIM16->ARR = 24000;
	TIM16->CCR1 = 12000;

	TIM16->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1FE;
	TIM16->CCER |= TIM_CCER_CC1E;
	TIM16->CCER &= ~TIM_CCER_CC1P;
	TIM16->BDTR |= TIM_BDTR_MOE;
	TIM16->CR1 |= TIM_CR1_CEN;

## GPIO Alternate modes


## Opdracht

1. Breid het programma van lab3 uit zodat de buzzer een signaal geeft als de temperatuur boven een bepaalde drempelwaarde komt. Deze drempelwaarde kan men instellen via de potmeter.

