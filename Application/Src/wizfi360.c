/*
 * test.c
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

/*********************************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <wizfi360_uart.h>
#include "wizfi360.h"
#include "string.h"

/*********************************************************************************************/

/**
 *	TODO: Comment
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

/* Includes ------------------------------------------------------------------*/

static void ScanUartReceiveBuffer(uint32_t bytesToScan);
static void ScanBufferForTags(uint8_t tmpChar, int i);
static void ScanBufferForEcho(uint8_t tmpChar, int i);
static void TagReceivedCallback(WIZFI360_TagIdTypeDef tagId, int length);
static void ErrorHandler();


/*********************************************************************************************/

/*
 * A global wizfi360 handler
 */
static WIZFI360_HandlerTypedef wizfi360;


/*********************************************************************************************/

/**
  * @brief  Initializes the WizFi360 module
  * @note	This function must be called once in application startup.
  * @retval none
  */
void WIZFI360_Initialize()
{
	WIZFI360_UART_Initialize();

	wizfi360.EchoEnabled = 1;
	wizfi360.ExpectingResponse = 0;
	wizfi360.Mode = WIZFI360_MODE_STATION;
}

/**
  * @brief  Checks for new data received from WizFi360 module and handles received data.
  * @note   This function must be called continuously as background task in the main application.
  * @note   The underlying UART ring-buffer overflows, if data is not handled fast enough.
  * @note   The underlying UART ring-buffer overflows, if data is not handled fast enough.
  * @retval none
  */
void WIZFI360_Process()
{
	// TODO: CHECK FOR TIMEOUTS

	// If no UART data from wizfi is available...
	uint32_t bytesAvailable = WIZFI360_UART_DataAvailable();

	if (!bytesAvailable)
	{
		// Do nothing.
		return;
	}

	ScanUartReceiveBuffer(bytesAvailable);
}

/**
 *	TODO: Comment
 */
WIZFI360_State WIZFI360_GetState()
{
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_STATE_BUSY;
	}

	return WIZFI360_STATE_READY;
}

/**
 * @brief	Connects to an AP.
 * @note	Module must be in station mode.
 * @note	There must be no ongoing AT command.
 * @param	ssid   string parameter, the SSID of the target AP, MAX: 32 bytes
 * @param	pwd    string parameter, the password of the target AP, MAX: 64-byte ASCII.
 * @retval	None
 */
void WIZFI360_ConnectToAccessPoint(const char* ssid, const char* pwd)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		// TODO: Error handling
		ErrorHandler();
	}
	// If the module is not in station mode...
	if (wizfi360.Mode != WIZFI360_MODE_STATION)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the ssid
	const int ssidLength = strlen(ssid);

	// If the SSID is too long...
	if (ssidLength > WIZFI360_MAX_SSID_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the password
	const int pwdLength  = strlen(pwd);

	// If the password is too long
	if (pwdLength > WIZFI360_MAX_PWD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the command (example: AT+CWJAP_CUR="ssid","pwd"<CR><LF>)
	const int cmdLength =
		strlen("AT+CWJAP_CUR=")
		+ 5						// ssid and password are wrapped in quotation marks and separated by comma
		+ ssidLength
		+ pwdLength
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_CWJAP_CUR;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+CWJAP_CUR=");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ssid);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, pwd);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, "\r\n");

	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer, wizfi360.CommandLength, 10000);

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoEnabled)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
}

/**
 * @brief	Sets the Current WiFi mode (Station, Soft-AP or both)
 * @note	There must be no ongoing AT command.
 * @param	mode   The mode to be set.
 * @retval	None
 */
void WIZFI360_ConfigureMode(WIZFI360_ModeTypeDef mode)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the command
	const int cmdLength =
		strlen("AT+CWMODE_CUR=")
		+ 1							// the mode is one character (either '1', '2' or '3')
		+ 2;						// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_CWMODE_CUR;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';


	// Build the command

	strcat(wizfi360.CommandBuffer, "AT+CWMODE_CUR=");

	switch (mode)
	{
		case WIZFI360_MODE_STATION:
		{
			strcat(wizfi360.CommandBuffer, "1");
			break;
		}
		case WIZFI360_MODE_SOFT_AP:
		{
			strcat(wizfi360.CommandBuffer, "2");
			break;
		}
		case WIZFI360_MODE_BOTH:
		{
			strcat(wizfi360.CommandBuffer, "3");
			break;
		}
		default:
		{
			// TODO: Error handling
			ErrorHandler();
		}
	}

	strcat(wizfi360.CommandBuffer, "\r\n");

	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer, wizfi360.CommandLength, 10000);

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoEnabled)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
}


/**
 * @brief	Enables/Disables DHCP for the selected mode.
 * @note	There must be no ongoing AT command.
 * @param	mode   The mode to for which DHCP should be enabled/disabled
 * @param	dhcp   The DHCP setting (enabled/disbaled)
 * @retval	None
 */
void WIZFI360_ConfigureDhcp(WIZFI360_ModeTypeDef mode, WIZFI360_DhcpModeTypeDef dhcp)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the command
	const int cmdLength =
		strlen("AT+CWDHCP_CUR=")
		+ 1							// the mode is one character (either '1', '2' or '3')
		+ 1							// mode and dhcp are separated by comma
		+ 1							// the dhcp setting is one character (either '0' or '1')
		+ 2;						// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_CWDHCP_CUR;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';


	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+CWDHCP_CUR=");

	switch (mode)
	{
		case WIZFI360_MODE_STATION:
		{
			// Append '1'
			strcat(wizfi360.CommandBuffer, "1");
			break;
		}
		case WIZFI360_MODE_SOFT_AP:
		{
			// Append '2'
			strcat(wizfi360.CommandBuffer, "2");
			break;
		}
		case WIZFI360_MODE_BOTH:
		{
			// Append '3'
			strcat(wizfi360.CommandBuffer, "3");
			break;
		}
		default:
		{
			// TODO: Error handling
			ErrorHandler();
		}
	}

	// Append <comma>
	strcat(wizfi360.CommandBuffer, ",");

	switch (dhcp)
	{
		case WIZFI360_DHCP_DISABLE:
		{
			// Append '0'
			strcat(wizfi360.CommandBuffer, "0");
			break;
		}
		case WIZFI360_DHCP_ENABLE:
		{
			// Append '1'
			strcat(wizfi360.CommandBuffer, "1");
			break;
		}
		default:
		{
			// TODO: Error handling
			ErrorHandler();
		}
	}

	// Append <CR><LF>
	strcat(wizfi360.CommandBuffer, "\r\n");

	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer, wizfi360.CommandLength, 10000);

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoEnabled)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
}

/*********************************************************************************************/

/**
  * @brief  Scans the received data for certain tags and strings.
  * @retval none
  */
static void ScanUartReceiveBuffer(uint32_t bytesToScan)
{
	// For each byte to be scanned...
	for (int i = 0; i < bytesToScan; i++)
	{
		// A storage for the current byte to be peeked
		uint8_t tmpChar;

		// Peek the next byte to be scanned from UART buffer
		WIZFI360_UART_PeekFromBuffer(&tmpChar, i);

		ScanBufferForEcho(tmpChar, i+1);

		// TODO: ScanBufferForMqqtMessages();

		ScanBufferForTags(tmpChar, i+1);
	}
}

/**
 *	TODO: Comment
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
			WIZFI360_UART_ReadArrayFromBuffer(echo, wizfi360.CommandLength);
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
 *	TODO: Comment
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

/*********************************************************************************************/

/**
 *	TODO: Comment
 */
static void TagReceivedCallback(WIZFI360_TagIdTypeDef tagId, int length)
{
	uint8_t buffer[length];
	WIZFI360_UART_ReadArrayFromBuffer(buffer, length);

	switch(tagId)
	{
		// If we received the OK tag...
		case WIZFI360_TAG_ID_OK:
		{
			wizfi360.ExpectingResponse = 0;
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_SEND_OK:
		{
			ErrorHandler();
			wizfi360.ExpectingResponse = 0;
			break;
		}
		case WIZFI360_TAG_ID_ERROR:
		{
			ErrorHandler();
			wizfi360.ExpectingResponse = 0;
			break;
		}
		case WIZFI360_TAG_ID_ALREADY_CONNECTED:
		{
			wizfi360.ExpectingResponse = 0;
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_SEND_FAIL:
		{
			ErrorHandler();
			wizfi360.ExpectingResponse = 0;
			break;
		}
		case WIZFI360_TAG_ID_FAIL:
		{
			ErrorHandler();
			if (wizfi360.CommandId == WIZFI360_CMD_ID_CWJAP_CUR &&
					wizfi360.ExpectingResponse)
			{
				wizfi360.ExpectingResponse = 0;
			}
			break;
		}
		case WIZFI360_TAG_ID_READY:
		{
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_WIFI_CONNECTED:
		{
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_WIFI_GOT_IP:
		{
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_WIFI_DISCONNECT:
		{
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_BUSY_SENDING:
		{
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_BUSY_PROCESSING:
		{
			ErrorHandler();
			break;
		}
		default:
		{
			ErrorHandler();
			break;
		}
	}
}

/*********************************************************************************************/

/**
 *	TODO: Comment
 */
static void ErrorHandler()
{
	__NOP();
}

/*********************************************************************************************/
