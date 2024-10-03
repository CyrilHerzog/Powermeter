/*
 * reg.h
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 */

#include <stdint.h>

#ifndef REG_H_
#define REG_H_

// ToDo summary to a structure
#define NVIC_BASE    (uint32_t) 0xE000E100
#define NVIC_ISER	 (*((volatile uint32_t *)(NVIC_BASE + 0x000)))
#define NVIC_ICER	 (*((volatile uint32_t *)(NVIC_BASE + 0x080)))
#define NVIC_ISPR	 (*((volatile uint32_t *)(NVIC_BASE + 0x100)))
#define NVIC_IPRO	 (*((volatile uint32_t *)(NVIC_BASE + 0x300)))
#define NVIC_IPR1	 (*((volatile uint32_t *)(NVIC_BASE + 0x304)))
#define NVIC_IPR2	 (*((volatile uint32_t *)(NVIC_BASE + 0x308)))
#define NVIC_IPR3	 (*((volatile uint32_t *)(NVIC_BASE + 0x30c)))
#define NVIC_IPR4	 (*((volatile uint32_t *)(NVIC_BASE + 0x310)))
#define NVIC_IPR5	 (*((volatile uint32_t *)(NVIC_BASE + 0x314)))
#define NVIC_IPR6	 (*((volatile uint32_t *)(NVIC_BASE + 0x318)))
#define NVIC_IPR7	 (*((volatile uint32_t *)(NVIC_BASE + 0x31c)))

// ToDo summary to a structure
#define RCC_BASE      (uint32_t) 0x40021000
#define RCC_CR		  (*((volatile uint32_t *)(RCC_BASE + 0x00)))
#define RCC_ICSCR	  (*((volatile uint32_t *)(RCC_BASE + 0x04)))
#define RCC_CFGR      (*((volatile uint32_t *)(RCC_BASE + 0x0C)))
#define RCC_CIER      (*((volatile uint32_t *)(RCC_BASE + 0x10)))
#define RCC_CIFR      (*((volatile uint32_t *)(RCC_BASE + 0x14)))
#define RCC_CICR      (*((volatile uint32_t *)(RCC_BASE + 0x18)))
#define RCC_IOPRSTR   (*((volatile uint32_t *)(RCC_BASE + 0x1C)))
#define RCC_AHBRSTR   (*((volatile uint32_t *)(RCC_BASE + 0x20)))
#define RCC_APB2RSTR  (*((volatile uint32_t *)(RCC_BASE + 0x24)))
#define RCC_APB1RSTR  (*((volatile uint32_t *)(RCC_BASE + 0x28)))
#define RCC_IOPENR    (*((volatile uint32_t *)(RCC_BASE + 0x2C)))
#define RCC_AHBENR    (*((volatile uint32_t *)(RCC_BASE + 0x30)))
#define RCC_APB2ENR   (*((volatile uint32_t *)(RCC_BASE + 0x34)))
#define RCC_APB1ENR   (*((volatile uint32_t *)(RCC_BASE + 0x38)))
#define RCC_IOPSMEN   (*((volatile uint32_t *)(RCC_BASE + 0x3C)))
#define RCC_AHBSMENR  (*((volatile uint32_t *)(RCC_BASE + 0x40)))
#define RCC_APB2SMENR (*((volatile uint32_t *)(RCC_BASE + 0x44)))
#define RCC_APB1SMENR (*((volatile uint32_t *)(RCC_BASE + 0x48)))
#define RCC_CCIPR     (*((volatile uint32_t *)(RCC_BASE + 0x4C)))
#define RCC_CSR       (*((volatile uint32_t *)(RCC_BASE + 0x50)))


// ToDo summary to a structure
#define ADC_BASE	  (uint32_t) 0x40012400
#define ADC_ISR       (*((volatile uint32_t *)(ADC_BASE + 0x00)))
#define ADC_IER       (*((volatile uint32_t *)(ADC_BASE + 0x04)))
#define ADC_CR        (*((volatile uint32_t *)(ADC_BASE + 0x08)))
#define ADC_CFGR1     (*((volatile uint32_t *)(ADC_BASE + 0x0C)))
#define ADC_CFGR2     (*((volatile uint32_t *)(ADC_BASE + 0x10)))
#define ADC_SMPR      (*((volatile uint32_t *)(ADC_BASE + 0x14)))
#define ADC_CHSELR    (*((volatile uint32_t *)(ADC_BASE + 0x28)))
#define ADC_DR        (*((volatile uint32_t *)(ADC_BASE + 0x40)))


// System control block (SCB)
#define SCB_BASE     (uint32_t) 0xE000ED00

typedef struct {
	volatile uint32_t CPUID;
	volatile uint32_t ICSR_0;
	volatile uint32_t ICSR_1;
	volatile uint32_t AIRCR;
	volatile uint32_t SCR;
	volatile uint32_t CCR_0;
	volatile uint32_t CCR_1;
	volatile uint32_t SHPR2;
	volatile uint32_t SHPR3;
}scb_reg_t;


#define STK_BASE	 (uint32_t) 0xE000E010

typedef struct{
	volatile uint32_t CSR;
	volatile uint32_t RVR;
	volatile uint32_t CVR;
	volatile uint32_t CALIB;
}systick_reg_t;


#define TIMER2_BASE  (uint32_t) 0x40000000

typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t Reserve;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t OR;
}timer_reg_t;


#define USART1_BASE  (uint32_t) 0x40013800
#define USART2_BASE  (uint32_t) 0x40004400

typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t BRR;
	volatile uint32_t GTPR;
	volatile uint32_t RTOR;
	volatile uint32_t RQR;
	volatile uint32_t ISR;
	volatile uint32_t ICR;
	volatile uint32_t RDR;
	volatile uint32_t TDR;
}usart_reg_t;

#define GPIOA_BASE    (uint32_t) 0x50000000
#define GPIOB_BASE    (uint32_t) 0x50000400
#define GPIOC_BASE    (uint32_t) 0x50000800
#define GPIOD_BASE    (uint32_t) 0x50000C00
#define GPIOH_BASE	  (uint32_t) 0x50001C00

typedef struct{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
	volatile uint32_t BRR;
}gpio_reg_t;



#define SPI1_BASE (uint32_t) 0x40013000
#define SPI2_BASE (uint32_t) 0x40003800

typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t CRCPR;
	volatile uint32_t RXCRCR;
	volatile uint32_t TXCRCR;
	volatile uint32_t I2SCFGR;
	volatile uint32_t I2SPR;
}spi_reg_t;

#define I2C1_BASE (uint32_t) 0x40005400
#define I2C2_BASE (uint32_t) 0x40005800


typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t OAR1;
	volatile uint32_t OAR2;
	volatile uint32_t TIMINGR;
	volatile uint32_t TIMEOUTR;
	volatile uint32_t ISR;
	volatile uint32_t ICR;
	volatile uint32_t PECR;
	volatile uint32_t RXDR;
	volatile uint32_t TXDR;
}i2c_reg_t;


#endif /* REG_H_ */
