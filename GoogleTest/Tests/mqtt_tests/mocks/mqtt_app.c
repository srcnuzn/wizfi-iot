/*
 * mqtt_user.c
 *
 *  Created on: 24.05.2021
 *      Author: serca
 */

/*********************************************************************************************/
/* Includes ---------------------------------------------------------------------------------*/

// The MqttClient driver
#include "mqtt.h"
// The MqttClient user-defintions
#include "mqtt_user.h"
// The required functions, that are implemented in this source-file.
#include "mqtt_required.h"

/*********************************************************************************************/
/* Private function prototypes --------------------------------------------------------------*/

static void TestTopicReceived(const char* message);

/*********************************************************************************************/
/* Implemetation of required application-specific functions ---------------------------------*/

/*
 * @brief	Writes the JSON-message to be published.
 * @note	This is an example implementation of the required function (mqtt_required.h).
 * @note	To adapt the JSON-message, following functions can be used:
 * 				- MqttClient_PublishInteger
 * 				- MqttClient_PublishString
 * 				- MqttClient_PublishBoolean
 * 				- MqttClient_PublishDouble
 * @note	This function is called periodically in MqttClient_Process(), when
 * 			a connection to the broker is established. The publish period can be
 * 			specified by the user-specific definition MQTT_PUBLISH_INTERVAL.
 * @retval	None
 */
void MqttClient_Publish()
{
	MqttClient_PublishInteger("time", 123);
}

/*
 * @brief	Registers user-specific callbacks to given events.
 * @note	This is an example implementation of the required function (mqtt_required.h).
 * 			To adapt the callback-registration, following functions can be used:
 * 				- MqttClient_RegisterSubscribeCallback
 * @retval	None
 */
void MqttClient_RegisterUserCallbacks()
{
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_1, TestTopicReceived);
}

/*********************************************************************************************/
/* Private function definitions -------------------------------------------------------------*/

/*
 * @brief	Callback is fired, when MQTT_SUBTOPIC_1 is received.
 * @note	This is an example function to demonstrate how the callback registration works.
 * @note	To read the JSON-message, following functions can be used:
 * 				- MqttClient_ReadInteger
 * 				- MqttClient_ReadString
 * 				- MqttClient_ReadBoolean
 * 				- MqttClient_ReadDouble
 * @param	message string-paramater: The message that is being received with MQTT_SUBTOPIC_1.
 * @retval	None
 */
static void TestTopicReceived(const char* message)
{
	// int a = MqttClient_ReadInteger(message, description);

}

/*********************************************************************************************/
