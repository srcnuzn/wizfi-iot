/*
 * wizfi360_InitializeTest.cpp
 *
 *  Created on: 30.05.2021
 *      Author: serca
 */


#include "../gtest/gtest.h"

extern "C" {
#include "wizfi360.h"
extern WIZFI360_HandlerTypedef wizfi360;
}

static const char pubTopic[]  = "pubTopic";
static const char subTopic[] = "subTopic";

// The expected commandId for all tests

class test_WIZFI360_AT_MqttSetTopic : public ::testing::Test
{
	protected:
		virtual void SetUp()
		{
			WIZFI360_Initialize();
			WIZFI360_Start();
		}

		virtual void TearDown()
		{

		}
};

// Example: AT+MQTTTOPIC="pubTopic","subTopic"<CR><LF>

TEST_F(test_WIZFI360_AT_MqttSetTopic, WithoutOptionalSubTopics_ReturnsOkWithCommandAsExpected)
{
	const char expectedCommandBuffer[] = "AT+MQTTTOPIC=\"pubTopic\",\"subTopic\"\r\n";

	WIZFI360_StatusTypeDef ret = WIZFI360_AT_MqttSetTopic(pubTopic, subTopic);

	ASSERT_EQ(ret, WIZFI360_OK);
	ASSERT_STREQ(wizfi360.CommandBuffer, expectedCommandBuffer);
	ASSERT_EQ(wizfi360.CommandLength, strlen(expectedCommandBuffer));
	ASSERT_EQ(wizfi360.CommandId, WIZFI360_CMD_ID_MQTTTOPIC);
}

