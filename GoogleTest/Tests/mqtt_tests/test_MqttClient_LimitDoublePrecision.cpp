/*
 * test.cpp
 *
 *  Created on: 29.05.2021
 *      Author: serca
 */


#include "../gtest/gtest.h"

extern "C" {
#include "mqtt.h"
extern double MqttClient_LimitDoublePrecision(const double value, const int precision);
}


TEST(test_MqttClient_LimitDoublePrecision, PositiveNumberWithLowerPrecisionFullyRepresented)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = 13.8;
	double expected = 13.8;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, PositiveNumberWithHigherPrecisionRepresentationLimited)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = 13.845987;
	double expected = 13.845;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, ZeroRepresentedCorrectly)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = 0.0;
	double expected = 0.0;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, NegativeNumberWithHigherPrecisionRepresentationLimited)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = -13.845987;
	double expected = -13.845;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, NegativeNumberWithLowerPrecisionFullyRepresented)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = -13.8;
	double expected = -13.8;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, PositiveNumberWithMatchingPrecisionFullyRepresented)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = 13.812;
	double expected = 13.812;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, NegativeNumberWithMatchingPrecisionFullyRepresented)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = -13.812;
	double expected = -13.812;
	const int precision = 3;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, PrecisionZeroExpectValueIsWithoutDecimals)
{
//	for(int i=0; i<10; i++)
//	{
//
//	}
	double input = -13.812;
	double expected = -13.0;
	const int precision = 0;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, PrecisionNegativeExpectValueIsWithoutDecimals)
{
	double input = -13.812;
	double expected = -13.0;
	const int precision = -5;

	double result =  MqttClient_LimitDoublePrecision(input, precision);

	ASSERT_DOUBLE_EQ(result, expected);
}


TEST(test_MqttClient_LimitDoublePrecision, Radical2)
{

	ASSERT_EQ(7126.0, 1000.0*7.126);
}
