/*
 * system.h
 *
 * ToDo: Improve better Error Handling
 * 		 create Abort() - Function
 * 		 const char* get_error_name(system_err_t code);
 * 		 void error_check(system_err_t code);
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>
#include <reg.h>


//Defines general config Clock Register's
#define RCC_CR_DEFAULT					(uint32_t) 0x00000300
#define RCC_CR_HSI16_ON					(uint32_t) 0x00000001
#define RCC_CR_HSI16_RDY				(uint32_t) 0x00000004
#define RCC_CFGR_PLLSRC					(uint32_t) 0x00008000
#define RCC_CR_HSI16_DIV_EN				(uint32_t) 0x00000008
#define RCC_CR_HSI16_DIVF				(uint32_t) 0x00000010
#define RCC_CFGR_PLL_MULX8  			(uint32_t) 0x000C0000
#define RCC_CFGR_PLL_MULX16				(uint32_t) 0x00140000
#define RCC_CR_PLL_ON					(uint32_t) 0x01000000
#define RCC_CR_PLL_RDY					(uint32_t) 0x02000000
#define RCC_CFGR_SW_PLL					(uint32_t) 0x00000003
#define RCC_CFGR_SW_HSI16				(uint32_t) 0x00000001
#define RCC_CFGR_SWS_PLL				(uint32_t) 0x0000000C
#define RCC_CFGR_PLL_DIV2				(uint32_t) 0x00400000
#define RCC_CFGR_HPRE_DIV2				(uint32_t) 0x00000080

/* Used for F-Serie => not tested for L-Serie
#define RCC_CR_DEFAULT					(uint32_t) 0x00000083
#define RCC_CR_DEFAULT					(uint32_t) 0x00000300
#define RCC_CR_HSION					(uint32_t) 0x00000001
#define RCC_CR_HSIRDY					(uint32_t) 0x00000002
#define RCC_CR_HSEON					(uint32_t) 0x00010000
#define RCC_CR_HSERDY					(uint32_t) 0x00020000
#define RCC_CR_PLLON					(uint32_t) 0x01000000
#define RCC_CR_PLLRDY					(uint32_t) 0x02000000
#define RCC_CFGR_SW						(uint32_t) 0x00000003
#define RCC_CFGR_SWS					(uint32_t) 0x0000000C
#define RCC_CFGR_HPRE					(uint32_t) 0x000000F0
#define RCC_CFGR_PPRE					(uint32_t) 0x00000700
#define RCC_CFGR_ADCPRE				    (uint32_t) 0x00004000
#define RCC_CFGR_PLLSRC					(uint32_t) 0x00010000
#define RCC_CFGR_PLLXTPRE				(uint32_t) 0x00020000
#define RCC_CFGR_PLLMUL					(uint32_t) 0x003C0000
#define RCC_CFGR_MCO					(uint32_t) 0x0F000000
#define RCC_CFGR_MCOPRE					(uint32_t) 0x70000000
#define RCC_CFGR_PLLNODIV				(uint32_t) 0x80000000
#define RCC_CFGR_SWHSI     				(uint32_t) 0x00000000
#define RCC_CFGR_SWHSE                  (uint32_t) 0x00000001
#define RCC_CFGR_SWPLL					(uint32_t) 0x00000002
#define RCC_CFGR_SWSHSE					(uint32_t) 0x00000001
#define RCC_CFGR_SWSPLL					(uint32_t) 0x00000002
#define RCC_CFGR_HPREDIV2				(uint32_t) 0x00000080
#define RCC_CFGR_HPREDIV4				(uint32_t) 0x00000090
#define RCC_CFGR_HPREDIV8				(uint32_t) 0x000000A0
#define RCC_CFGR_HPREDIV16				(uint32_t) 0x000000B0
#define RCC_CFGR_HPREDIV64				(uint32_t) 0x000000C0
#define RCC_CFGR_HPREDIV128				(uint32_t) 0x000000D0
#define RCC_CFGR_HPREDIV256				(uint32_t) 0x000000E0
#define RCC_CFGR_HPREDIV512				(uint32_t) 0x000000F0
#define RCC_CFGR_PPREDIV2				(uint32_t) 0x00000400
#define RCC_CFGR_PPREDIV4				(uint32_t) 0x00000500
#define RCC_CFGR_PPREDIV8				(uint32_t) 0x00000600
#define RCC_CFGR_PPREDIV16				(uint32_t) 0x00000700
#define RCC_CFGR_PLLMULX3				(uint32_t) 0x00040000
#define RCC_CFGR_PLLMULX4				(uint32_t) 0x00080000
#define RCC_CFGR_PLLMULX5				(uint32_t) 0x000C0000
#define RCC_CFGR_PLLMULX6				(uint32_t) 0x00100000
#define RCC_CFGR_PLLMULX7				(uint32_t) 0x00140000
#define RCC_CFGR_PLLMULX8				(uint32_t) 0x00180000
#define RCC_CFGR_PLLMULX9				(uint32_t) 0x001C0000
#define RCC_CFGR_PLLMULX10				(uint32_t) 0x00200000
#define RCC_CFGR_PLLMULX11				(uint32_t) 0x00240000
#define RCC_CFGR_PLLMULX12				(uint32_t) 0x00280000
#define RCC_CFGR_PLLMULX13				(uint32_t) 0x002C0000
#define RCC_CFGR_PLLMULX14				(uint32_t) 0x00300000
#define RCC_CFGR_PLLMULX15				(uint32_t) 0x00340000
#define RCC_CFGR_PLLMULX16				(uint32_t) 0x00380000
//#define RCC_CFGR_PLLMULX16			(uint32_t) 0x003C0000  wrong in Datasheet ?
#define RCC_CFGR_MCOHSI14				(uint32_t) 0x01000000
#define RCC_CFGR_MCOLSI				    (uint32_t) 0x02000000
#define RCC_CFGR_MCOLSE				    (uint32_t) 0x03000000
#define RCC_CFGR_MCOSYSCLK				(uint32_t) 0x04000000
#define RCC_CFGR_MCOHSI			    	(uint32_t) 0x05000000
#define RCC_CFGR_MCOHSE			    	(uint32_t) 0x06000000
#define RCC_CFGR_MCOPLL		     		(uint32_t) 0x07000000
#define RCC_CFGR_MCODIV2				(uint32_t) 0x10000000
#define RCC_CFGR_MCODIV4				(uint32_t) 0x20000000
#define RCC_CFGR_MCODIV8				(uint32_t) 0x30000000
#define RCC_CFGR_MCODIV16				(uint32_t) 0x40000000
#define RCC_CFGR_MCODIV32				(uint32_t) 0x50000000
#define RCC_CFGR_MCODIV64				(uint32_t) 0x60000000
#define RCC_CFGR_MCODIV128				(uint32_t) 0x70000000
#define RCC_CFGR2_PREDIV2				(uint32_t) 0x00000001
#define RCC_CFGR2_PREDIV3				(uint32_t) 0x00000002
#define RCC_CFGR2_PREDIV4				(uint32_t) 0x00000003
#define RCC_CFGR2_PREDIV5				(uint32_t) 0x00000004
#define RCC_CFGR2_PREDIV6				(uint32_t) 0x00000005
#define RCC_CFGR2_PREDIV7				(uint32_t) 0x00000006
#define RCC_CFGR2_PREDIV8				(uint32_t) 0x00000007
#define RCC_CFGR2_PREDIV9				(uint32_t) 0x00000008
#define RCC_CFGR2_PREDIV10				(uint32_t) 0x00000009
#define RCC_CFGR2_PREDIV11				(uint32_t) 0x0000000A
#define RCC_CFGR2_PREDIV12				(uint32_t) 0x0000000B
#define RCC_CFGR2_PREDIV13				(uint32_t) 0x0000000C
#define RCC_CFGR2_PREDIV14				(uint32_t) 0x0000000D
#define RCC_CFGR2_PREDIV15				(uint32_t) 0x0000000E
#define RCC_CFGR2_PREDIV16				(uint32_t) 0x0000000F
#define RCC_CFGR3_USART1_SYSCLK			(uint32_t) 0x00000001
#define RCC_CFGR3_USART1_LSE			(uint32_t) 0x00000002
#define RCC_CFGR3_USART1_HSI			(uint32_t) 0x00000003
*/

// Timer
#define TIMER_CR1_CEN					(uint32_t) 0x01
#define TIMER_DIER_UIE					(uint32_t) 0x01
#define TIMER_SR_UIF					(uint32_t) 0x01
#define RCC_APB1ENR_TIMER2				(uint32_t) 0x01

// Defines System control Block
#define ICSR_PENDSVSET					(uint32_t) 0x10000000

//Vector Table => RM0360
#define ISER_USART1_IRQ					(uint32_t) 0x8000000
#define ISER_TIMER2_IRQ                 (uint32_t) 0x0008000



// Defines for using SysTick
#define SYSTICK_ENABLE					(uint32_t) 0x00001
#define SYSTICK_INTR_ENABLE				(uint32_t) 0x00002
#define SYSTICK_ENABLE_PROCESSOR_CLOCK	(uint32_t) 0x00004
#define SYSTICK_COUNTFLAG				(uint32_t) 0x10000

// boolean type
typedef enum{
	false = 0, true = !false
}bool_t;

// System Error Codes
typedef enum{
	SYSTEM_OK = 0,
	ADC_INIT_TIMEOUT,
	ADC_CALI_TIMEOUT,
	ADC_READ_TIMEOUT,
	SPI_WRITE_TIMEOUT,
	SPI_TRANSACTION_TIMEOUT,
	USART_INVALID_PARAM,
	USART_SEND_TIMEOUT,
	USART_RECEIVE_TIMEOUT,
	USART_SEND_STRING_OVERFLOW,
	I2C_INVALID_PARAM,
	I2C_TRANSMIT_TIMEOUT,
	I2C_RECEIVE_TIMEOUT,
	RCC_TIMEOUT
}system_err_t;


// get current Milliseconds from SysTick Handler
uint32_t get_ticks();

// delay function using SysTick
void wait(uint32_t ms);

// print a string to console (COM3)
void print(const char *msg);

// System initialization => Reset, Clock management, Interrupt set, ...
system_err_t init_system();


void __OS();
void __Run_Up(void);
void __Task_1(void);
void __Task_2(void);
void __Error(void);


// not implemented
void error_check(system_err_t code);

// not implemented
const char* get_error_name(system_err_t code);

#endif /* SYSTEM_H_ */
