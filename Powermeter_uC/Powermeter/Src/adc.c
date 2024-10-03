/*
 * adc.c
 *
 * Analog Digital-Converter for GPIO Port A. (Not all pins supported)
 * ToDo => Extend to all ports and variables sequence settings
 *
 *  Created on: 29.03.2023
 *      Author: Cyril Herzog
 */

#include <adc.h>

#define ADC_TIMEOUT (uint32_t)0x100   // 10ms


void adc_reset(){
	RCC_APB2RSTR |= APBx_ADC;
	RCC_APB2RSTR &= ~APBx_ADC;
}

system_err_t adc_init(adc_config_t *pConfig){

	uint32_t ticks_pre = get_ticks();

	gpio_config_t gpio_cfg;
	gpio_cfg.pin         = pConfig->pin_num;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_ANALOG;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOA, &gpio_cfg);

	RCC_APB2ENR |= APBx_ADC;
	/*  update F-Serie <=> L-Serie
	RCC_CFGR3   |= RCC_CFGR3_ADCSW; In L-Serie not used

	RCC_CR     |= RCC_CR_HSI14_ON;

	while(!(RCC_CR2 & RCC_CR_HSI14_RDY)){
		if ((get_ticks() - ticks_pre) >= ADC_TIMEOUT){
			return ADC_INIT_TIMEOUT;
		}
	}
*/
	ADC_CHSELR  |= ADC_CHSELR_CH0;
	ADC_SMPR    = pConfig->sample_time;
	ADC_CFGR1   = pConfig->resolution;
	ADC_CFGR2   = 0x00;

	if ((ADC_ISR & ADC_ISR_ADRDY) == ADC_ISR_ADRDY){
		ADC_ISR |= ADC_ISR_ADRDY;
	}
	ADC_CR |= ADC_CR_ADEN;
	while((ADC_ISR & ADC_ISR_ADRDY) == ADC_ISR_ADRDY){
		if ((get_ticks() - ticks_pre) >= ADC_TIMEOUT){
			return ADC_INIT_TIMEOUT;
		}
	}

	ADC_CR |= ADC_CR_ADSTART;

	return SYSTEM_OK;

}

system_err_t adc_cal(){
	uint32_t ticks_pre = get_ticks();

	if((ADC_CR & ADC_CR_ADEN) == ADC_CR_ADEN){
		ADC_CR |= ADC_CR_ADDIS;
	}
	while(!(ADC_CR & ADC_CR_ADEN)){
		if((get_ticks() - ticks_pre) >= ADC_TIMEOUT){
			return ADC_CALI_TIMEOUT;
		}
	}
	ADC_CR |= ADC_CR_ADCAL;
	while((ADC_CR & ADC_CR_ADCAL) == ADC_CR_ADCAL){
		if((get_ticks() - ticks_pre) >= ADC_TIMEOUT){
			return ADC_CALI_TIMEOUT;
		}
	}
	ADC_CR |= ADC_CR_ADEN;
	while((ADC_ISR & ADC_ISR_ADRDY) == ADC_ISR_ADRDY){
		if((get_ticks() - ticks_pre) >= ADC_TIMEOUT){
			return ADC_CALI_TIMEOUT;
		}
	}

	return SYSTEM_OK;
}


system_err_t adc_read(uint16_t *pData){
	uint32_t ticks_pre = get_ticks();
	ADC_CR |= ADC_CR_ADSTART;
	while(!(ADC_ISR & ADC_ISR_EOC)){
		if((get_ticks() - ticks_pre) >= ADC_TIMEOUT){
			return ADC_READ_TIMEOUT;
		}
	}
	*pData = (uint16_t)ADC_DR;
	//ADC_CR &= ~ADC_CR_ADSTART;
	return SYSTEM_OK;
}

