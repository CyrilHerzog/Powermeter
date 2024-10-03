/*
 * system.c
 *
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 */

#include <system.h>
#include <usart.h>
#include <adc.h>
#include <gpio.h>
#include <spi.h>

// some of the task structure's and enum's are not really used for this application
// it was originally created for a different approach
typedef struct{
	uint8_t task_num;
	uint32_t freq;
	uint32_t phase;
}task_config_t;

typedef enum{
	EXECUTE = 1,
	PENDING = 2,
	DONE    = 3
}task_state_t;

typedef struct{
	void(*task_func)(void);
	uint32_t freq;
	uint32_t phase;
	uint32_t count;
	task_state_t request;
	task_state_t state;
}task_control_t;


systick_reg_t *STK = (systick_reg_t *) STK_BASE;
scb_reg_t *SCB     = (scb_reg_t *) SCB_BASE;
timer_reg_t *TIM2  = (timer_reg_t *) TIMER2_BASE;

static volatile uint32_t msTicks = 0;

static volatile task_control_t task_cntrl[3];


void __OS() {
	// delay before init the system
	for (uint32_t j=0; j<524288; j++) {
		asm("NOP");
	}
	task_cntrl[0].task_func = __Run_Up;
	task_cntrl[1].task_func = __Task_1;
	task_cntrl[2].task_func = __Task_2;
	// Run Up Task for all Configuration
	task_cntrl[0].state = EXECUTE;
	task_cntrl[0].task_func();
	task_cntrl[0].state = DONE;

	// Timer start => start task 1
	TIM2->CR1    |= TIMER_CR1_CEN;
	while(true) {
		// start with execute task's
		task_cntrl[2].task_func();
	}

}


void task_config(task_config_t *pConfig) {
	task_cntrl[pConfig->task_num].count = pConfig->phase;
	task_cntrl[pConfig->task_num].freq  = pConfig->freq;
}

void wait(uint32_t ms){
	uint32_t TicksPre = msTicks;
	while((msTicks - TicksPre) < ms){
		asm("NOP");
	}
}



// print function for testing
void print(const char *msg){

	uint8_t i = 0;
	uint8_t TxBuf[63]; // message buffer

	// copy message
	while (*msg != 0){
		TxBuf[i] = *msg++;
		i++;
	}
	// for console
	TxBuf[i] = '\r';  // no diagonal printing
	i++;
	TxBuf[i] = '\n'; // => println()
	i++;

	usart_send(USART2, TxBuf, i);
}




void SysTick_Handler(void){

	// for system function's
	msTicks++;
}



uint32_t get_ticks(){
	return msTicks;
}

// for context switch => not used
void PendSV_Handler(void)
{

}


void TIM2_IRQHandler(void) {
	task_cntrl[1].task_func();
	static bool_t state = false;
	state = !state;
	gpio_set_val(GPIOB, GPIO_15, state); // frequency check

	 if (TIM2->SR & TIMER_SR_UIF) {
		 TIM2->SR &= ~TIMER_SR_UIF;
	 }
}

system_err_t init_system(){


	// Reset all used peripheral
	gpiox_reset();
	spix_reset();
	usartx_reset();
	adc_reset();


	RCC_CFGR = 0;
	RCC_CR &= ~RCC_CR_PLL_ON;

	RCC_CR |= RCC_CR_HSI16_ON;
	while (!(RCC_CR & RCC_CR_HSI16_RDY)){
		asm("NOP");
	}

	// set HSI16 as system-clock
	RCC_CFGR |= RCC_CFGR_SW_HSI16;

/*	Try to use pll as system clock =>    i'am at loss ;D

	// divide HSI16 by 4
	RCC_CR |= RCC_CR_HSI16_DIV_EN;
	while (!(RCC_CR & RCC_CR_HSI16_DIVF)){
		asm("NOP");
	}

	// select HSI16
	RCC_CFGR &= ~RCC_CFGR_PLLSRC;

	// set pll gain to factor 8
	RCC_CFGR |= (1 << 19);

	RCC_CFGR |= (1 << 22);

	while (RCC_CR & RCC_CR_PLL_RDY){
			asm("NOP");
	}

	RCC_CR |= RCC_CR_PLL_ON;

	// Systemclock => (16 Mhz / 4) * 16 / 2 = 32 Mhz
	RCC_CFGR |= RCC_CFGR_SW_PLL;
//	while (!(RCC_CFGR & RCC_CFGR_SWS_PLL)){
//		asm("NOP");
//	}
*/

	// usart2 for print - function
	usart_config_t usart_cfg;
	usart_cfg.clk_div      = 139;   // (Systemclock / Baudrate) => 16000000 / 115200
	usart_cfg.format       = USART_LSB_FIRST;
	usart_cfg.mode         = USART_MODE_TX;
	usart_cfg.parity       = USART_NO_PARITY;
	usart_cfg.word_len     = USART_WORD_LEN_8BIT;
	usart_cfg.stp_bit      = USART_STOP_1BIT;
	usart_cfg.flow_control = USART_HW_FLOW_CONTROL_DISABLE;
	usart_cfg.interrupt    = USART_INTERRUPT_DISABLE;
	usart_init(USART2, &usart_cfg);

	gpio_config_t gpio_cfg;
	gpio_cfg.pin         = GPIO_15;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_OUTPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);


	// Enable USART1 Interrupt
	NVIC_ISER |= ISER_USART1_IRQ;
	// Enable Timer2 Interrupt
	NVIC_ISER |= ISER_TIMER2_IRQ;

	// configuration timer 2
	TIM2->CR1 &= ~TIMER_CR1_CEN;

	// 5 ms
	RCC_APB1ENR |= RCC_APB1ENR_TIMER2;
	TIM2->PSC = 7999;
	TIM2->ARR = 9;
	TIM2->DIER  |= TIMER_DIER_UIE;

	// configuration SysTick
	STK->CSR = (uint32_t) 0x00;
	STK->RVR = (uint32_t) 15999;   //  => (16000000 / 1000) - 1
	STK->CVR = (uint32_t) 0x00;
	STK->CSR |= (SYSTICK_INTR_ENABLE | SYSTICK_ENABLE_PROCESSOR_CLOCK);
	STK->CSR |= SYSTICK_ENABLE;



	print("SYSTEM: Initialization success");


	return SYSTEM_OK;
}


