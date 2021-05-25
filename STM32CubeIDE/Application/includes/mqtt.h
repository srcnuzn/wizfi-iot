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

/*********************************************************************************************/

#endif /* INCLUDES_MQTT_H_ */
