/*
 * test.cpp
 *
 *  Created on: 29.05.2021
 *      Author: serca
 */


#include "../gtest/gtest.h"

extern "C" {
#include "mqtt.h"
extern void JRead_BuildIdentifier(const char* description, char* ident_out, uint8_t layer);
}


TEST(JRead_BuildIdentifier, IdentifierBuiltCorrectly)
{
	char description[] = "intVal";
	const char expected[] = "{'intVal'";
	const int layer = 0;

	// Estimate identifier length
	const int identLen =
		(layer+1) * MQTT_JSON_FORMATTING_OVERHEAD +
				// taking into account formatting starting at lowest layer
		strlen(description) +
				// length of the full entry path
		1;		// length of trailing '
	char identifier[identLen];

	JRead_BuildIdentifier(description, identifier, layer);
	ASSERT_STREQ(identifier, expected);
}
