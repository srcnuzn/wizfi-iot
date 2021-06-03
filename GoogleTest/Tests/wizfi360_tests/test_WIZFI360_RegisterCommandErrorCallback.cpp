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

static int isExecuted = 0;

static void User_CommandErrorCallback()
{
	isExecuted = 1;
}

TEST(test_WIZFI360_RegisterCommandErrorCallback, UserCallbackGetsRegistered)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterCommandErrorCallback(User_CommandErrorCallback);

	ASSERT_EQ(wizfi360.CommandErrorCallback, User_CommandErrorCallback);
}

TEST(test_WIZFI360_RegisterCommandErrorCallback, ErrorTagReceived_UserCallbackFires)
{
	WIZFI360_Initialize();

	WIZFI360_RegisterCommandErrorCallback(User_CommandErrorCallback);

	WIZFI360_Start();

	WIZFI360_UART_BytesReceived(WIZFI360_TAG_ERROR, strlen(WIZFI360_TAG_ERROR));

	WIZFI360_Process();

	ASSERT_TRUE(isExecuted);
}

