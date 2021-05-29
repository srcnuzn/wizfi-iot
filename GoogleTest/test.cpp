/*
 * test.cpp
 *
 *  Created on: 29.05.2021
 *      Author: serca
 */


#include "gtest/gtest.h"

extern "C" {
#include "wizfi360.h"
}

extern WIZFI360_HandlerTypedef wizfi360;

TEST(WIZFI360_Initialize, struct_fields_as_expected) {

	WIZFI360_Initialize();

	ASSERT_EQ(wizfi360.CommandBuffer[0], '\0');
	ASSERT_EQ(wizfi360.CommandLength, 0);
	ASSERT_EQ(wizfi360.ExpectingEcho, 0);
	ASSERT_EQ(wizfi360.ExpectingResponse, 0);
	ASSERT_EQ(wizfi360.UartRxBuffer.head_index, 0u);
	ASSERT_EQ(wizfi360.UartRxBuffer.tail_index, 0u);
}
