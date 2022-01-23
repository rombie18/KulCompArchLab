/**
 * @file system.c
 * @version 0.1
 *
 */

#include <stm32l4xx.h>
void SystemInit(void) {

	// Enable PWR
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

	// Switch voltage scaling
	if ((PWR->CR1 & PWR_CR1_VOS) != PWR_CR1_VOS_0){
		MODIFY_REG(PWR->CR1, PWR_CR1_VOS, PWR_CR1_VOS_0);
		while (PWR->SR2 & PWR_SR2_VOSF);
	}

	// Configure MSI
	while(!(RCC->CR & RCC_CR_MSIRDY));
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);
	SET_BIT(RCC->CR, RCC_CR_MSIRGSEL);
	MODIFY_REG(RCC->CR , RCC_CR_MSIRANGE_Msk, RCC_CR_MSIRANGE_11);

	// Configure LSE
	SET_BIT(PWR->CR1, PWR_CR1_DBP);
    SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
    SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
    while(!(RCC->BDCR & RCC_BDCR_LSERDY));

    // Enable MSI PLL MODE
    SET_BIT(RCC->CR, RCC_CR_MSIPLLEN);


    SCB->CPACR |= ((3UL << 20U) | (3UL << 22U)); /* set CP10 and CP11 Full Access */
}
