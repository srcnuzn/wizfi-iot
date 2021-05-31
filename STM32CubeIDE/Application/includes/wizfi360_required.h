/*
 * wizfi360_required.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 *
 *  TODO: Add Comment for wizfi360_required.h
 *
 */

#ifndef INCLUDES_WIZFI360_REQUIRED_H_
#define INCLUDES_WIZFI360_REQUIRED_H_

/*********************************************************************************************/
/* Includes ---------------------------------------------------------------------------------*/

#include <stdint.h>

/*********************************************************************************************/
/* Target specific UART functions to be implemented -----------------------------------------*/

/*
 * @brief	Must start the UART in continous reception mode.
 * @note	This target-specific function must be implemented by user.
 */
extern void WIZFI360_UART_StartContinousReception();

/*
 * TODO: Comment on WIZFI360_UART_Stop
 */
extern void WIZFI360_UART_Stop();

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
/**
 * @brief	Must send an amount of data to WizFi360 module in interrupt mode.
 * @note	This target-specific function must be implemented by user.
 * @param	pData   Pointer to data buffer (u8 or u16 data elements).
 * @param	Size    Amount of data elements (u8 or u16) to be sent.
 * @retval	None
 */
extern void WIZFI360_UART_SendNonBlockingMode(uint8_t* pData, uint16_t Size);
#else
/**
 * @brief	Must send an amount of data to WizFi360 module in blocking mode.
 * @note	This target-specific function must be implemented by user.
 * @param	pData   Pointer to data buffer (u8 or u16 data elements).
 * @param	Size    Amount of data elements (u8 or u16) to be sent.
 * @retval	None
 */
extern void WIZFI360_UART_SendBlockingMode(uint8_t* pData, uint16_t Size, uint16_t Timeout);
#endif

/*********************************************************************************************/
/* Target specific Reset functions to be implemented ----------------------------------------*/

/**
  * @brief 	Must set the RST pin of the module to low.
  * @note	This target-specific function must be implemented by user.
  * @retval None
  */
extern void WIZFI360_WriteResetPinLow();

/**
  * @brief 	Must set the RST pin of the module to high.
  * @note	This target-specific function must be implemented by user.
  * @retval None
  */
extern void WIZFI360_WriteResetPinHigh();

/**
  * @brief 	This function executes before hard-reset is performed.
  * @note	Writing the reset pin to low could lead to errors on UART lines.
  * 		Thus, any UART operation should be aborted and the UART reception
  * 		should be disabled,	before resetting the wizfi360 module.
  * @note	This target-specific function must be implemented by user.
  * @retval None
  */
extern void WIZFI360_PreResetHard();

/**
  * @brief 	This function executes after hard-reset is performed.
  * @note	In this function, UART could be re-enabled, if it was disabled
  * 		before hard-reset.
  * @note	This target-specific function must be implemented by user.
  * @retval None
  */
extern void WIZFI360_PostResetHard();

/**
  * @brief 	Must provide a delay (in milliseconds).
  * @note	This target-specific function must be implemented by user.
  * @param 	Delay  specifies the delay time length, in milliseconds.
  * @retval None
  */
extern void WIZFI360_Delay(uint32_t Delay);


#endif /* INCLUDES_WIZFI360_REQUIRED_H_ */
