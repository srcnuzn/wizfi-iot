/*
 * mqtt.h
 *
 *  Created on: 08.05.2021
 *      Author: serca
 *
 *      TODO: Add comments for mqtt.h
 *
 */

#ifndef INCLUDES_MQTT_H_
#define INCLUDES_MQTT_H_

/*********************************************************************************************/
// TODO: Check if include is needed.
#include <stdint.h>

void MqttClient_Initialize();

void MqttClient_Process();

void MqttClient_SubscribeTo(const char* topic);

void MqttClient_RegisterSubscribeCallback(const char* topic, void (*func)(char*));

void MqttClient_PublishInteger(const char* description, const int value);

void MqttClient_PublishString(const char* description, const char* value);

void MqttClient_PublishDouble(const char* description, const double value);

void MqttClient_PublishBoolean(const char* description, const int oneOrZero);

int MqttClient_ReadInteger(const char* message, const char* description);

char* MqttClient_ReadString(const char* message, const char* description);

double MqttClient_ReadDouble(const char* message, const char* description);

int MqttClient_ReadBoolean(const char* message, const char* description);

char* JRead_BuildIdentifier(const char* description, uint8_t layer);

/*********************************************************************************************/

#endif /* INCLUDES_MQTT_H_ */
