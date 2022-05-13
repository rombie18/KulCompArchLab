/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>

void delay(unsigned int n) {
  volatile unsigned int delay = n;
  while (delay--)
    ;
}

#define I2C_CR2_SADD I2C_CR2_SADD_Msk                  /*!< Slave address (master mode)                             */
#define I2C_CR2_RD_WRN I2C_CR2_RD_WRN_Msk                /*!< Transfer direction (master mode)                        */
#define I2C_CR2_ADD10 I2C_CR2_ADD10_Msk                /*!< 10-bit addressing mode (master mode)                    */
#define I2C_CR2_HEAD10R I2C_CR2_HEAD10R_Msk                /*!< 10-bit address header only read direction (master mode) */
#define I2C_CR2_START I2C_CR2_START_Msk                /*!< START generation                                        */
#define I2C_CR2_STOP I2C_CR2_STOP_Msk                /*!< STOP generation (master mode)                           */
#define I2C_CR2_NACK I2C_CR2_NACK_Msk                /*!< NACK generation (slave mode)                            */
#define I2C_CR2_NBYTES_Pos (16U)
#define I2C_CR2_NBYTES I2C_CR2_NBYTES_Msk                 /*!< Number of bytes                                         */
#define I2C_CR2_RELOAD I2C_CR2_RELOAD_Msk                /*!< NBYTES reload mode                                      */
#define I2C_CR2_AUTOEND I2C_CR2_AUTOEND_Msk                /*!< Automatic end mode (master mode)                        */
#define I2C_CR2_PECBYTE I2C_CR2_PECBYTE_Msk                /*!< Packet error checking byte                              */

uint8_t i2cread(uint8_t slave, uint8_t reg){
	while(I2C1->ISR & I2C_ISR_BUSY);

	uint32_t tmp = I2C1->CR2;

	tmp &= ~I2C_CR2_NBYTES;
	tmp &= ~I2C_CR2_STOP;
	tmp &= ~I2C_CR2_START;
	tmp &= ~I2C_CR2_SADD;
	tmp &= ~I2C_CR2_AUTOEND;
	tmp &= ~I2C_CR2_RD_WRN;

	tmp |= (slave << I2C_CR2_SADD_Pos);
	tmp |= (1 << I2C_CR2_NBYTES_Pos);
	tmp |= I2C_CR2_START;

	I2C1->CR2 = tmp;
	while(!(I2C1->ISR & I2C_ISR_TXIS));

	I2C1->TXDR = reg;
	while(!(I2C1->ISR & I2C_ISR_TC));

	tmp = I2C1->CR2;

	tmp &= ~I2C_CR2_NBYTES;
	tmp &= ~I2C_CR2_STOP;
	tmp &= ~I2C_CR2_START;
	tmp &= ~I2C_CR2_SADD;
	tmp &= ~I2C_CR2_AUTOEND;
	tmp &= ~I2C_CR2_RD_WRN;

	tmp |= (slave << I2C_CR2_SADD_Pos);
	tmp |= (1 << I2C_CR2_NBYTES_Pos);
	tmp |= I2C_CR2_RD_WRN;
	tmp |= I2C_CR2_START;
	tmp |= I2C_CR2_AUTOEND;

	I2C1->CR2 = tmp;
	while(!(I2C1->ISR & I2C_ISR_RXNE));

	uint8_t data = I2C1->RXDR;


	return data;

}
int main(void) {

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	// EXTI
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOB->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE9_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9;


	GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;
	GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;

	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;

	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI14_Msk;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB;

	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13_Msk;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PB;

	EXTI->IMR1 |= EXTI_IMR1_IM14;
	EXTI->FTSR1 |= EXTI_FTSR1_FT14;


	EXTI->IMR1 |= EXTI_IMR1_IM13;
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;

	NVIC_SetPriority(EXTI15_10_IRQn, 128);
	NVIC_EnableIRQ(EXTI15_10_IRQn);


	//ADC
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

	ADC1->CR &= ~ADC_CR_DEEPPWD;
	ADC1->CR |= ADC_CR_ADVREGEN;
	delay(25000);
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);

	ADC1->CR |= ADC_CR_ADEN;

	ADC1->SMPR1 &= ~ADC_SMPR1_SMP5;
	ADC1->SMPR1 |= ADC_SMPR1_SMP5_2 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_0;

	ADC1->SMPR1 &= ~ADC_SMPR1_SMP6;
	ADC1->SMPR1 |= ADC_SMPR1_SMP5_2 | ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_0;

	ADC1->SQR1 &= ~ADC_SQR1_L_Msk;
	ADC1->SQR1 |= (1 << ADC_SQR1_L_Pos);

	ADC1->SQR1 &= ~ADC_SQR1_SQ1_Msk;
	ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);

	ADC1->SQR1 &= ~ADC_SQR1_SQ2_Msk;
	ADC1->SQR1 |= (6 << ADC_SQR1_SQ2_Pos);

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
	//TIM16->BDTR |= TIM_BDTR_MOE;
	TIM16->CR1 |= TIM_CR1_CEN;


	//UART

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;


	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |=  GPIO_MODER_MODE9_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);

	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);

	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->BRR = 417;
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;


	// I2C

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);


	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;


	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	DWT->CYCCNT = 0;

	volatile uint16_t ntc = 0;
	volatile uint16_t pot = 0;
	volatile float voltage = 0.0f;
	printf("Test\r\n");
	while (1) {
		ADC1->CR |= ADC_CR_ADSTART;
		while(!(ADC1->ISR & ADC_ISR_EOC));
		ADC1->ISR = ADC_ISR_EOC;
		ntc = ADC1->DR;
		//while(!(ADC1->ISR & ADC_ISR_EOC));
		while(!(ADC1->ISR & ADC_ISR_EOS));
		ADC1->ISR = ADC_ISR_EOC | ADC_ISR_EOS;
		pot = ADC1->DR;

		DWT->CYCCNT = 0;
		voltage = (float)ntc / 4095.0f * 3.3f;
		float res = 10000.0f / (3.3f/voltage - 1);
		float temp = 1.0f/(logf(res/10000.0f)/3936.0f + 1.0f/298.15f) - 273.15f;
		uint32_t cyc = DWT->CYCCNT;
		delay(25000);
		printf("%5i %5i %02x %i\r\n", pot, (int)(temp*10),
				i2cread(0xA6, 0x00), cyc);
		if(temp<30.0f){
			TIM16->BDTR &=  ~TIM_BDTR_MOE;
		}else{
			TIM16->BDTR |= TIM_BDTR_MOE;
		}
	}
}


int __io_putchar(int ch){
	while(!(USART1->ISR & USART_ISR_TXE));
	USART1->TDR = ch;

}

int freq = 100;

int a = 0;
int test = 0;

void EXTI15_10_IRQHandler(void){
	if(EXTI->PR1 & EXTI_PR1_PIF14){
		EXTI->PR1 = EXTI_PR1_PIF14;
		freq = 200;
	}
	if(EXTI->PR1 & EXTI_PR1_PIF13){
		EXTI->PR1 = EXTI_PR1_PIF13;
		freq = 100;
	}
}

void SysTick_Handler(void){
	if(test == 0){
		if(a){
			GPIOB->BSRR = GPIO_BSRR_BS9;

			//TIM16->BDTR |= TIM_BDTR_MOE;
			a = 0;
		}else{
			GPIOB->BSRR = GPIO_BSRR_BR9;

			//TIM16->BDTR &= ~TIM_BDTR_MOE;
			a = 1;
		}
		test = freq;
	}else{
		test--;
	}


}
