/*
 * adc.h
 *
 *  Created on: 29.03.2023
 *      Author: cyril
 */

#ifndef ADC_H_
#define ADC_H_

#include <system.h>
#include <gpio.h>


#define APBx_ADC		 (uint32_t) 0x200
/* update F-Serie <=> L-Serie
#define RCC_CFGR3_ADCSW  (uint32_t) 0x100
#define RCC_CR_HSI14_ON  (uint32_t) 0x01
#define RCC_CR_HSI14_RDY (uint32_t) 0x02
*/
#define ADC_CR_ADEN      (uint32_t) 0x01
#define ADC_CR_ADDIS     (uint32_t) 0x02
#define ADC_CR_ADSTART   (uint32_t) 0x04
#define ADC_CR_ADSTP     (uint32_t) 0x10
#define ADC_CR_ADCAL     (uint32_t) 0x80000000
#define ADC_ISR_ADRDY	 (uint32_t) 0x01
#define ADC_ISR_EOSMP    (uint32_t) 0x02
#define ADC_ISR_EOC      (uint32_t) 0x04
#define ADC_ISR_EOSEQ    (uint32_t) 0x08

#define ADC_SMPR_SMP0    (uint32_t) 0x01
#define ADC_SMPR_SMP1    (uint32_t) 0x02
#define ADC_SMPR_SMP2    (uint32_t) 0x04

#define ADC_CFGR1_RES0   (uint32_t) 0x08
#define ADC_CFGR1_RES1   (uint32_t) 0x0F
#define ADC_CFGR1_AUTOFF (uint32_t) 0x8000

#define ADC_CHSELR_CH0   (uint32_t) 0x01


typedef enum{
	ADC_CYCLE_1_5   = 0,
	ADC_CYCLE_7_5   = ADC_SMPR_SMP0,
	ADC_CYCLE_13_5  = ADC_SMPR_SMP1,
	ADC_CYCLE_28_5  = ADC_SMPR_SMP0 | ADC_SMPR_SMP1,
	ADC_CYCLE_41_5  = ADC_SMPR_SMP2,
	ADC_CYCLE_55_5  = ADC_SMPR_SMP0 | ADC_SMPR_SMP2,
	ADC_CYCLE_71_5  = ADC_SMPR_SMP1 | ADC_SMPR_SMP2,
	ADC_CYCLE_239_5 = ADC_SMPR_SMP0 | ADC_SMPR_SMP1 | ADC_SMPR_SMP2
}adc_sample_time_t;

typedef enum{
	ADC_RES_12_BIT = 0,
	ADC_RES_10_BIT = ADC_CFGR1_RES0,
	ADC_RES_8_BIT  = ADC_CFGR1_RES1,
	ADC_RES_6_BIT  = ADC_CFGR1_RES0 | ADC_CFGR1_RES1
}adc_resolution_t;

//
typedef struct{
	gpio_pin_t 		  pin_num;
	adc_resolution_t  resolution;
	adc_sample_time_t sample_time;
}adc_config_t;

void adc_reset();

system_err_t adc_init(adc_config_t *pConfig);

system_err_t adc_cal();

system_err_t adc_read(uint16_t *pData);


#endif /* ADC_H_ */
