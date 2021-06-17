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

static const char subtopic[] = "subtopic/sensor";
static const char subtopic_msg[] = "subtopic message";

static const char pubtopic_msg[] = "pubtopic message";

static int execution_ctr;
static char received_msg[128];

static void SubTopicReceivedCallback(const char* message)
{
	execution_ctr++;
	strcpy(received_msg, message);
}


class test_WIZFI360_Process : public ::testing::Test
{
	protected:
		virtual void SetUp()
		{
			execution_ctr = 0;

			for(int i = 0; i < 128; i++)
				received_msg[i] = 0;

			for (int i = 0; i < RING_BUFFER_SIZE; i++)
				wizfi360.UartRxBuffer.buffer[i] = 0;

			WIZFI360_Initialize();

			WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);

			WIZFI360_Start();
		}

		virtual void TearDown()
		{
			WIZFI360_Stop();
		}
};

/*
 * TODO: Comment
 */
TEST_F(test_WIZFI360_Process, MessagePublishedAndEchoAndResponseAndSubTopicReceivedInOrder_EverythingProcessedCorrectly)
{
	wizfi360.WifiMode = WIZFI360_MODE_STATION;
	wizfi360.WifiState = WIZFI360_WIFI_CONNECTED;
	wizfi360.EchoMode = WIZFI360_ECHO_ENABLE;

	// publish topic
	WIZFI360_AT_MqttPublishMessage(pubtopic_msg);
	// Make sure we expect echo and response...
	ASSERT_TRUE(wizfi360.ExpectingEcho);
	ASSERT_TRUE(wizfi360.ExpectingResponse);


	// Receive echo, ok-response and subTopic in order
	WIZFI360_UART_BytesReceived(wizfi360.CommandBuffer, wizfi360.CommandLength);
	WIZFI360_UART_BytesReceived(WIZFI360_TAG_OK, strlen(WIZFI360_TAG_OK));

	// Receive SubTopic
	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	// Do the processing
	for (int i = 0; i < 50; i++)
		WIZFI360_Process();

	// Echo and Response were received correctly
	ASSERT_FALSE(wizfi360.ExpectingEcho);
	ASSERT_FALSE(wizfi360.ExpectingResponse);

	// Callback was executed and message correctly received
	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}


/*
 * TODO: Comment
 */
TEST_F(test_WIZFI360_Process, MessagePublishedAndSubTopicReceivedWhileReceivingEchoAndResponse_EverythingProcessedCorrectly)
{
	wizfi360.WifiMode = WIZFI360_MODE_STATION;
	wizfi360.WifiState = WIZFI360_WIFI_CONNECTED;
	wizfi360.EchoMode = WIZFI360_ECHO_ENABLE;

	// publish topic
	WIZFI360_AT_MqttPublishMessage(pubtopic_msg);
	// Make sure we expect echo and response...
	ASSERT_TRUE(wizfi360.ExpectingEcho);
	ASSERT_TRUE(wizfi360.ExpectingResponse);

	// Partly receive echo...
	const int half = wizfi360.CommandLength / 2;

	// Receive first half of echo
	for (int i = 0; i < half; i++)
	{
		WIZFI360_UART_BytesReceived(&(wizfi360.CommandBuffer[i]), 1);

	}

	// Receive SubTopic in between
	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	// Receive second half of echo
	for (int i = half; i < wizfi360.CommandLength; i++)
	{
		WIZFI360_UART_BytesReceived(&(wizfi360.CommandBuffer[i]), 1);

	}

	// Receive ok-response
	WIZFI360_UART_BytesReceived(WIZFI360_TAG_OK, strlen(WIZFI360_TAG_OK));

	// Do the processing
	for (int i = 0; i < 100; i++)
	{
		if (wizfi360.UartRxBuffer.tail_index ==
				wizfi360.UartRxBuffer.head_index)
			break;

		WIZFI360_Process();
	}

	// Echo and Response were received correctly
	ASSERT_FALSE(wizfi360.ExpectingEcho);
	ASSERT_FALSE(wizfi360.ExpectingResponse);

	// Callback was executed and message correctly received
	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}
