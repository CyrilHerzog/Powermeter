/*
 * spi.c
 *
 *  Created on: 08.04.2023
 *      Author: cyril
 */

#include <spi.h>

#define SPI_TIMEOUT    (uint32_t) 0x50 // anpassen

spi_reg_t *SPI1 = (spi_reg_t *) SPI1_BASE;
spi_reg_t *SPI2 = (spi_reg_t *) SPI2_BASE;


void spix_reset(){
	RCC_APB2RSTR |= APBx_SPI1;
	RCC_APB2RSTR &= ~APBx_SPI1;
	RCC_APB1RSTR |= APBx_SPI2;
	RCC_APB1RSTR &= ~APBx_SPI2;
}

void spi_init(spi_reg_t *pDrv, spi_config_t *pConfig){
	gpio_config_t gpio_cfg;

	pDrv->CR1 &= ~SPI_CR_SPE;

	if(pDrv == SPI1){
		RCC_APB2ENR |= APBx_SPI1;
		// sck
		gpio_cfg.pin         = GPIO_5;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF0;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_config(GPIOA, &gpio_cfg);
		// miso
		gpio_cfg.pin         = GPIO_6;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF0;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_config(GPIOA, &gpio_cfg);
		// mosi
		gpio_cfg.pin         = GPIO_7;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF0;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_config(GPIOA, &gpio_cfg);
	}else{
		RCC_APB1ENR |= APBx_SPI2;
		// sck
		gpio_cfg.pin         = GPIO_13;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF0;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_config(GPIOB, &gpio_cfg);
		// miso
		gpio_cfg.pin         = GPIO_14;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF0;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_config(GPIOB, &gpio_cfg);
		// mosi
		gpio_cfg.pin         = GPIO_15;
		gpio_cfg.func_mode   = GPIO_FUNC_MODE_AF0;
		gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
		gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
		gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
		gpio_config(GPIOB, &gpio_cfg);
	}

	// configure CR1
	pDrv->CR1 = (pConfig->mode | SPI_CR_MSTR | SPI_CR_SSM | SPI_CR_SSI);
	pDrv->CR1 |= ((pConfig->clk_div << 0x3) | pConfig->frame_format);

	/* update F-Serie <=> L-Serie
	configure CR2
	pDrv->CR2 = SPI_CR_DS_8BIT;
	*/

	// start spi
	pDrv->CR1 |= SPI_CR_SPE;
}


system_err_t spi_write(spi_reg_t *pDrv, uint8_t *pTxBuf, uint8_t len){
	uint32_t ticks_pre = get_ticks();

	pDrv->CR1 |= SPI_CR_SPE;
	for(uint8_t i = 0; i < len; i++){
		while(!(pDrv->SR & SPI_SR_TXE)){
			if((get_ticks() - ticks_pre) >= SPI_TIMEOUT){
				return SPI_WRITE_TIMEOUT;
			}
		}
		*(uint8_t *)&(pDrv->DR) = pTxBuf[i];
	}
	while(pDrv->SR & SPI_SR_BSY){
		if((get_ticks() - ticks_pre) >= SPI_TIMEOUT){
			return SPI_WRITE_TIMEOUT;
		}
	};
	return SYSTEM_OK;
}

system_err_t spi_transaction(spi_reg_t *pDrv, uint8_t *pTxBuf, uint8_t *pRxBuf, uint8_t len){
	uint32_t ticks_pre = get_ticks();

	for (uint32_t i = 0; i < len; i++){
		while(!(pDrv->SR & SPI_SR_TXE)){
			if((get_ticks() - ticks_pre) >= SPI_TIMEOUT){
				return SPI_TRANSACTION_TIMEOUT;
			}
		}
		*(uint8_t *)&pDrv->DR = pTxBuf[i];

		while(!(pDrv->SR & SPI_SR_RXNE)){
			if((get_ticks() - ticks_pre) >= SPI_TIMEOUT){
				return SPI_TRANSACTION_TIMEOUT;
			}
		}

		pRxBuf[i] = *(uint8_t *)&pDrv->DR;
		//uint32_t temp = pDrv->SR; => reset overflow flag
	}
	while(pDrv->SR & SPI_SR_BSY){
		if((get_ticks() - ticks_pre) >= SPI_TIMEOUT){
			return SPI_TRANSACTION_TIMEOUT;
		}
	}
	return SYSTEM_OK;
}



