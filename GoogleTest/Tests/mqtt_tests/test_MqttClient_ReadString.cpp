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


TEST(test_MqttClient_ReadString, ReadsStringFromJson)
{
	char message[] = "{\"astring\" : \"This is a string\"}";
	char expected[] = "This is a string";
	const int maxLen = sizeof(expected);
	char result[maxLen];

	int ret = MqttClient_ReadString(message, "astring", maxLen, result);

	const int success = 1;
	ASSERT_EQ(ret, success);
	ASSERT_STREQ(result, expected);
}

TEST(test_MqttClient_ReadString, ReadsStringFromJsonWithLineBreak)
{
	char message[] = "{\n  \"astring\": \"This is a string\"\n}";
	char expected[] = "This is a string";
	const int maxLen = sizeof(expected);
	char result[maxLen];

	int ret = MqttClient_ReadString(message, "astring", maxLen, result);

	const int success = 1;
	ASSERT_EQ(ret, success);
	ASSERT_STREQ(result, expected);
}
