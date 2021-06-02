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

static int execution_ctr;
static char received_msg[128];

static const char subtopic[] = "test-topic/sensor/value";
static const char subtopic_msg[] = "subtopic message";

static const char another_subtopic[] = "test-topic/sensor/another_value";
static const char another_subtopic_msg[] = "another subtopic message";

class test_WIZFI360_RegisterSubTopicCallback : public ::testing::Test
{
	protected:
		virtual void SetUp()
		{
			execution_ctr = 0;
			for(int i = 0; i < 128; i++)
				received_msg[i] = 0;
		}

		// virtual void TearDown() {}
};


static void SubTopicReceivedCallback(char* message)
{
	execution_ctr++;
	strcpy(received_msg, message);
}

TEST_F(test_WIZFI360_RegisterSubTopicCallback, UserCallbackGetsRegistered)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);

	ASSERT_EQ(wizfi360.SubTopicCallbacks[0], SubTopicReceivedCallback);

	ASSERT_EQ(wizfi360.NumSubTopicCallbacks, 1);
}

TEST_F(test_WIZFI360_RegisterSubTopicCallback, SubTopicReceived_UserCallbackFiresAndMessageIsReceived)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);

	WIZFI360_Start();

	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	for (int i = 0; i < 100; i++)
		WIZFI360_Process();

	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}


TEST_F(test_WIZFI360_RegisterSubTopicCallback, MultipleTopicsReceived_RegisteredSubTopicMessageReceivedCorrectly_1)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);

	WIZFI360_Start();

	WIZFI360_UART_BytesReceived(another_subtopic, strlen(another_subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(another_subtopic_msg, strlen(another_subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	for (int i = 0; i < 100; i++)
		WIZFI360_Process();


	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}

TEST_F(test_WIZFI360_RegisterSubTopicCallback, MultipleTopicsReceived_RegisteredSubTopicMessageReceivedCorrectly_2)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);

	WIZFI360_Start();

	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	for (int i = 0; i < 100; i++)
		WIZFI360_Process();

	WIZFI360_UART_BytesReceived(another_subtopic, strlen(another_subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(another_subtopic_msg, strlen(another_subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}

TEST_F(test_WIZFI360_RegisterSubTopicCallback, MultipleTopicsReceived_RegisteredSubTopicMessageReceivedCorrectly_3)
{

	WIZFI360_Initialize();
	WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);
	WIZFI360_Start();

	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	WIZFI360_UART_BytesReceived(another_subtopic, strlen(another_subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(another_subtopic_msg, strlen(another_subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	for (int i = 0; i < 100; i++)
		WIZFI360_Process();

	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}

TEST_F(test_WIZFI360_RegisterSubTopicCallback, MultipleTopicsReceived_RegisteredSubTopicMessageReceivedCorrectly_4)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterSubTopicCallback(subtopic, SubTopicReceivedCallback);

	WIZFI360_Start();

	WIZFI360_UART_BytesReceived(another_subtopic, strlen(another_subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(another_subtopic_msg, strlen(another_subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	for (int i = 0; i < 100; i++)
		WIZFI360_Process();

	WIZFI360_UART_BytesReceived(subtopic, strlen(subtopic));
	WIZFI360_UART_BytesReceived(" -> ", strlen(" -> "));
	WIZFI360_UART_BytesReceived(subtopic_msg, strlen(subtopic_msg));
	WIZFI360_UART_BytesReceived("\r\n", strlen("\r\n"));

	ASSERT_EQ(execution_ctr, 1);
	ASSERT_STREQ(received_msg, subtopic_msg);
}


