/*
 * test.c
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

/*********************************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include "../includes/wizfi360_uart.h"
#include "../includes/wizfi360.h"

/*********************************************************************************************/

/* Private constants ----------------------------------------------------------*/

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

// TODO Create MQTT subscribe_topic list

/*********************************************************************************************/

/* Private function prototypes -----------------------------------------------*/

static void ScanUartReceiveBuffer(uint32_t bytesToScan);
static void ScanBufferForTags(uint8_t tmpChar, int i);
static void ScanBufferForEcho(uint8_t tmpChar, int i);
static void TagReceivedCallback(WIZFI360_TagIdTypeDef tagId, int length);
static void ErrorHandler();

// TODO Scan Buffer for MQTT topics
// TODO Create MQTT Message Receive callback
// TODO Disconnect from Broker
// TODO Test Command
// TODO Reset Command

/*********************************************************************************************/

/* Private variables ---------------------------------------------------------*/

/*
 * The wizfi360 handler
 */
static WIZFI360_HandlerTypedef wizfi360;


/*********************************************************************************************/

/* Public functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the WizFi360 module
  * @note	This function must be called once in application startup.
  * @retval none
  */
void WIZFI360_Initialize()
{
	WIZFI360_UART_Initialize();

	// TODO: DO PROPER RESET
	wizfi360.EchoEnabled = 1;
	wizfi360.ExpectingResponse = 0;
	wizfi360.Mode = WIZFI360_MODE_STATION;
	wizfi360.WifiState = WIZFI360_WIFI_DISCONNECTED;
}

/**
  * @brief  Checks for new data received from WizFi360 module and handles received data.
  * @note   This function must be called continuously as background task in the main application.
  * @note   The underlying UART ring-buffer overflows, if data is not handled fast enough.
  * @retval none
  */
void WIZFI360_Process()
{
	// TODO: CHECK FOR TIMEOUTS?

	// The amount of received bytes.
	uint32_t bytesAvailable = WIZFI360_UART_DataAvailable();

	// If no UART data is available...
	if (!bytesAvailable)
	{
		// Do nothing.
		return;
	}

	ScanUartReceiveBuffer(bytesAvailable);
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
	if (ssidLength > WIZFI360_MAX_AP_SSID_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the password
	const int pwdLength  = strlen(pwd);

	// If the password is too long
	if (pwdLength > WIZFI360_MAX_AP_PWD_LEN)
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
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);

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
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);

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
 * @brief	Gets the SSL certificate of the WizFi module.
 * @note	There must be no ongoing AT command.
 * @param	None
 * @retval	None
 */
void WIZFI360_SetSSLCertificate()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// TODO Finish setting SSL cert function

	// The length of the command
	const int cmdLength =
		strlen("AT+CASEND=")
		+1							// mode 0: delete cert, 1: generate cert
		//+ strlen(CLIENT_SSL_CERT)		// length of certificate
		+ 6;						// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_CASEND_SET;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+CASEND?");

	// Append <CR><LF>
	strcat(wizfi360.CommandBuffer, "\r\n");

	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);

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
 * @brief	Gets the SSL certificate of the WizFi module.
 * @note	There must be no ongoing AT command.
 * @param	None
 * @retval	None
 */
void WIZFI360_GetSSLCertificate()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the command
	const int cmdLength =
		strlen("AT+CASEND?")
		+ 2;						// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_CASEND_GET;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+CASEND?");

	// Append <CR><LF>
	strcat(wizfi360.CommandBuffer, "\r\n");

	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);

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
 * TODO: Check aliveTime range
 * @brief	Sets the Configuration of MQTT connection.
 * @note	This command should be set before connecting to a broker.
 * @note	There must be no ongoing AT command.
 * @param	userName   	string parameter, User Name used in the broker authentication Max: 50byte
 * @param	pwd     	string parameter, Password used in the broker authentication. Max: 50byte
 * @param	ClientID    string parameter, Client ID connected to the broker. Max: 50byte
 * @param	aliveTime   keep-alive time setting with the broker within the range of 30s~300s.
 * @retval	None
 */
void WIZFI360_MqttInit(const char* userName, const char*  pwd,
		const char* clientId, uint16_t aliveTime )
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

	// The length of the user name
	const int userNameLength = strlen(userName);

	// If the userName is too long...
	if (userNameLength > WIZFI360_MAX_MQQT_USERNAME_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the mqtt password
	const int pwdLength  = strlen(pwd);

	// If the password is too long
	if (pwdLength > WIZFI360_MAX_MQQT_PWD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// The length of the client id
	const int clientIdLength = strlen(clientId);

	// If the clientId is too long...
	if (clientIdLength > WIZFI360_MAX_MQQT_CLIENTID_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Build string from alive time
	char sAliveTime[6] = {0};
	sprintf(sAliveTime, "%d", aliveTime);



	// The length of the command (example: AT+MQTTSET="user","password","id",30<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTSET=")
		+ 6						// username, password and clientId are wrapped in quotation marks
		+ 3						// and are separated by this amount of commas
		+ userNameLength
		+ pwdLength
		+ clientIdLength
		+ strlen(sAliveTime)	// the length of alive time (as string)
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_MQTTSET;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+MQTTSET=");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, userName);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, pwd);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, clientId);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, sAliveTime);
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
 * TODO: NULL check
 * TODO: (strlen < 1) check
 * TODO: Comment subTopic2 / subtopic3
 * @brief	Sets the Topic of Publish and Subscribe
 * @note	This command should be set before connecting to a broker.
 * @note	There must be no ongoing AT command.
 * @param	pubTopic	string parameter, The topic published on the WizFi360
 * @param	subscribeTopic 	string parameter, The topic subscribed by the WizFi360
 * @retval	None
 */
void WIZFI360_MqttSetTopic(const char* pubTopic, const char*  subTopic1,
		const char* subTopic2, const char* subTopic3)
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

	// The length of the command (example: AT+MQTTTOPIC="pubTopic","subTopic1","SubTopic2","SubTopic3"<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTTOPIC=")
		+ 2						// publishTopic is wrapped in quotation marks
		+ 2						// each subscribeTopic is wrapped in quotation marks
		+ 1						// topics are separated by this amount of commas
		+ strlen(pubTopic)
		+ strlen(subTopic1)
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_MQTTTOPIC;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+MQTTTOPIC=");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, pubTopic);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, subTopic1);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, "\r\n");

	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);

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
 * @brief	Connects to a Broker
 * @note	There must be no ongoing AT command.
 * @note	Whenever messages of subscribe topic is received, it will return as below:
 * 			<subscribe topic> -> "subscribe data"
 * @param	authMode		Decides, whether to connect to a broker with/without authentication
 * @param	mqttBrokerIP	string parameter indicating the broker IP address
 * @param	mqttBrokerPort 	the broker port number
 * @retval	None
 */
void WIZFI360_MqttConnectToBroker(WIZFI360_MqqtAuthModeTypeDef authMode,
		const char*  mqttBrokerIP, uint16_t mqttBrokerPort)
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

	// Build string from alive time
	char sPort[6] = {0};
	sprintf(sPort, "%d", mqttBrokerPort);

	// The length of the command (example: AT+MQTTCON=0,"35.156.215.0",1883<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTCON=")
		+ 2						// IP address is wrapped in quotation marks
		+ 2						// the parameters are separated by this amount of commas
		+ 1						// authMode is one character long ('0' or '1')
		+ strlen(sPort)
		+ strlen(mqttBrokerIP)
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_MQTTCON;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+MQTTCON=");

	switch (authMode)
	{
		case WIZFI360_MQTT_AUTH_DISABLE:
		{
			strcat(wizfi360.CommandBuffer, "0");
			break;
		}
		case WIZFI360_MQQT_AUTH_ENABLE:
		{
			strcat(wizfi360.CommandBuffer, "1");
			break;
		}
		default:
		{
			// TODO: Error handling
			ErrorHandler();
		}
	}
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, mqttBrokerIP);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, sPort);
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
 * @brief	Connects to a Broker TODO COMMENT
 * @note	There must be no ongoing AT command.
 * @note	Whenever messages of subscribe topic is received, it will return as below:
 * 			<subscribe topic> -> "subscribe data"
 * @param	authMode		Decides, whether to connect to a broker with/without authentication
 * @param	mqttBrokerIP	string parameter indicating the broker IP address
 * @param	mqttBrokerPort 	the broker port number
 * @retval	None
 */
void WIZFI360_MqttPublishMessage(const char* message)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		// TODO: Error handling
		ErrorHandler();
	}
	// TODO if is not connected to broker
	// If the module is not in station mode...
	if (wizfi360.Mode != WIZFI360_MODE_STATION)
	{
		// TODO: Error handling
		ErrorHandler();
	}


	// The length of the command (example: AT+MQTTCON=0,"35.156.215.0",1883<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTPUB=")
		+ 2						// the message is wrapped in quotation marks
		+ strlen(message)
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		// TODO: Error handling
		ErrorHandler();
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_MQTTPUB;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+MQTTPUB=");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, message);
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

/*********************************************************************************************/
/* Private functions ---------------------------------------------------------*/

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

/*********************************************************************************************/


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
	uint8_t buffer[length];
	WIZFI360_UART_ReadArrayFromBuffer(buffer, length);

	switch(tagId)
	{
		// If we received the OK tag...
		// TODO: handle tags received
		case WIZFI360_TAG_ID_OK:
		{
			wizfi360.ExpectingResponse = 0;
			WIZFI360_CommandCpltCallback(wizfi360.CommandId,
					WIZFI360_RESPONSE_OK);
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
			wizfi360.ExpectingResponse = 0;
			WIZFI360_CommandCpltCallback(wizfi360.CommandId,
					WIZFI360_RESPONSE_ERROR);
			ErrorHandler();
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
			WIZFI360_WifiConnectFailedCallback();

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
			wizfi360.WifiState = WIZFI360_WIFI_CONNECTED;
			break;
		}
		case WIZFI360_TAG_ID_WIFI_GOT_IP:
		{
			ErrorHandler();
			break;
		}
		case WIZFI360_TAG_ID_WIFI_DISCONNECT:
		{
			wizfi360.WifiState = WIZFI360_WIFI_DISCONNECTED;
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
 * @brief	Handles errors.
 * @note	Useful for debugging purposes.
 * @retval	None
 */
static void ErrorHandler()
{
	__NOP();
}

/*********************************************************************************************/

/**
  * @brief Response OK received
  * @retval None
  */
__weak void WIZFI360_CommandCpltCallback(WIZFI360_CommandIdTypeDef command,
		WIZFI360_ResponseTypeDef response)
{

  /* NOTE : This function should not be modified, when the callback is needed,
            the WIZFI360_CommandCpltCallback can be implemented in the user file.
   */
}

__weak void WIZFI360_WifiConnectedCallback()
{

}

__weak void WIZFI360_WifiConnectFailedCallback()
{

}

