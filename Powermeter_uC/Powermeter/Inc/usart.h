/*
 * usart.h
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 */

#ifndef USART_H_
#define USART_H_

#include <system.h>
#include <gpio.h>


#define APBx_USART1		       (uint32_t)0x00004000
#define APBx_USART2		       (uint32_t)0x00020000
#define APBx_USART3		       (uint32_t)0x00040000
#define APBx_USART4		       (uint32_t)0x00080000
#define APBx_USART5		  	   (uint32_t)0x00100000
#define APBx_USART6		  	   (uint32_t)0x00000020

#define USART_CR1_UE           (uint32_t)0x00000001
#define USART_CR1_RE		   (uint32_t)0x00000004
#define USART_CR1_TE		   (uint32_t)0x00000008
#define USART_CR1_IDLEIE       (uint32_t)0x00000010
#define USART_CR1_RXNEIE       (uint32_t)0x00000020
#define USART_CR1_TCIE         (uint32_t)0x00000040
#define USART_CR1_TXEIE        (uint32_t)0x00000080
#define USART_CR1_PEIE         (uint32_t)0x00000100
#define USART_CR1_PS		   (uint32_t)0x00000200
#define USART_CR1_PCE		   (uint32_t)0x00000400
#define USART_CR1_M0           (uint32_t)0x00001000
#define USART_CR1_M1           (uint32_t)0x10000000

#define USART_CR2_STOP0		   (uint32_t)0x00001000
#define USART_CR2_STOP1        (uint32_t)0x00002000
#define USART_CR2_MSBFIRST     (uint32_t)0x00080000
#define USART_CR2_CTSIE        (uint32_t)0x00000400

#define USART_CR3_RTSE         (uint32_t)0x00000100
#define USART_CR3_CTSE		   (uint32_t)0x00000200

#define USART_ISR_RXNE         (uint32_t)0x00000020
#define USART_ISR_TC		   (uint32_t)0x00000040
#define USART_ISR_TXE          (uint32_t)0x00000080
#define USART_ISR_CTS          (uint32_t)0x00000400
#define USART_ISR_BUSY         (uint32_t)0x00010000

#define USART_ICR_PECF		   (uint32_t)0x00000001
#define USART_ICR_FECF		   (uint32_t)0x00000002
#define USART_ICR_NCF		   (uint32_t)0x00000004
#define USART_ICR_ORECF        (uint32_t)0x00000008
#define USART_ICR_IDLECF       (uint32_t)0x00000010
#define USART_ICR_TCCF         (uint32_t)0x00000040
#define USART_ICR_CTSCF        (uint32_t)0x00000200
#define USART_ICR_RTOCF        (uint32_t)0x00000800
#define USART_ICR_CMCF         (uint32_t)0x00020000

typedef enum{
	USART_WORD_LEN_8BIT = 0,
	USART_WORD_LEN_9BIT = USART_CR1_M0,
	USART_WORD_LEN_7BIT = USART_CR1_M1
}usart_word_len_t;

typedef enum{
	USART_STOP_1BIT = 0,
	USART_STOP_2BIT = USART_CR2_STOP1
}usart_stp_bit_t;

typedef enum{
	USART_NO_PARITY   = 0,
	USART_EVEN_PARITY = USART_CR1_PCE,
	USART_ODD_PARITY  = USART_CR1_PCE | USART_CR1_PS
}usart_parity_t;

typedef enum{
	USART_MODE_TX    = USART_CR1_TE,
	USART_MODE_RX    = USART_CR1_RE,
	USART_MODE_TX_RX = (USART_CR1_TE | USART_CR1_RE)
}usart_mode_t;

typedef enum{
	USART_LSB_FIRST = 0,
	USART_MSB_FIRST = USART_CR2_MSBFIRST
}usart_format_t;

typedef enum{
	USART_HW_FLOW_CONTROL_DISABLE = 0,
	USART_HW_FLOW_CONTROL_ENABLE  = (USART_CR3_CTSE | USART_CR3_RTSE)
}usart_flow_ctrl_t;

typedef enum{
	USART_INTERRUPT_DISABLE     = 0,
	USART_RX_INTERRUPT_ENABLE   = USART_CR1_RXNEIE,
	USART_TX_INTERRUPT_ENABLE   = USART_CR1_TXEIE,
	USART_TC_INTERRUPT_ENABLE   = USART_CR1_TCIE,
	USART_IDLE_INTERRUPT_ENABLE = USART_CR1_IDLEIE,
	USART_PE_INTERRUPT_ENABLE   = USART_CR1_PEIE
}usart_interrupt_t;

typedef struct{
	uint32_t clk_div;
	usart_mode_t mode;
	usart_word_len_t word_len;
	usart_parity_t parity;
	usart_stp_bit_t stp_bit;
    usart_format_t format;
    usart_flow_ctrl_t flow_control;
    usart_interrupt_t interrupt;
}usart_config_t;


extern usart_reg_t *USART1;
extern usart_reg_t *USART2;

void usartx_reset();

void usart_init(usart_reg_t *pDrv, usart_config_t *pConfig);

system_err_t usart_send(usart_reg_t *pDrv, uint8_t *pTxBuf, uint8_t len);

system_err_t usart_receive(usart_reg_t *pDrv, uint8_t *pData);
//system_err_t usart_receive(usart_reg_t *pDrv, uint8_t *pRxBuf, uint8_t *pIndex);

system_err_t usart_send_string(usart_reg_t *pDrv, const char *string);

void usart_enable_interrupt(usart_reg_t *pDrv, uint32_t mask);

#endif /* USART_H_ */
