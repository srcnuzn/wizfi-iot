/*
 * test.c
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

#include "test.h"
#include "ringbuffer.h"

#define RX_BUFFER_SIZE 32
#define send(X) (HAL_UART_Transmit(&huart2, (uint8_t*)(X), sizeof((X))-1, 10000))

uint8_t aRXBufferUser[RX_BUFFER_SIZE];

const char* WIZFI360TAGS[] =
{
	"\r\nERROR\r\n",
	"\r\nALREADY CONNECTED\r\n",
	"\r\nSEND FAIL\r\n",
    "\r\nOK\r\n",
    "\r\nSEND OK\r\n",
	"\r\nready\r\n",
	"\r\nWIFI CONNECTED\r\n",
	"\r\nWIFI GOT IP\r\n",
	"\r\nWIFI DISCONNECTED\r\n",
	"\r\nbusy s...\r\n",
	"\r\nbusy p...\r\n",
};

uint8_t MATCH_COUNTER[11] = {0};


/* Create ring buffer */
ring_buffer_t uart2_rbuf;

/**
 * @brief Data buffers used to manage received data in interrupt routine
 */
uint8_t aRXBufferA[RX_BUFFER_SIZE];
uint8_t aRXBufferB[RX_BUFFER_SIZE];

__IO uint32_t uwNbReceivedChars;
uint8_t *pBufferReadyForUser;
uint8_t *pBufferReadyForReception;

__IO uint8_t doStep;

static void Init();

void SuccessCallback();

void StartReception(void);
void UserDataTreatment(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

void testInit()
{
	/* Initiate Continuous reception */
	StartReception();
	Init();
}


void testRun()
{
	if (doStep)
	{
		static uint8_t ctr = 0;

		if (++ctr == 4)
		{
			send("AT+GMR\r\n");
			ctr = 0;
		}
		doStep = 0;
	}
}


static void Init()
{
	HAL_TIM_Base_Start_IT(&htim2);
}


/**
 * @brief  This function handles buffer containing received data
 * @note   Any other processing such as copying received data in a larger buffer to make it
 *         available for application, could be implemented here.
 * @note   This routine is executed in Interrupt context.
 * @param  huart UART handle.
 * @param  pData Pointer on received data buffer to be processed
 * @retval Size  Nb of received characters available in buffer
 */
void UserDataTreatment(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	/*
	 * This function might be called in any of the following interrupt contexts :
	 *  - DMA TC and HT events
	 *  - UART IDLE line event
	 *
	 * pData and Size defines the buffer where received data have been copied, in order to be processed.
	 * During this processing of already received data, reception is still ongoing.
	 *
	 */
	__NOP();

	for (int i = 0; i < Size; i++)
	{
		ring_buffer_queue(&uart2_rbuf, pData[i]);

		for (int j = 0; j < 10; j++)
		{
			if (pData[i] == WIZFI360TAGS[j][MATCH_COUNTER[j]])
			{
				MATCH_COUNTER[j]++;
				if (MATCH_COUNTER[j] == sizeof(WIZFI360TAGS[j]) - 1)
				{
					SuccessCallback(j);
					MATCH_COUNTER[j] = 0;
				}
			}
			else if (pData[i] == WIZFI360TAGS[j][0])
			{
				MATCH_COUNTER[j] = 1;
			}
			else
			{
				MATCH_COUNTER[j] = 0;
			}
		}
	}
	__NOP();
}


void SuccessCallback(int tag)
{
	__NOP();
}





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
			uwNbReceivedChars = RX_BUFFER_SIZE - old_pos;
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
		UserDataTreatment(huart, pBufferReadyForUser, uwNbReceivedChars);

		/* Swap buffers for next bytes to be processed */
		ptemp = pBufferReadyForUser;
		pBufferReadyForUser = pBufferReadyForReception;
		pBufferReadyForReception = ptemp;
	}
	/* Update old_pos as new reference of position in User Rx buffer that
	 indicates position to which data have been processed */
	old_pos = Size;
}

/**
 * @brief  This function prints user info on PC com port and initiates RX transfer
 * @retval None
 */
void StartReception(void)
{
	/* Initialize ring buffer */
	ring_buffer_init(&uart2_rbuf);

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
	if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart2, aRXBufferUser,
	RX_BUFFER_SIZE))
	{
		Error_Handler();
	}
}

/**
 * @brief  Period elapsed callback in non-blocking mode
 * @param  htim TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		if (doStep == 1)
		{
			//RealTime Error (interrupt während Ausführung getriggert)
			__NOP();
		}
		doStep = 1;
	}
	else
	{
		__NOP();
	}
}
