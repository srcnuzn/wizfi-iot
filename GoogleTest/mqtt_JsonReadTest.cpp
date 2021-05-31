/*
 * test.cpp
 *
 *  Created on: 29.05.2021
 *      Author: serca
 */


#include "gtest/gtest.h"

extern "C" {
#include "mqtt.h"
}


TEST(MqttClient_ReadInteger, ReadsIntegerValueFromMessage)
{
	char message[] = "{\"intVal\": 3}";
	int result = MqttClient_ReadInteger(message, "intVal");
	ASSERT_EQ(result, 3);
}

TEST(MqttClient_ReadDouble, ReadsDoubleFromMessage)
{
	char message[] = "{\"doubleVal\": 3.532134}";
	double result = MqttClient_ReadDouble(message, "doubleVal");
	ASSERT_DOUBLE_EQ(result, 3.532134);
}
