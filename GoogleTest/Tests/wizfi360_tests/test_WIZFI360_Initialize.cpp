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

	extern void DefaultSubscribeCallback(char* message);
	extern void DefaultCommandOkCallback(void);
	extern void DefaultCommandErrorCallback(void);
	extern void DefaultCommandFailCallback(void);
	extern void DefaultReadyCallback(void);
	extern void DefaultWifiConnectedCallback(void);
	extern void DefaultWifiDisconnectedCallback(void);
}


TEST(WIZFI360_Initialize, CommandBufferGetsReset) {

	wizfi360.CommandBuffer[0] = 'A';
	wizfi360.CommandLength = 32;
	wizfi360.CommandId = WIZFI360_CMD_ID_MQTTSET;

	WIZFI360_Initialize();

	ASSERT_EQ(wizfi360.CommandBuffer[0], '\0');
	ASSERT_EQ(wizfi360.CommandLength, 0);
	ASSERT_EQ(wizfi360.CommandId, -1);
}


TEST(WIZFI360_Initialize, CallbacksInitializeToDefault) {

	for (int i = 0; i < WIZFI360_MAX_SUBTOPIC_CALLBACKS; i++)
	{
		wizfi360.SubTopicCallbacks[i] = NULL;
	}
	wizfi360.CommandOkCallback = NULL;
	wizfi360.CommandErrorCallback = NULL;
	wizfi360.WifiConnectFailedCallback = NULL;
	wizfi360.ReadyCallback = NULL;
	wizfi360.WifiConnectedCallback = NULL;
	wizfi360.WifiDisconnectedCallback = NULL;

	WIZFI360_Initialize();

	for (int i = 0; i < WIZFI360_MAX_SUBTOPIC_CALLBACKS; i++)
	{
		ASSERT_EQ(wizfi360.SubTopicCallbacks[i], DefaultSubscribeCallback);
	}
	ASSERT_EQ(wizfi360.CommandOkCallback, DefaultCommandOkCallback);
	ASSERT_EQ(wizfi360.CommandErrorCallback, DefaultCommandErrorCallback);
	ASSERT_EQ(wizfi360.WifiConnectFailedCallback, DefaultCommandFailCallback);
	ASSERT_EQ(wizfi360.ReadyCallback, DefaultReadyCallback);
	ASSERT_EQ(wizfi360.WifiConnectedCallback, DefaultWifiConnectedCallback);
	ASSERT_EQ(wizfi360.WifiDisconnectedCallback, DefaultWifiDisconnectedCallback);
}

TEST(WIZFI360_Initialize, ScanVariablesReset) {

	wizfi360.EchoCharsReceived = 10;

	for (int i = 0; i < WIZFI360_NUM_TAGS; i++)
	{
		wizfi360.TagCharsReceived[i] = i+1;
	}

	for (int i = 0; i < WIZFI360_MAX_SUBTOPIC_CALLBACKS; i++)
	{
		wizfi360.SubTopicCharsReceived[i] = i+1;
	}

	wizfi360.MessageIncoming = 1;
	wizfi360.receivingTopic = 12;
	wizfi360.cr_found = 1;

	WIZFI360_Initialize();

	ASSERT_EQ(wizfi360.EchoCharsReceived, 0);

	for (int i = 0; i < WIZFI360_NUM_TAGS; i++)
	{
		ASSERT_EQ(wizfi360.TagCharsReceived[i], 0);
	}

	for (int i = 0; i < WIZFI360_MAX_SUBTOPIC_CALLBACKS; i++)
	{
		ASSERT_EQ(wizfi360.SubTopicCharsReceived[i], 0);
	}

	ASSERT_FALSE(wizfi360.MessageIncoming);

	ASSERT_EQ(wizfi360.receivingTopic, 0);

	ASSERT_FALSE(wizfi360.cr_found);
}


