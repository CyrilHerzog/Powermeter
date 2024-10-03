/*
 * fifo.h
 *
 *	First In - First Out for a Ringbuffer with Elements of size 8Bit
 *
 *  Created on: 26.05.2023
 *      Author: Herzog Cyril
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>

typedef enum {
	FIFO_OK    = 0,
	FIFO_FULL  = 1,
	FIFO_EMPTY = 2
}fifo_state_t;

typedef struct {
	uint8_t *pBuffer;
	uint16_t size;
	uint16_t wr_ptr;
	uint16_t rd_ptr;
	fifo_state_t state;
}fifo_t;


void fifo_init(fifo_t *pFifo, uint8_t *pBuffer, uint16_t size);

void fifo_write(fifo_t *pFifo, uint8_t data);

uint8_t fifo_read(fifo_t *pFifo);

void fifo_clear(fifo_t *pFifo);



#endif /* FIFO_H_ */
