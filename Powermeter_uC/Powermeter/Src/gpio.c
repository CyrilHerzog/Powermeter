/*
 * gpio.c
 *
 *
 *  Created on: 05.04.2023
 *  Author: Herzog Cyril
 */

#include <reg.h>
#include <gpio.h>


gpio_reg_t *GPIOA = (gpio_reg_t *) GPIOA_BASE;
gpio_reg_t *GPIOB = (gpio_reg_t *) GPIOB_BASE;
gpio_reg_t *GPIOC = (gpio_reg_t *) GPIOC_BASE;
gpio_reg_t *GPIOD = (gpio_reg_t *) GPIOD_BASE;
gpio_reg_t *GPIOH = (gpio_reg_t *) GPIOH_BASE;


void gpiox_reset(){
	// reset all gpio register bank
	GPIOA->MODER = 0;
	uint32_t temp = (IOPx_IOPAx | IOPx_IOPBx | IOPx_IOPCx | IOPx_IOPDx | IOPx_IOPHx);
	RCC_IOPRSTR |= temp;
	RCC_IOPRSTR &= ~temp;
}

void gpio_config(gpio_reg_t *pPort, gpio_config_t *pConfig){
	uint32_t pinNum = (pConfig->pin & 0x0F);
	uint32_t shift;

	// Activate Port
	if (pPort == GPIOA){RCC_IOPENR |= IOPx_IOPAx;};
	if (pPort == GPIOB){RCC_IOPENR |= IOPx_IOPBx;};
	if (pPort == GPIOC){RCC_IOPENR |= IOPx_IOPCx;};
	if (pPort == GPIOD){RCC_IOPENR |= IOPx_IOPDx;};
	if (pPort == GPIOH){RCC_IOPENR |= IOPx_IOPHx;};

	//RCC_IOPSMEN |= IOPx_IOPBx;


	// First Config Registers => Config Bits = PinNum
	shift = pinNum;
	pPort->OTYPER &= ~(0x01u << shift);
	pPort->OTYPER |= (pConfig->output_mode << shift);
	// Second Config Registers => Config Bits = pinNum * 2
	shift = (shift << 1);
	pPort->MODER &= ~(0x03 << shift);
	pPort->MODER |= ((pConfig->func_mode & 0x03) << shift);
	pPort->OSPEEDR &= ~(0x03 << shift);
	pPort->OSPEEDR |= (pConfig->speed_mode << shift);
	pPort->PUPDR &= ~(0x03 << shift);
	pPort->PUPDR |= (pConfig->pull_mode << shift);

	// Alternate Config Registers => Config Bits = (pinNum * 4) in Range 0..28
	shift = (shift << 1) & 0x1c; // maximum 28
	if (pinNum > 0x07){  // Pin Num > 7
		pPort->AFRH &= ~(0x0f << shift);
		pPort->AFRH |= ((pConfig->func_mode >> 2) << shift);
	}
	else{
		pPort->AFRL &= ~(0x0f << shift);
		pPort->AFRL |= ((pConfig->func_mode >> 2) << shift);
	};

}


void gpio_set_val(gpio_reg_t *pPort, gpio_pin_t pin, bool_t val){
	uint32_t shift   = pin & 0x0F;

	pPort->ODR &= ~(0x01 << shift);
	if (val == true){pPort->ODR |= (0x01 << shift);};
}

bool_t gpio_get_val(gpio_reg_t *pPort, gpio_pin_t pin){

	bool_t temp = false;
	if (((pPort->IDR & (0x01u << pin)) & 0xFFFF) == (0x01u << pin)) {
		temp = true;
	}
	return temp;
}
