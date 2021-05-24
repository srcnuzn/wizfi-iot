/*
 * mqtt_user.c
 *
 *  Created on: 24.05.2021
 *      Author: serca
 */

/*********************************************************************************************/

#include "main.h"
#include "mqtt.h"
#include "mqtt_user.h"

/*********************************************************************************************/

static void TestTopicReceived(char* message);

/*********************************************************************************************/

/**
 *	TODO: Comment on MqttClient_Publish
 */
void MqttClient_Publish()
{
	MqttClient_PublishInteger("time", HAL_GetTick());
}

/**
 *	TODO: Comment on MqttClient_RegisterCallbacks
 */
void MqttClient_RegisterCallbacks()
{
	MqttClient_RegisterSubscribeCallback(MQTT_SUBTOPIC_1, TestTopicReceived);
}

/*********************************************************************************************/

/**
 *	TODO: Comment on TestTopicReceived
 */
static void TestTopicReceived(char* message)
{
	__NOP();
}

/*********************************************************************************************/
