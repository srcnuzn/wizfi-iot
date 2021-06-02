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


TEST(test_MqttClient_ReadDouble, ReadsDoubleFromMessage)
{
	char message[] = "{\"doubleVal\": 3.532134}";
	double result = MqttClient_ReadDouble(message, "doubleVal");
	ASSERT_DOUBLE_EQ(result, 3.532134);
}
