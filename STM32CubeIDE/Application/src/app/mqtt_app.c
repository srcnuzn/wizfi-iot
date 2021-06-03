/*
 * mqtt_user.c
 *
 *  Created on: 24.05.2021
 *      Author: serca
 */

/*********************************************************************************************/
/* Includes ---------------------------------------------------------------------------------*/

// Used to access STM32-HAL drivers / CubeMX definitions
#include "main.h"
// The MqttClient driver
#include "mqtt.h"
// The MqttClient user-defintions
#include "mqtt_user.h"
// The required functions, that are implemented in this source-file.
#include "mqtt_required.h"

/*********************************************************************************************/
/* Private function prototypes --------------------------------------------------------------*/

static void SubTopic1Received(char* message);
static void SubTopic2Received(char* message);
static void SubTopic3Received(char* message);

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
	MqttClient_PublishInteger("int", HAL_GetTick());
	MqttClient_PublishDouble("double", 0.5467*(double)(HAL_GetTick()));
	MqttClient_PublishString("string", "this is a string");
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
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_1, SubTopic1Received);
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_2, SubTopic2Received);
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_3, SubTopic3Received);
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
static void SubTopic1Received(char* message)
{
	int i = MqttClient_ReadInteger(message, "int");
	double d = MqttClient_ReadDouble(message, "double");
	const char* s[64];
	int stringSuccess = MqttClient_ReadString(message, "string", 64, s);
	__NOP();
}

/*
 * @brief	Callback is fired, when MQTT_SUBTOPIC_2 is received.
 * @note	This is an example function to demonstrate how the callback registration works.
 * @note	To read the JSON-message, following functions can be used:
 * 				- MqttClient_ReadInteger
 * 				- MqttClient_ReadString
 * 				- MqttClient_ReadBoolean
 * 				- MqttClient_ReadDouble
 * @param	message string-paramater: The message that is being received with MQTT_SUBTOPIC_1.
 * @retval	None
 */
static void SubTopic2Received(char* message)
{
	// int a = MqttClient_ReadInteger(message, description);
	__NOP();
}

/*
 * @brief	Callback is fired, when MQTT_SUBTOPIC_2 is received.
 * @note	This is an example function to demonstrate how the callback registration works.
 * @note	To read the JSON-message, following functions can be used:
 * 				- MqttClient_ReadInteger
 * 				- MqttClient_ReadString
 * 				- MqttClient_ReadBoolean
 * 				- MqttClient_ReadDouble
 * @param	message string-paramater: The message that is being received with MQTT_SUBTOPIC_1.
 * @retval	None
 */
static void SubTopic3Received(char* message)
{
	// int a = MqttClient_ReadInteger(message, description);
	__NOP();
}
/*********************************************************************************************/
