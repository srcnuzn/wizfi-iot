/*
 * wizfi360_opts.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 *
 *
 *  TODO: Add Comment for wizfi360_opts.h
 *
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

#define WIZFI360_MAX_CMD_LEN 				256

#define WIZFI360_MAX_SUBTOPIC_LEN		 	64
#define WIZFI360_MAX_SUBTOPIC_CALLBACKS		16

/*********************************************************************************************/

#define WIZFI360_UART_TX_MODE_NON_BLOCKING

/*********************************************************************************************/

#endif /* INCLUDES_WIZFI360_OPTS_H_ */
