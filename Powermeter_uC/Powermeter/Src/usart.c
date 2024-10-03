/*
 * usart.c
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 *
 */


#include <usart.h>

#define USART_TIMEOUT (uint32_t)0x1000

usart_reg_t *USART1 = (usart_reg_t *) USART1_BASE;
usart_reg_t *USART2 = (usart_reg_t *) USART2_BASE;


void usartx_reset(){
	// uint32_t temp = (APBx_USART1 | APBx_USART6);
	RCC_APB2RSTR |= APBx_USART1;
	RCC_APB2RSTR &= ~APBx_USART1;

	// temp = (APBx_USART2 | APBx_USART3 | APBx_USART4 | APBx_USART5);

	RCC_APB1RSTR |= APBx_USART2;
	RCC_APB1RSTR &= ~APBx_USART2;
}


void usart_init(usart_reg_t *pDrv, usart_config_t *pConfig){

	gpio_pin_t pin_tx, pin_rx;
	gpio_pin_t pin_cts, pin_rts;

	pDrv->CR1 &= ~USART_CR1_UE;

	if (pDrv == USART1){
		RCC_APB2ENR 	|= APBx_USART1;
		pin_tx  = GPIO_9;
		pin_rx  = GPIO_10;
		pin_cts = GPIO_11;
		pin_rts = GPIO_12;
	}else{
		// Flow Control on USART2 not possible with the Test Board-Layout (Nucleo F070)
		RCC_APB1ENR  |= APBx_USART2;
		pin_tx = GPIO_2;
		pin_rx = GPIO_3;
		pin_cts = GPIO_0;
		pin_rts = GPIO_1;
	};

	gpio_config_t gpio_cfg;
	gpio_cfg.pin         = pin_tx;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF4;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
	gpio_config(GPIOA, &gpio_cfg);

	gpio_cfg.pin         = pin_rx;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF4;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
	gpio_config(GPIOA, &gpio_cfg);

	if (pConfig->flow_control == USART_HW_FLOW_CONTROL_ENABLE){
		gpio_cfg.pin         = pin_cts;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF4;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_config(GPIOA, &gpio_cfg);

		gpio_cfg.pin         = pin_rts;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF4;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_config(GPIOA, &gpio_cfg);
	}

	pDrv->CR1 = (pConfig->mode | pConfig->parity | pConfig->word_len | pConfig->interrupt);
	pDrv->BRR = pConfig->clk_div;
	pDrv->CR2 = (pConfig->format | pConfig->stp_bit);
	pDrv->CR3 = pConfig->flow_control;


	pDrv->CR1 |= (USART_CR1_UE);

}


system_err_t usart_send(usart_reg_t *pDrv, uint8_t *pTxBuf, uint8_t len){
	uint32_t ticks_pre = get_ticks();
	for(uint8_t i = 0; i < len; i++){
		while(!(pDrv->ISR & USART_ISR_TXE)){
		if((get_ticks() - ticks_pre) >= USART_TIMEOUT){
				return USART_SEND_TIMEOUT;
			}
		}
		pDrv->TDR = pTxBuf[i];
	}

//	while(!(pDrv->ISR & USART_ISR_TC)){
//		if((get_ticks() - ticks_pre) >= USART_TIMEOUT){
//			return USART_SEND_TIMEOUT;
//		}
//	}

	return SYSTEM_OK;
}


// this function is used in interrupt routine
system_err_t usart_receive(usart_reg_t *pDrv, uint8_t *pData){
	while(!(pDrv->ISR & USART_ISR_RXNE)){
		// time delay without SysTick, because USART ISR can have higher priority
		for (uint32_t j = 0; j < 44000; j++){
			asm("NOP");
		}
		return USART_RECEIVE_TIMEOUT;
	}
	*pData = (uint8_t)pDrv->RDR;
	// clear overrun flag
	pDrv->ICR |= USART_ICR_ORECF;
	return SYSTEM_OK;
}



system_err_t usart_send_string(usart_reg_t *pDrv, const char *string){
	uint8_t TxBuf[32];
	uint8_t len = 0;

	//copy
	while (*string != 0){
		TxBuf[len] = *string++;
		len++;
	}
	if (len > 15){return USART_SEND_STRING_OVERFLOW;}
	uint32_t ticks_pre = get_ticks();
		for(uint8_t i = 0; i < len; i++){
			while(!(pDrv->ISR & USART_ISR_TXE)){
				if((get_ticks() - ticks_pre) >= USART_TIMEOUT){
					return USART_SEND_TIMEOUT;
				}
			}
	pDrv->TDR = TxBuf[i];
		}
	while(!(pDrv->ISR & USART_ISR_TC)){
		if((get_ticks() - ticks_pre) >= USART_TIMEOUT){
			return USART_SEND_TIMEOUT;
		}
	}
	return SYSTEM_OK;
}

