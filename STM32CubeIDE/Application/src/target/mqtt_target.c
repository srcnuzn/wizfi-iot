/*
 * required.c
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

/*********************************************************************************************/

/* User Includes ----------------------------------------------------------------------------*/

// Used to access STM32-HAL drivers / CubeMX definitions
#include "main.h"

/*********************************************************************************************/
/* Implementation of required target-specific functions -------------------------------------*/

/*
 * @brief	Provides a tick value in millisecond.
 * @note	This is an example implementation of the required function (mqtt_required.h).
 * @retval	tick value (uint32) in milliseconds
 */
uint32_t MqttClient_GetTick()
{
	return HAL_GetTick();
}

/*
 * @brief	Performs a software-reset of the microcontroller.
 * @note	This is an example implementation of the required function (mqtt_required.h).
 * @retval	None
 */
void MqttClient_HandleSystemResetRequest()
{
	NVIC_SystemReset();
}

/*********************************************************************************************/
