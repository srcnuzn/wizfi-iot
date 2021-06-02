/*
 * MqttClient_ReadInteger.cpp
 *
 *  Created on: 01.06.2021
 *      Author: serca
 */


#include "../gtest/gtest.h"

extern "C" {
#include "mqtt.h"
}


TEST(test_MqttClient_ReadInteger, ReadsIntegerValueFromMessage)
{
	char message[] = "{\"intVal\": 3}";
	int result = MqttClient_ReadInteger(message, "intVal");
	ASSERT_EQ(result, 3);
}
