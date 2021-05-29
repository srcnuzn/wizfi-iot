/*
 * required.c
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

/*********************************************************************************************/
/* User Includes ----------------------------------------------------------------------------*/

#include "mqtt_required.h"

/*********************************************************************************************/
/* User Variables ---------------------------------------------------------------------------*/

static uint32_t tick = 0;

/*********************************************************************************************/
/* Implementation of required target-specific functions -------------------------------------*/

/*
 * @brief	Provides a tick value in millisecond.
 * @note	This is an example implementation of the required function (mqtt_required.h).
 * @retval	tick value (uint32) in milliseconds
 */
uint32_t MqttClient_GetTick()
{
	return tick;
}

/*
 * @brief	Performs a software-reset of the microcontroller.
 * @note	This is an example implementation of the required function (mqtt_required.h).
 * @retval	None
 */
void MqttClient_HandleSystemResetRequest()
{

}

/*********************************************************************************************/

void MqttClient_SetTick(uint32_t value)
{
	tick = value;
}

/*********************************************************************************************/
