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

#define MQTT_JSON_FORMATTING_OVERHEAD 10

#define MQTT_JSON_FORMATTING_DOUBLE_LITERALS 2

void MqttClient_Initialize();

void MqttClient_Process();

void MqttClient_SubscribeTo(const char* topic);

void MqttClient_RegisterSubscribeCallback(const char* topic, void (*func)(const char*));

void MqttClient_PublishInteger(const char* description, const int value);

void MqttClient_PublishString(const char* description, const char* value);

void MqttClient_PublishDouble(const char* description, const double value);

int MqttClient_ReadInteger(const char* message, const char* description);

int MqttClient_ReadString(const char* message, const char* description, int destLen, char* result);

double MqttClient_ReadDouble(const char* message, const char* description);

/*********************************************************************************************/

#endif /* INCLUDES_MQTT_H_ */
