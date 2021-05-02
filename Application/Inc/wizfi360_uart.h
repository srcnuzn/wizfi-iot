/*
 * uart.h
 *
 *  Created on: 02.05.2021
 *      Author: serca
 */

#ifndef INC_WIZFI360_UART_H_
#define INC_WIZFI360_UART_H_

#include "main.h"
#include "ringbuffer.h"


void WIZFI360_UART_Initialize();
ring_buffer_size_t WIZFI360_UART_DataAvailable();
uint8_t WIZFI360_UART_PeekFromBuffer(uint8_t *data, ring_buffer_size_t index);
ring_buffer_size_t WIZFI360_UART_ReadFromBuffer(uint8_t *data, uint16_t len);
void WIZFI360_UART_SendBlockingMode(uint8_t* data, uint16_t len, uint16_t timeout);

#endif /* INC_WIZFI360_UART_H_ */
