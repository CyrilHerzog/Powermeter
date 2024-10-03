/*
 * fifo.c
 *
 *  Created on: 26.05.2023
 *      Author: Herzog Cyril
 */

#include <fifo.h>



void fifo_init(fifo_t *pFifo, uint8_t *pBuffer, uint16_t size) {
	pFifo->pBuffer = pBuffer;
	pFifo->size    = size;
	pFifo->rd_ptr  = 0;
	pFifo->wr_ptr  = 0;
	pFifo->state   = FIFO_EMPTY;
}

void fifo_write(fifo_t *pFifo, uint8_t data) {
	uint16_t wr_ptr = pFifo->wr_ptr;
	if (pFifo->state != FIFO_FULL) {
		pFifo->pBuffer[wr_ptr] = data;
		wr_ptr++;
	}

	if (wr_ptr >= pFifo->size) {
		wr_ptr = 0;
	}

	pFifo->wr_ptr = wr_ptr;

	if ((wr_ptr + 1) == pFifo->rd_ptr) {
		pFifo->state = FIFO_FULL;
	} else {
		pFifo->state = FIFO_OK;
	}
}

uint8_t fifo_read(fifo_t *pFifo) {
	uint16_t rd_ptr = pFifo->rd_ptr;
	uint8_t data;
	if (pFifo->state != FIFO_EMPTY) {
		data = pFifo->pBuffer[rd_ptr];
		rd_ptr++;
	}

	if (rd_ptr >= pFifo->size) {
		rd_ptr = 0;
	}

	pFifo->rd_ptr = rd_ptr;

	if (rd_ptr == pFifo->wr_ptr) {
		pFifo->state = FIFO_EMPTY;
	} else {
		pFifo->state = FIFO_OK;
	}

	return data;
}

void fifo_clear(fifo_t *pFifo) {
	for (uint16_t i = 0; i < pFifo->size; i++) {
		pFifo->pBuffer[i] = 0;
	}
	pFifo->wr_ptr = 0;
	pFifo->rd_ptr = 0;
	pFifo->state  = FIFO_EMPTY;

}

