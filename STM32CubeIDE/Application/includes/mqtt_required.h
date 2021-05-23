/*
 * mqtt_required.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

#ifndef INCLUDES_MQTT_REQUIRED_H_
#define INCLUDES_MQTT_REQUIRED_H_

/*********************************************************************************************/
/* Required functions to be implemented by user ---------------------------------------------*/

extern void MqttClient_Publish();

extern uint32_t MqttClient_GetTick();

extern void MqttClient_HandleSystemResetRequest();

/*********************************************************************************************/


#endif /* INCLUDES_MQTT_REQUIRED_H_ */
