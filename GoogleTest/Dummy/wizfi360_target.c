/*
 * required.c
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

/*********************************************************************************************/

/* User Includes ----------------------------------------------------------------------------*/

#include "wizfi360.h"

/*********************************************************************************************/
/* Imported variables  ----------------------------------------------------------------------*/


/*********************************************************************************************/
/* User Defines -----------------------------------------------------------------------------*/


/*********************************************************************************************/
/* User Variables ---------------------------------------------------------------------------*/

/*********************************************************************************************/
/* Implementation of required target-specific functions -------------------------------------*/

/**
  * @brief 	Starts the UART driver in continuous background reception (DMA + IDLE-Event).
  * @note	This is an example implementation of the required function.
  * @retval None
  */
void WIZFI360_UART_StartContinousReception()
{

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

}
#endif


/**
  * @brief 	Provides a minimum delay (in milliseconds).
  * @note	This is an example implementation of the required function.
  * @param 	Delay  specifies the delay time length, in milliseconds.
  * @retval None
  */
void WIZFI360_Delay(uint32_t Delay)
{

}

/**
  * @brief 	Sets the RST pin of the module to low.
  * @note	This is an example implementation of the required function.
  * @retval None
  */
void WIZFI360_WriteResetPinLow()
{

}

/**
  * @brief 	Sets the RST pin of the module to high.
  * @note	This is an example implementation of the required function.
  * @retval None
  */
void WIZFI360_WriteResetPinHigh()
{

}

/**
  * @brief 	This function aborts UART and disables UART RX before hard-reset.
  * @note	Writing the reset pin to low could mess up the STM32 UART drivers.
  * 		Thus, any UART operation is aborted and the UART reception is disabled,
  * 		before resetting the wizfi360 module.
  * @note	This is an example implementation of the required function.
  * @retval None
  */
void WIZFI360_PreResetHard()
{

}

/**
  * @brief 	This function re-enables UART RX after hard-reset.
  * @note	This is an example implementation of the required function.
  * @retval None
  */
void WIZFI360_PostResetHard()
{

}

/*********************************************************************************************/



/******************************************************************************/
