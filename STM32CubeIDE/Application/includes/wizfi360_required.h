/*
 * wizfi360_required.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

#ifndef INCLUDES_WIZFI360_REQUIRED_H_
#define INCLUDES_WIZFI360_REQUIRED_H_

/*********************************************************************************************/
/* Target specific UART functions to be implemented -----------------------------------------*/

/*
 * TODO: Comment on WIZFI360_UART_StartContinousReception
 */
extern void WIZFI360_UART_StartContinousReception();

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
/*
 * TODO: Comment on WIZFI360_UART_SendNonBlockingMode
 */
extern void WIZFI360_UART_SendNonBlockingMode(uint8_t* pData, uint16_t Size);
#else
/*
 * TODO: Comment on WIZFI360_UART_SendBlockingMode
 */
extern void WIZFI360_UART_SendBlockingMode(uint8_t* pData, uint16_t Size, uint16_t Timeout);
#endif

/*********************************************************************************************/
/* Target specific Reset functions to be implemented ----------------------------------------*/

/*
 * TODO: Comment on WIZFI360_WriteResetPinLow
 */
extern void WIZFI360_WriteResetPinLow();

/*
 * TODO: Comment on WIZFI360_WriteResetPinHigh
 */
extern void WIZFI360_WriteResetPinHigh();

/*
 * TODO: Comment on WIZFI360_PreResetHard
 */
extern void WIZFI360_PreResetHard();

/*
 * TODO: Comment on WIZFI360_PostResetHard
 */
extern void WIZFI360_PostResetHard();

/*
 * TODO: Comment on WIZFI360_Delay
 */
extern void WIZFI360_Delay(uint32_t Delay);

/*********************************************************************************************/
/* User specific callback functions to be implemented ---------------------------------------*/

#ifdef WIZFI360_CALLBACK_USED_COMMAND_COMPLETE
/*
 * TODO: Comment on WIZFI360_CommandCpltCallback
 */
extern void WIZFI360_CommandCpltCallback(WIZFI360_CommandIdTypeDef command,
		WIZFI360_ResponseTypeDef response);
#endif

#ifdef WIZFI360_CALLBACK_USED_WIFI_CONNECTED
/*
 * TODO: Comment on WIZFI360_WifiConnectedCallback
 */
extern void WIZFI360_WifiConnectedCallback();
#endif

#ifdef WIZFI360_CALLBACK_USED_WIFI_DISCONNECTED
/*
 * TODO: Comment on WIZFI360_WifiDisconnectedCallback
 */
extern void WIZFI360_WifiDisconnectedCallback();
#endif

#ifdef WIZFI360_CALLBACK_USED_WIFI_CONNECT_FAILED
/*
 * TODO: Comment on WIZFI360_WifiConnectFailedCallback
 */
extern void WIZFI360_WifiConnectFailedCallback();
#endif

#ifdef WIZFI360_CALLBACK_USED_MODULE_READY
/*
 * TODO: Comment on WIZFI360_ModuleReadyCallback
 */
extern void WIZFI360_ModuleReadyCallback();
#endif

/*
 * TODO: Comment on WIZFI360_RegisterSubscribeCallbacks
 */
extern void WIZFI360_RegisterSubscribeCallbacks();

/*********************************************************************************************/


#endif /* INCLUDES_WIZFI360_REQUIRED_H_ */
