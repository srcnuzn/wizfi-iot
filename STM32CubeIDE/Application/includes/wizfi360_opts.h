/*
 * wizfi360_opts.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

#ifndef INCLUDES_WIZFI360_OPTS_H_
#define INCLUDES_WIZFI360_OPTS_H_

/*********************************************************************************************/
/*
 * Choose which WizFi Board is used
 */
#define WIZFI360_EVB_MINI
// #define WIZFI360_EVB_SHIELD

/*********************************************************************************************/

#define WIZFI360_MAX_CMD_LEN 	256

/*********************************************************************************************/

#define WIZFI360_UART_TX_MODE_NON_BLOCKING

/*********************************************************************************************/

#define WIZFI360_CALLBACK_USED_COMMAND_COMPLETE
//#define WIZFI360_CALLBACK_USED_WIFI_CONNECTED
//#define WIZFI360_CALLBACK_USED_WIFI_DISCONNECTED
#define WIZFI360_CALLBACK_USED_WIFI_CONNECT_FAILED
#define WIZFI360_CALLBACK_USED_MODULE_READY

/*********************************************************************************************/

#endif /* INCLUDES_WIZFI360_OPTS_H_ */
