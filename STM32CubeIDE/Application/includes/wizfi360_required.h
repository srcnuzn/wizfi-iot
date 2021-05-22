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

extern void WIZFI360_UART_StartContinousReception();

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
extern void WIZFI360_UART_SendNonBlockingMode(uint8_t* pData, uint16_t Size);
#else
extern void WIZFI360_UART_SendBlockingMode(uint8_t* pData, uint16_t Size, uint16_t Timeout);
#endif

/*********************************************************************************************/
/* Target specific Reset functions to be implemented ----------------------------------------*/

extern void WIZFI360_WriteResetPinLow();

extern void WIZFI360_WriteResetPinHigh();

extern void WIZFI360_PreResetHard();

extern void WIZFI360_PostResetHard();

extern void WIZFI360_Delay(uint32_t Delay);

/*********************************************************************************************/
/* User specific callback functions to be implemented ---------------------------------------*/

#ifdef WIZFI360_CALLBACK_USED_COMMAND_COMPLETE
extern void WIZFI360_CommandCpltCallback(WIZFI360_CommandIdTypeDef command,
		WIZFI360_ResponseTypeDef response);
#endif

#ifdef WIZFI360_CALLBACK_USED_WIFI_CONNECTED
extern void WIZFI360_WifiConnectedCallback();
#endif

#ifdef WIZFI360_CALLBACK_USED_WIFI_DISCONNECTED
extern void WIZFI360_WifiDisconnectedCallback();
#endif

#ifdef WIZFI360_CALLBACK_USED_WIFI_CONNECT_FAILED
extern void WIZFI360_WifiConnectFailedCallback();
#endif

#ifdef WIZFI360_CALLBACK_USED_MODULE_READY
extern void WIZFI360_ModuleReadyCallback();
#endif

/*********************************************************************************************/


#endif /* INCLUDES_WIZFI360_REQUIRED_H_ */
