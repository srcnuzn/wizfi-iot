/*
 * required.c
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

/*********************************************************************************************/

/* User Includes ----------------------------------------------------------------------------*/

#include "main.h"
#include "wizfi360.h"

/*********************************************************************************************/
/* Imported variables  ----------------------------------------------------------------------*/

// The UART handler used for WizFi360 module communication.
extern UART_HandleTypeDef huart2;

/*********************************************************************************************/
/* User Defines -----------------------------------------------------------------------------*/

#define RX_DMA_BUFFER_SIZE 32

/*********************************************************************************************/
/* User Variables ---------------------------------------------------------------------------*/

static uint8_t aRXBufferUser[RX_DMA_BUFFER_SIZE];
static uint8_t aRXBufferA[RX_DMA_BUFFER_SIZE];
static uint8_t aRXBufferB[RX_DMA_BUFFER_SIZE];

static __IO  uint32_t uwNbReceivedChars;
static uint8_t *pBufferReadyForUser;
static uint8_t *pBufferReadyForReception;


// A pointer to the UART handler used for WizFi360 module communication.
static UART_HandleTypeDef* pWizFi360_huart = &huart2;

/*********************************************************************************************/
/* Required Functions -----------------------------------------------------------------------
 *
 *
 *	TODO: Comment on required function implementation
 *
 *
 *
 */

/*
 *	TODO: Comment on WIZFI360_UART_StartContinousReception
 */
void WIZFI360_UART_StartContinousReception()
{
	// Abort ongoing UART reception
	HAL_UART_AbortReceive(pWizFi360_huart);

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
}

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
/**
 * @brief	Send an amount of data to WizFi360 module in interrupt mode.
 * @note	This function contains target specific code sections.
 * @param	pData   Pointer to data buffer (u8 or u16 data elements).
 * @param	Size    Amount of data elements (u8 or u16) to be sent.
 * @retval	None
 */
void WIZFI360_UART_SendNonBlockingMode(uint8_t* pData, uint16_t Size, uint16_t Timeout)
{
	if (HAL_OK != HAL_UART_Transmit_IT(pWizFi360_huart, pData, Size))
	{
		// TODO: ErrorHandling
		__NOP();
	}
}
#else
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
	if (HAL_OK != HAL_UART_Transmit(pWizFi360_huart, pData, Size, Timeout))
	{
		// TODO: ErrorHandling
		__NOP();
	}
}
#endif

/*
 *	TODO: Comment on WIZFI360_Delay
 */
void WIZFI360_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}


/*
 *	TODO: Comment on WIZFI360_WriteResetPinLow
 */
void WIZFI360_WriteResetPinLow()
{
	HAL_GPIO_WritePin(WIZFI360_RST_GPIO_Port, WIZFI360_RST_Pin, GPIO_PIN_RESET);
}

/*
 *	TODO: Comment on WIZFI360_WriteResetPinHigh
 */
void WIZFI360_WriteResetPinHigh()
{
	HAL_GPIO_WritePin(WIZFI360_RST_GPIO_Port, WIZFI360_RST_Pin, GPIO_PIN_SET);
}

/*
 *	TODO: Comment on WIZFI360_PreResetHard
 */
void WIZFI360_PreResetHard()
{
	// Abort ongoing UART transmission
	HAL_UART_Abort(pWizFi360_huart);
	// Abort ongoing UART reception
	HAL_UART_AbortReceive(pWizFi360_huart);

	// Disable UART reception
	(pWizFi360_huart)->Instance->CR1 &= ~USART_CR1_RE;
}

/*
 *	TODO: Comment on WIZFI360_PostResetHard
 */
void WIZFI360_PostResetHard()
{
	// Enable UART reception
	(pWizFi360_huart)->Instance->CR1 |= USART_CR1_RE;

	// Restart continous UART reception
	WIZFI360_UART_StartContinousReception();
}


/******************************************************************************/

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
		WIZFI360_UART_BytesReceived((char*) pBufferReadyForUser, uwNbReceivedChars);

		/* Swap buffers for next bytes to be processed */
		ptemp = pBufferReadyForUser;
		pBufferReadyForUser = pBufferReadyForReception;
		pBufferReadyForReception = ptemp;
	}
	/* Update old_pos as new reference of position in User Rx buffer that
	 indicates position to which data have been processed */
	old_pos = Size;
}


uint32_t uart_err = 0;
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	uart_err++;
}

/******************************************************************************/
