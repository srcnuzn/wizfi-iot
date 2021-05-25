/*
 * test.c
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

/*********************************************************************************************/
/* Includes ---------------------------------------------------------------------------------*/

#include <string.h>

#include "../includes/wizfi360.h"
#include "../includes/wizfi360_opts.h"
#include "../includes/wizfi360_required.h"

/*********************************************************************************************/
/* Private constants ------------------------------------------------------------------------*/

/**
 *	A string table holding tags which can be received from wizfi360 module via UART.
 *	Each tag entry is assigned to it's tag identifier.
 */
static const char *WIZFI360_TAGS[WIZFI360_NUM_TAGS] = {
	[WIZFI360_TAG_ID_ERROR] = WIZFI360_TAG_ERROR,
	[WIZFI360_TAG_ID_ALREADY_CONNECTED] = WIZFI360_TAG_ALREADY_CONNECTED,
	[WIZFI360_TAG_ID_SEND_FAIL] = WIZFI360_TAG_SEND_FAIL,
	[WIZFI360_TAG_ID_OK] = WIZFI360_TAG_OK,
	[WIZFI360_TAG_ID_SEND_OK] = WIZFI360_TAG_SEND_OK,
	[WIZFI360_TAG_ID_READY] = WIZFI360_TAG_READY,
	[WIZFI360_TAG_ID_WIFI_CONNECTED] = WIZFI360_TAG_WIFI_CONNECTED,
	[WIZFI360_TAG_ID_WIFI_GOT_IP] = WIZFI360_TAG_WIFI_GOT_IP,
	[WIZFI360_TAG_ID_WIFI_DISCONNECT] = WIZFI360_TAG_WIFI_DISCONNECT,
	[WIZFI360_TAG_ID_BUSY_SENDING] = WIZFI360_TAG_BUSY_SENDING,
	[WIZFI360_TAG_ID_BUSY_PROCESSING] = WIZFI360_TAG_BUSY_PROCESSING,
	[WIZFI360_TAG_ID_LINK_ID_CONNECT] = WIZFI360_TAG_LINK_ID_CONNECT,
	[WIZFI360_TAG_ID_LINK_ID_CLOSED] = WIZFI360_TAG_LINK_ID_CLOSED,
	[WIZFI360_TAG_ID_NETWORK_DATA_RECEIVED] = WIZFI360_TAG_NETWORK_DATA_RECEIVED,
	[WIZFI360_TAG_ID_STA_CONNECTED] = WIZFI360_TAG_STA_CONNECTED,
	[WIZFI360_TAG_ID_DIST_STA_IP] = WIZFI360_TAG_DIST_STA_IP,
	[WIZFI360_TAG_ID_STA_DISCONNECTED] = WIZFI360_TAG_STA_DISCONNECTED,
	[WIZFI360_TAG_ID_FAIL] = WIZFI360_TAG_FAIL
};


/*********************************************************************************************/
/* Private function prototypes --------------------------------------------------------------*/

static void ResetDataStructure();
static void ScanBufferForTags(uint8_t tmpChar, int i);
static void ScanBufferForEcho(uint8_t tmpChar, int i);
static void ScanBufferForMqttTopics(uint8_t tmpChar, int i);
static void TagReceivedCallback(WIZFI360_TagIdTypeDef tagId, int length);
static void DefaultSubscribeCallback(char* message);

/*********************************************************************************************/

/* Private variables ---------------------------------------------------------*/

/*
 * The wizfi360 handler
 */
WIZFI360_HandlerTypedef wizfi360;

/*********************************************************************************************/

/* Public functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the WizFi360 handler and starts listening for UART data.
  * @note	This function should be called once before using other WIZFI360 functions.
  * @note	We assume, that the module is reset on startup. This assumption might be false.
  * 		To make sure, that the module is reset, either reset the module using the reset
  * 		pin, or restart the module using the AT+RST command.
  * @retval none
  */
void WIZFI360_Initialize()
{
	// Reset the wizfi360 data structure.
	ResetDataStructure();

	// Call user specific function
	WIZFI360_UART_StartContinousReception();
}

/**
  * @brief  Checks for new data received from WizFi360 module and handles received data.
  * @note   This function must be called continuously as background task in the main application.
  * @note   The underlying UART ring-buffer overflows, if data is not handled fast enough.
  * @retval none
  */
void WIZFI360_Process()
{
	// The amount of received bytes.
	uint32_t bytesAvailable = ring_buffer_num_items(&(wizfi360.UartRxBuffer));

	// TODO: Don't rescan already scanned bytes

	// Go through all bytes in ring buffer...
	for (int i = 0; i < bytesAvailable; i++)
	{
		// A temporary byte storage
		char tmpChar;

		// Peek the next byte from ring buffer
		ring_buffer_peek(&(wizfi360.UartRxBuffer), &tmpChar, i);

		// Check if ring buffer contains AT command echo
		ScanBufferForEcho(tmpChar, i+1);

		// Check if ring buffer contains mqtt message
		ScanBufferForMqttTopics(tmpChar, i+1);

		// Check if ring buffer contains wizfi360 tags
		ScanBufferForTags(tmpChar, i+1);
	}
}

/**
 * @brief	Resets the WizFi360 module
 * @note	After resetting the module we expect to receive WIZFI360_TAG_READY via UART
 * @retval	None
 */
void WIZFI360_Reset()
{
	#ifdef WIZFI360_EVB_MINI
		WIZFI360_ResetHard();
	#elif WIZFI360_EVB_SHIELD
		WIZFI360_AT_ResetModule();
	#endif
}

/**
 * @brief	Resets the WizFi360 module using the Reset Pin
 * @note	After resetting the module we expect to receive WIZFI360_TAG_READY via UART
 * @retval	None
 */
void WIZFI360_ResetHard()
{
	WIZFI360_PreResetHard();
	WIZFI360_WriteResetPinLow();
	WIZFI360_Delay(1);
	WIZFI360_WriteResetPinHigh();
	WIZFI360_Delay(10);

	WIZFI360_PostResetHard();
}


/**
 * @brief	Returns the state of the module.
 * @retval	The modules state
 */
WIZFI360_State WIZFI360_GetState()
{
	// If we are expecting a response (from AT command)
	if (wizfi360.ExpectingResponse)
	{
		// The module is busy processing the AT command.
		return WIZFI360_STATE_BUSY;
	}

	// The module is ready
	return WIZFI360_STATE_READY;
}

WIZFI360_WifiState WIZFI360_GetWifiState()
{
	return wizfi360.WifiState;
}

/**
 * @brief	Puts received UART-data into ring buffer.
 * @note	This function must be called by user on  UART data reception
 * @param	data	Pointer to UART data that is being received.
 * @param	size	Number of received bytes.
 * @retval	None
 */
void WIZFI360_UART_BytesReceived(const char *data, ring_buffer_size_t size)
{
	ring_buffer_queue_arr(&(wizfi360.UartRxBuffer), data, size);
}

/**
  * @brief  Associates a user defined callback function with a subscribe-topic.
  * @note   - The topic parameter must be a full topic path (without wildcards)
  * 		- The callback is called, when the subscribe-topic is received.
  * 		- In order to receive the topic, it must be configured in the module using
  * 		  the WIZFI360_AT_MqttSetTopic command.
  * @param	topic	The topic, that we subscribe to. (must be a '\0' terminated string!)
  * @param	func 	Pointer to the user-defined callback function.
  * @retval none
  */
void WIZFI360_RegisterSubTopicCallback(const char* topic, void (*func)(char*))
{
	if (wizfi360.NumSubTopicCallbacks >= WIZFI360_MAX_SUBTOPIC_CALLBACKS)
	{
		// TODO: Error Handling
		return;
	}

	if (strlen(topic) >= WIZFI360_MAX_SUBTOPIC_LEN)
	{
		// TODO: Error Handling
		return;
	}

	uint8_t topicId = wizfi360.NumSubTopicCallbacks;

	wizfi360.SubTopicCharsReceived[topicId] = 0;

	strcpy(wizfi360.SubTopics[topicId], topic);

	strcat(wizfi360.SubTopics[topicId], " -> ");

	wizfi360.SubTopicCallbacks[topicId] = func;

	wizfi360.NumSubTopicCallbacks++;
}

/*********************************************************************************************/
/* Private functions ---------------------------------------------------------*/

/*
 * TODO: Comment on ResetDataStructure
 */
static void ResetDataStructure()
{
	/* Initialize ring buffer */
	ring_buffer_init(&(wizfi360.UartRxBuffer));

	wizfi360.WifiMode = WIZFI360_MODE_STATION;

	wizfi360.EchoMode = WIZFI360_ECHO_ENABLE;

	wizfi360.ExpectingResponse = 0;

	wizfi360.ExpectingEcho = 0;

	wizfi360.WifiState = WIZFI360_WIFI_DISCONNECTED;

	wizfi360.CommandBuffer[0] = '\0';

	wizfi360.CommandLength = 0;

	wizfi360.CommandId = -1;

	wizfi360.EchoCharsReceived = 0;

	// TODO: Improve data structure reset for registered subtopic callbacks
	// >>>>>>>
	wizfi360.MessageIncoming = 0;

	for (int tagId = 0; tagId < WIZFI360_NUM_TAGS; tagId++)
	{
		wizfi360.TagCharsReceived[tagId] = 0;
	}

	wizfi360.NumSubTopicCallbacks = 0;

	for (int topicId = 0; topicId < WIZFI360_MAX_SUBTOPIC_CALLBACKS; topicId++)
	{
		wizfi360.SubTopicCharsReceived[topicId] = 0;
		wizfi360.SubTopics[topicId][0] = '\0';
		wizfi360.SubTopicCallbacks[topicId] = DefaultSubscribeCallback;
	}

	WIZFI360_RegisterSubscribeCallbacks();
	// <<<<<<<
}


/**
 * @brief	Checks if the UART receive buffer contains an echo to be handled.
 * @note	When sending AT commands, the modules sends back the command, if echo mode is not disabled.
 * @param	tmpChar	The character in UART receive buffer, that is being checked.
 * @param	i		The index in UART receive buffer that is being checked.
 * @retval	None
 */
static void ScanBufferForEcho(uint8_t tmpChar, int i)
{
	// If we do not except an echo...
	if (!wizfi360.ExpectingEcho)
	{
		// Do nothing.
		return;
	}

	// If we found a consecutive matching character in the tag...
	if (tmpChar == wizfi360.CommandBuffer[wizfi360.EchoCharsReceived])
	{
		// Increment the counter.
		wizfi360.EchoCharsReceived++;

		// If we found the full echo in UART receive buffer...
		if (wizfi360.EchoCharsReceived == wizfi360.CommandLength)
		{
			// A buffer to read the echo into (the length of the echo equals command length)
			uint8_t echo[wizfi360.CommandLength];
			// Read the echo from UART buffer
			ring_buffer_dequeue_arr(&(wizfi360.UartRxBuffer), (char*)echo, wizfi360.CommandLength);
			// We do not expect an echo anymore
			wizfi360.ExpectingEcho = 0;
			// Reset the counter.
			wizfi360.EchoCharsReceived = 0;
		}
	}
	// if we did not find a consecutive matching character, but instead the first character...
	else if (tmpChar == wizfi360.CommandBuffer[0])
	{
		// Set the counter to one.
		wizfi360.EchoCharsReceived = 1;
	}
	// else, if we did not find anything related to the tag...
	else
	{
		// Reset the counter.
		wizfi360.EchoCharsReceived = 0;
	}
}

/**
 * @brief	Checks if the UART receive buffer contains a tag to be handled.
 * @note	Tags can be received as response to an AT command.
 * @note	Tags can be received as event.
 * @param	tmpChar	The character in UART receive buffer, that is being checked.
 * @param	i		The index in UART receive buffer that is being checked.
 * @retval	None
 */
static void ScanBufferForTags(uint8_t tmpChar, int i)
{
	// For each tag that could be received from wizfi module...
	for (WIZFI360_TagIdTypeDef tagId = 0; tagId < WIZFI360_NUM_TAGS; tagId++)
	{
		// If we found a consecutive matching character in the tag...
		if (tmpChar == WIZFI360_TAGS[tagId][wizfi360.TagCharsReceived[tagId]])
		{
			// We found one more matching character for this tag.
			wizfi360.TagCharsReceived[tagId]++;

			// If we found a full tag in UART receive buffer...
			if (wizfi360.TagCharsReceived[tagId] == strlen(WIZFI360_TAGS[tagId]))
			{
				// Handle the tag
				TagReceivedCallback(tagId, i);

				// Reset the tag counter.
				wizfi360.TagCharsReceived[tagId] = 0;
			}
		}
		// if we did not find a consecutive matching character, but instead the first character...
		else if (tmpChar == WIZFI360_TAGS[tagId][0])
		{
			// Set the matchCounter to one.
			wizfi360.TagCharsReceived[tagId] = 1;
		}
		// else, if nothing related to the tag was found...
		else
		{
			// Reset the tag counter.
			wizfi360.TagCharsReceived[tagId] = 0;
		}
	}
}


/**
 * @brief	Checks if the UART receive buffer contains a message of subscribed topic.
 * @note	Topics can be received, when we are connected to the broker.
 * @note	When a topic is received, the associated user-defined callback is called.
 * @param	tmpChar	The character in UART receive buffer, that is being checked.
 * @param	i		The index in UART receive buffer that is being checked.
 * @retval	None
 */
static void ScanBufferForMqttTopics(uint8_t tmpChar, int i)
{
	/*
	 * MqttTopics come as follows: "subtopic -> message\r\n"
	 *
	 * This function contains of two parts:
	 *
	 * 		1)  Look for topic string: "subtopic -> "
	 *
	 * 		    When no message is incoming, we look for the topic string.
	 * 		    When the topic string was found, a message is incoming. The topic
	 * 		    id is stored in receivingTopic variable.
	 *
	 * 		2)  Look for termination string: "\r\n"
	 *
	 * 			When the message is incoming, we look for the CR+LF termination string.
	 * 			When the termination string is found, we read the message (in between
	 * 			of topic string and termination string) and raise the corresponding
	 * 			callback.
	 */

	// Represents the topicId, if a message is incoming
	static int receivingTopic = 0;
	// Helper variable used to detect termination string
	static int cr_found = 0;

	// If no message is incoming...
	if (!wizfi360.MessageIncoming)
	{
		// For each topic that could be received from wizfi module...
		for (int topicId = 0; topicId < wizfi360.NumSubTopicCallbacks; topicId++)
		{
			uint32_t CharsFound = wizfi360.SubTopicCharsReceived[topicId];
			char CharToFind = wizfi360.SubTopics[topicId][CharsFound];

			// If we found a consecutive matching character in the topic string...
			if (tmpChar == CharToFind)
			{
				// We found one more matching character for this topic string.
				wizfi360.SubTopicCharsReceived[topicId]++;

				// If we found a full topic string in UART receive buffer...
				if (wizfi360.SubTopicCharsReceived[topicId]
						== strlen(wizfi360.SubTopics[topicId]))
				{
					// The length of the topic string.
					const int length = strlen(wizfi360.SubTopics[topicId]);

					// A dummy storage for the topic string
					uint8_t data[length];
					// Read the topic string into the dummy storage (to clear the buffer)
					ring_buffer_dequeue_arr(&(wizfi360.UartRxBuffer), (char*)data, length);

					// Now we scan for the actual message...
					wizfi360.MessageIncoming = 1;
					cr_found = 0;
					// The message belongs to this topicId.
					receivingTopic = topicId;

					// Reset the counter.
					wizfi360.SubTopicCharsReceived[topicId] = 0;
				}
			}
			// if we did not find a consecutive matching character, but instead the first character...
			else if (tmpChar == wizfi360.SubTopics[topicId][0])
			{
				// Set the matchCounter to one.
				wizfi360.SubTopicCharsReceived[topicId] = 1;
			}
			// else, if nothing related to the tag was found...
			else
			{
				// Reset the tag counter.
				wizfi360.SubTopicCharsReceived[topicId] = 0;
			}
		}
	}
	// If there is a message incoming...
	else
	{
		// If we did not find '\r' yet...
		if (!cr_found)
		{
			// If the current char is '\r'...
			if (tmpChar == '\r')
			{
				// We found '\r'!
				cr_found = 1;
			}
		}
		// If we did find '\r' already....
		else
		{
			// And if the next char is '\n'...
			if (tmpChar == '\n')
			{
				// We found the message!

				// The length of the message
				uint32_t length = i - 2;
				// A storage for the message + '\0' null termination character
				char message[length + 1];
				// Read the message data
				ring_buffer_dequeue_arr(&(wizfi360.UartRxBuffer), message, length);
				// Append a null termination character.
				message[length] = '\0';

				// Raise the callback
				wizfi360.SubTopicCallbacks[receivingTopic](message);

				// Read the \r\n characters into dummy string (to clear the buffer)
				char dummy[2];
				ring_buffer_dequeue_arr(&(wizfi360.UartRxBuffer), dummy, 2);

				// The incoming message was fully read. We are ready for new messages.
				wizfi360.MessageIncoming = 0;
			}
			// If the next char was not '\n'...
			else
			{
				// We look for CR again.
				cr_found = 0;
			}
		}

	}
}


/**
 * @brief	Handles the received tag.
 * @note	Tags can be received as response to an AT command.
 * @note	Tags can be received as event.
 * @param	tagId	The tag, that was received.
 * @param	length	Amount of bytes in UART receive buffer (including the full tag)
 * @retval	None
 */
static void TagReceivedCallback(WIZFI360_TagIdTypeDef tagId, int length)
{
	uint8_t data[length];
	ring_buffer_dequeue_arr(&(wizfi360.UartRxBuffer), (char*)data, length);

	switch(tagId)
	{
		// If we received an AT Response...
		case WIZFI360_TAG_ID_OK:
		case WIZFI360_TAG_ID_ERROR:
		case WIZFI360_TAG_ID_FAIL:
		case WIZFI360_TAG_ID_SEND_OK:
		case WIZFI360_TAG_ID_SEND_FAIL:
		case WIZFI360_TAG_ID_ALREADY_CONNECTED:
		case WIZFI360_TAG_ID_BUSY_SENDING:
		case WIZFI360_TAG_ID_BUSY_PROCESSING:
		{
			// Handle the response.
			WIZFI360_AT_HandleResponse(tagId);
			break;
		}
		case WIZFI360_TAG_ID_READY:
		{
			ResetDataStructure();

			#ifdef WIZFI360_CALLBACK_USED_MODULE_READY
			WIZFI360_ModuleReadyCallback();
			#endif

			break;
		}
		case WIZFI360_TAG_ID_WIFI_CONNECTED:
		{
			wizfi360.WifiState = WIZFI360_WIFI_CONNECTED;
			#ifdef WIZFI360_CALLBACK_USED_WIFI_CONNECTED
			WIZFI360_WifiConnectedCallback();
			#endif
			break;
		}
		case WIZFI360_TAG_ID_WIFI_GOT_IP:
		{
			break;
		}
		case WIZFI360_TAG_ID_WIFI_DISCONNECT:
		{
			wizfi360.WifiState = WIZFI360_WIFI_DISCONNECTED;
			#ifdef WIZFI360_CALLBACK_USED_WIFI_CONNECTED
			WIZFI360_WifiDisconnectedCallback();
			#endif
			break;
		}
		default:
		{
			break;
		}
	}
}


/*********************************************************************************************/

/**
 * @brief	Default callback for subscribed topics.
 * @note	- This dummy-function is used, to initialize the SubTopicCallbacks function-array in
 * 			WIZFI360_HandlerTypedef. If we leave the array uninitialized, memory access violations
 * 			and undefined behavior might occur.
 *			- This function should never be called. When it's called, something went wrong.
 * @param	message	Unused dummy-parameter to avoid compilation errors.
 * @retval	None
 */
static void DefaultSubscribeCallback(char* message)
{

}

/*********************************************************************************************/
