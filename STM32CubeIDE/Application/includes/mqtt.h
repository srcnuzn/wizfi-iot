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

void MqttClient_Initialize();

void MqttClient_Process();

void MqttClient_SubscribeTo(const char* topic);

void MqttClient_RegisterSubscribeCallback(const char* topic, void (*func)(char*));

void MqttClient_PublishInteger(const char* description, const int value);

void MqttClient_PublishString(const char* description, const char* value);

void MqttClient_PublishDouble(const char* description, const double value);

void MqttClient_PublishBoolean(const char* description, const int oneOrZero);

int MqttClient_ReadInteger(const char* description, const int value);

char* MqttClient_ReadString(const char* description, const char* value);

double MqttClient_ReadDouble(const char* description, const double value);

int MqttClient_ReadBoolean(const char* description, const int value);

/*********************************************************************************************/

#endif /* INCLUDES_MQTT_H_ */
