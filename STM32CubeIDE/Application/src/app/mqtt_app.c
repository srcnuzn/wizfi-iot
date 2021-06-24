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
/* Private defines  --------------------------------------------------------------*/

#ifdef MQTT_SUBSCRIBE_MODE
#define MQTT_SUBTOPIC_1 	"mc-labor/soenke/test"
#define MQTT_SUBTOPIC_2 	"mc-labor/soenke/test2"
#define MQTT_SUBTOPIC_3 	"mc-labor/soenke/test3"
#endif

/*********************************************************************************************/
/* Private function prototypes --------------------------------------------------------------*/

#ifdef MQTT_SUBSCRIBE_MODE
static void SubTopic1Received(const char* message);
static void SubTopic2Received(const char* message);
static void SubTopic3Received(const char* message);
#endif

/*********************************************************************************************/
/* Private variables  --------------------------------------------------------------*/

#ifdef MQTT_SUBSCRIBE_MODE

#define MAX_STRING_SIZE 64

static uint32_t SubTopic1ReceivedCounter = 0;
static uint32_t SubTopic2ReceivedCounter = 0;
static uint32_t SubTopic3ReceivedCounter = 0;

static int SubTopic1ReceivedInteger = 0;
static double SubTopic1ReceivedDouble = 0.0;
static char SubTopic1ReceivedString[MAX_STRING_SIZE];
static int SubTopic1ReceivedStringSuccessfully = 0;

#endif

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
#ifdef MQTT_PUBLISH_MODE
	MqttClient_PublishInteger("int", HAL_GetTick());
	MqttClient_PublishDouble("double", 0.5467*(double)(HAL_GetTick()));
	MqttClient_PublishString("string", "this is a string");
#endif
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
#ifdef MQTT_SUBSCRIBE_MODE
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_1, SubTopic1Received);
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_2, SubTopic2Received);
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_3, SubTopic3Received);
#endif
}

/*********************************************************************************************/
/* Private function definitions -------------------------------------------------------------*/

#ifdef MQTT_SUBSCRIBE_MODE
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
static void SubTopic1Received(const char* message)
{
	SubTopic1ReceivedCounter++;
	SubTopic1ReceivedInteger = MqttClient_ReadInteger(message, "int");
	SubTopic1ReceivedDouble = MqttClient_ReadDouble(message, "double");
	SubTopic1ReceivedStringSuccessfully = MqttClient_ReadString(message,
			"string", MAX_STRING_SIZE, SubTopic1ReceivedString);
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
static void SubTopic2Received(const char* message)
{
	SubTopic2ReceivedCounter++;
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
static void SubTopic3Received(const char* message)
{
	SubTopic3ReceivedCounter++;
}
/*********************************************************************************************/
#endif
