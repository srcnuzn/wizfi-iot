/*
 * mqtt_required.h
 *
 *  Created on: 21.05.2021
 *      Author: serca
 *
 *  TODO: Add Comment for mqtt_required.h
 */

#ifndef INCLUDES_MQTT_REQUIRED_H_
#define INCLUDES_MQTT_REQUIRED_H_

/*********************************************************************************************/
/* Target specific functions to be implemented by user --------------------------------------*/

/*
 * @brief	Must provide a tick value in millisecond.
 * @note	This target-specific function must be implemented by user.
 * @retval	tick value (uint32) in milliseconds
 */
extern uint32_t MqttClient_GetTick();

/*
 * @brief	Must performs a software-reset of the microcontroller.
 * @note	This target-specific function must be implemented by user.
 * @retval	None
 */
extern void MqttClient_HandleSystemResetRequest();

/*********************************************************************************************/
/* Application specific functions to be implemented by user ---------------------------------*/

/*
 * @brief	Must prepare the JSON-message to be published.
 * @note	This application-specific function must be implemented by user.
 * 			To write the JSON-message, following functions can be used:
 * 				- MqttClient_PublishInteger
 * 				- MqttClient_PublishString
 * @note	This function is called periodically in MqttClient_Process(), when
 * 			a connection to the broker is established. The publish period can be
 * 			specified by the user-specific definition MQTT_PUBLISH_INTERVAL.
 * @retval	None
 */
extern void MqttClient_Publish();

/*
 * @brief	Must register user-specific callbacks.
 * @note	This application-specific function must be implemented by user.
 * 			To register callbacks, following functions can be used:
 * 				- MqttClient_RegisterSubscribeCallback
 * @retval	None
 */
extern void MqttClient_RegisterCallbacks();

/*********************************************************************************************/


#endif /* INCLUDES_MQTT_REQUIRED_H_ */
