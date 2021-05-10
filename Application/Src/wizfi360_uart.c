/*
 * uart.c
 *
 *  Created on: 02.05.2021
 *      Author: serca
 */

#include <wizfi360_uart.h>

/******************************************************************************/

// Target specific code section starts here....

#define RX_DMA_BUFFER_SIZE 128

uint8_t aRXBufferUser[RX_DMA_BUFFER_SIZE];
uint8_t aRXBufferA[RX_DMA_BUFFER_SIZE];
uint8_t aRXBufferB[RX_DMA_BUFFER_SIZE];

__IO uint32_t uwNbReceivedChars;
uint8_t *pBufferReadyForUser;
uint8_t *pBufferReadyForReception;


/**
  * @brief 	A pointer to the UART handler used for WizFi360 module communication.
  * @note   huart2 is exported in main.h
  */
UART_HandleTypeDef* pWizFi360_huart = &huart2;

// ... target specific code section ends here

/******************************************************************************/


/**
  * @brief Ring buffer for UART data reception
  */
ring_buffer_t rbuffer;


/**
 * @brief  This function initializes the UART interface in continuous reception mode.
 * @note   This function contains target specific code sections.
 * @retval None
 */
void WIZFI360_UART_Initialize()
{
	/* Initialize ring buffer */
	ring_buffer_init(&rbuffer);

	// Target specific code section starts here....

	/* Initializes Buffer swap mechanism (used in User callback) :
	 - 2 physical buffers aRXBufferA and aRXBufferB (RX_BUFFER_SIZE length)
	 */
	pBufferReadyForReception = aRXBufferA;
	pBufferReadyForUser = aRXBufferB;
	uwNbReceivedChars = 0;

	/* Initializes Rx sequence using Reception To Idle event API.
	 As DMA channel associated to UART Rx is configured as Circular,
	 reception is endless.
	 If reception has to be stopped, call to HAL_UART_AbortReceive() could be used.
	 Use of HAL_UARTEx_ReceiveToIdle_DMA service, will generate calls to
	 user defined HAL_UARTEx_RxEventCallback callback for each occurrence of
	 following events :
	 - DMA RX Half Transfer event (HT)
	 - DMA RX Transfer Complete event (TC)
	 - IDLE event on UART Rx line (indicating a pause is UART reception flow)
	 */
	if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(pWizFi360_huart, aRXBufferUser,
	RX_DMA_BUFFER_SIZE))
	{
		// TODO: Error handling
	}

	// ... target specific code section ends here
}

/**
 * @brief  Returns the number of items in UART ring buffer.
 * @return The number of items in UART ring buffer.
 */
ring_buffer_size_t WIZFI360_UART_DataAvailable()
{
	return ring_buffer_num_items(&rbuffer);
}

/**
 * @brief	Send an amount of data to WizFi360 module in blocking mode.
 * @note	This function contains target specific code sections.
 * @param	pData   Pointer to data buffer (u8 or u16 data elements).
 * @param	Size    Amount of data elements (u8 or u16) to be sent.
 * @param	Timeout Timeout duration.
 * @retval	None
 */
void WIZFI360_UART_SendBlockingMode(uint8_t* pData, uint16_t Size, uint16_t Timeout)
{
	// Target specific code section starts here....

	if (HAL_OK != HAL_UART_Transmit(pWizFi360_huart, pData, Size, Timeout))
	{
		// TODO: ErrorHandling
	}

	// ... target specific code section ends here
}

/**
 * Returns the <em>len</em> oldest bytes in  UART ring buffer.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of bytes to return.
 * @return The number of bytes returned.
 */
ring_buffer_size_t WIZFI360_UART_ReadArrayFromBuffer(uint8_t *data, uint16_t len)
{
	return ring_buffer_dequeue_arr(&rbuffer, (char*) data, len);
}

/**
 * Returns the oldest byte in  UART ring buffer.
 * @param data A pointer to the location at which the data should be placed.
 * @return 1 if data was returned; 0 otherwise.
 */
uint8_t WIZFI360_UART_ReadByteFromBuffer(uint8_t *data)
{
	return ring_buffer_dequeue(&rbuffer, (char*) data);
}

/**
 * Peeks the UART ring buffer, i.e. returns an element without removing it.
 * @param data A pointer to the location at which the data should be placed.
 * @param index The index to peek.
 * @return 1 if data was returned; 0 otherwise.
 */
uint8_t WIZFI360_UART_PeekFromBuffer(uint8_t *data, ring_buffer_size_t index)
{
	return ring_buffer_peek(&rbuffer, (char*)data, index);
}


/******************************************************************************/

// Target specific code section starts here....

/**
 * @brief  User implementation of the Reception Event Callback
 *         (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	static uint8_t old_pos = 0;
	uint8_t *ptemp;
	uint8_t i;

	/* Check if number of received data in recpetion buffer has changed */
	if (Size != old_pos)
	{
		/* Check if position of index in reception buffer has simply be increased
		 of if end of buffer has been reached */
		if (Size > old_pos)
		{
			/* Current position is higher than previous one */
			uwNbReceivedChars = Size - old_pos;
			/* Copy received data in "User" buffer for evacuation */
			for (i = 0; i < uwNbReceivedChars; i++)
			{
				pBufferReadyForUser[i] = aRXBufferUser[old_pos + i];
			}
		}
		else
		{
			/* Current position is lower than previous one : end of buffer has been reached */
			/* First copy data from current position till end of buffer */
			uwNbReceivedChars = RX_DMA_BUFFER_SIZE - old_pos;
			/* Copy received data in "User" buffer for evacuation */
			for (i = 0; i < uwNbReceivedChars; i++)
			{
				pBufferReadyForUser[i] = aRXBufferUser[old_pos + i];
			}
			/* Check and continue with beginning of buffer */
			if (Size > 0)
			{
				for (i = 0; i < Size; i++)
				{
					pBufferReadyForUser[uwNbReceivedChars + i] =
							aRXBufferUser[i];
				}
				uwNbReceivedChars += Size;
			}
		}
		/* Process received data that has been extracted from Rx User buffer */
		ring_buffer_queue_arr(&rbuffer, (char*) pBufferReadyForUser, uwNbReceivedChars);

		/* Swap buffers for next bytes to be processed */
		ptemp = pBufferReadyForUser;
		pBufferReadyForUser = pBufferReadyForReception;
		pBufferReadyForReception = ptemp;
	}
	/* Update old_pos as new reference of position in User Rx buffer that
	 indicates position to which data have been processed */
	old_pos = Size;
}

// ... target specific code section ends here

/******************************************************************************/
