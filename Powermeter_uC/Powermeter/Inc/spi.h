/*
 * spi.h
 *
 *  Created on: 04.04.2023
 *      Author: cyril
 *
 *      only for Master Mode
 */

#ifndef SPI_H_
#define SPI_H_

#include <system.h>
#include <gpio.h>


#define APBx_SPI1    		(uint32_t) 0x1000
#define APBx_SPI2    		(uint32_t) 0x4000

#define SPI_CR_CPHA			(uint32_t) 0x0001
#define SPI_CR_CPOL			(uint32_t) 0x0002
#define SPI_CR_MSTR			(uint32_t) 0x0004
#define SPI_CR_SSM			(uint32_t) 0x0200
#define SPI_CR_SSI			(uint32_t) 0x0100
#define SPI_CR_BIDIMODE		(uint32_t) 0x8000
#define SPI_CR_SPE   		(uint32_t) 0x0040
#define SPI_CR_LSBFIRST     (uint32_t) 0x0080
#define SPI_CR_DS_8BIT		(uint32_t) 0x0700

#define SPI_SR_RXNE         (uint32_t) 0x01
#define SPI_SR_TXE			(uint32_t) 0x02
#define SPI_SR_BSY			(uint32_t) 0x80



typedef enum{
	SPI_MSB_FIRST = 0,
	SPI_LSB_FIRST = SPI_CR_LSBFIRST
}spi_frame_format_t;

typedef enum{
	SPI_MODE_0 = 0,
	SPI_MODE_1 = SPI_CR_CPHA,
	SPI_MODE_2 = SPI_CR_CPOL,
	SPI_MODE_3 = SPI_CR_CPOL | SPI_CR_CPHA
}spi_mode_t;

typedef enum{
	SPI_DIV2   = 0,
	SPI_DIV4   = 1,
	SPI_DIV8   = 2,
	SPI_DIV16  = 3,
	SPI_DIV32  = 4,
	SPI_DIV64  = 5,
	SPI_DIV128 = 6,
	SPI_DIV256 = 7
}spi_clk_div_t;

typedef struct{
	spi_mode_t mode;
	spi_clk_div_t clk_div;
	spi_frame_format_t frame_format;
}spi_config_t;

extern spi_reg_t *SPI1;
extern spi_reg_t *SPI2;

void spix_reset();

void spi_init(spi_reg_t *pDrv, spi_config_t *pConfig);

system_err_t spi_write(spi_reg_t *pDrv, uint8_t *pTxBuf, uint8_t len);

system_err_t spi_transaction(spi_reg_t *pDrv, uint8_t *pTxBuf, uint8_t *pRxBuf, uint8_t len);


#endif /* SPI_H_ */
