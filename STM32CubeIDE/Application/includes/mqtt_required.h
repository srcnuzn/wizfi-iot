/*
 * mqtt_required.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 */

#ifndef INCLUDES_MQTT_REQUIRED_H_
#define INCLUDES_MQTT_REQUIRED_H_

/*********************************************************************************************/
/* Target specific functions to be implemented by user --------------------------------------*/

/*
 * TODO: Comment on MqttClient_GetTick
 */
extern uint32_t MqttClient_GetTick();

/*
 * TODO: Comment on MqttClient_HandleSystemResetRequest
 */
extern void MqttClient_HandleSystemResetRequest();

/*********************************************************************************************/
/* Application specific functions to be implemented by user ---------------------------------*/

/*
 * TODO: Comment on MqttClient_Publish
 */
extern void MqttClient_Publish();

/*
 * TODO: Comment on MqttClient_RegisterCallbacks
 */
extern void MqttClient_RegisterCallbacks();

/*********************************************************************************************/


#endif /* INCLUDES_MQTT_REQUIRED_H_ */
