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

// The expected commandId for all tests

class test_WIZFI360_AT_MqttPublishMessage : public ::testing::Test
{
	protected:
		virtual void SetUp()
		{
			WIZFI360_Initialize();

			for (int i = 0; i < RING_BUFFER_SIZE; i++)
				wizfi360.UartRxBuffer.buffer[i] = 0;

			WIZFI360_Start();

			wizfi360.WifiMode = WIZFI360_MODE_STATION;

			wizfi360.WifiState = WIZFI360_WIFI_CONNECTED;
		}

		virtual void TearDown()
		{
			WIZFI360_Stop();
		}
};

// Example: AT+MQTTPUB="HelloWorld!"<CR><LF>

TEST_F(test_WIZFI360_AT_MqttPublishMessage, ValidInput_ReturnsOkWithCommandAsExpected)
{
	const char expectedCommandBuffer[] = "AT+MQTTPUB=\"HelloWorld!\"\r\n";

	WIZFI360_StatusTypeDef ret = WIZFI360_AT_MqttPublishMessage("HelloWorld!");

	ASSERT_EQ(ret, WIZFI360_OK);
	ASSERT_STREQ(wizfi360.CommandBuffer, expectedCommandBuffer);
	ASSERT_EQ(wizfi360.CommandLength, strlen(expectedCommandBuffer));
	ASSERT_EQ(wizfi360.CommandId, WIZFI360_CMD_ID_MQTTPUB);
}

TEST_F(test_WIZFI360_AT_MqttPublishMessage, CommandSentWithEchoEnabled_ResponseAndEchoFlagsSet)
{
	wizfi360.EchoMode = WIZFI360_ECHO_ENABLE;

	WIZFI360_StatusTypeDef ret = WIZFI360_AT_MqttPublishMessage("HelloWorld!");

	ASSERT_EQ(ret, WIZFI360_OK);
	ASSERT_TRUE(wizfi360.ExpectingEcho);
	ASSERT_TRUE(wizfi360.ExpectingResponse);
}

TEST_F(test_WIZFI360_AT_MqttPublishMessage, CommandSentWithEchoEnabledResponseAndEchoReceived_ResponseAndEchoFlagsCleared)
{
	wizfi360.EchoMode = WIZFI360_ECHO_ENABLE;

	WIZFI360_AT_MqttPublishMessage("HelloWorld!");

	// Receive Echo
	WIZFI360_UART_BytesReceived(wizfi360.CommandBuffer, wizfi360.CommandLength);
	// Receive OK
	WIZFI360_UART_BytesReceived(WIZFI360_TAG_OK, strlen(WIZFI360_TAG_OK));
	// Process received data
	for (int i = 0; i < 10; i++)
		WIZFI360_Process();

	ASSERT_FALSE(wizfi360.ExpectingEcho);
	ASSERT_FALSE(wizfi360.ExpectingResponse);
}





