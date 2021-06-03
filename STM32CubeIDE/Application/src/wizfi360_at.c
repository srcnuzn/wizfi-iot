/*
 * wizfi360_at.c
 *
 *  Created on: May 22, 2021
 *      Author: serca
 */

/*********************************************************************************************/
/* Includes ---------------------------------------------------------------------------------*/

// TODO: Don't use system libraries (implement custom functions instead)
#include <string.h>
#include <stdio.h>

#include "../includes/wizfi360.h"
#include "../includes/wizfi360_opts.h"
#include "../includes/wizfi360_required.h"

/*********************************************************************************************/
/* Imported variables -----------------------------------------------------------------------*/

extern WIZFI360_HandlerTypedef wizfi360;

/*********************************************************************************************/
/* Private function prototypes --------------------------------------------------------------*/

static inline void HandleResponse_Ok();
static inline void HandleResponse_Error();
static inline void HandleResponse_Fail();

/*********************************************************************************************/
/* Public function definitions --------------------------------------------------------------*/

/**
 * @brief	Tests the WizFi360 communication interface.
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_Test()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command (example: ATE1<CR><LF>)
	const int cmdLength =
			strlen("AT")
			+ 2;			// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_TEST;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT");
	strcat(wizfi360.CommandBuffer, "\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}

/**
 * @brief	Restarts the WizFi360 module.
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_Restart()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command (example: ATE1<CR><LF>)
	const int cmdLength =
			strlen("AT+RST")
			+ 2;			// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_RST;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+RST\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}


/**
 * @brief	Enables/Disables AT command echos.
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_SetEchoMode(WIZFI360_EchoModeTypeDef mode)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command (example: ATE1<CR><LF>)
	const int cmdLength =
			strlen("ATE")
			+ 1				// the mode is one character (either '0', '1')
			+ 2;			// command ends with <CR><LF>


	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_ECHO;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "ATE");

	switch (mode)
	{
		case WIZFI360_ECHO_ENABLE:
		{
			strcat(wizfi360.CommandBuffer, "1");
			break;
		}
		case WIZFI360_ECHO_DISABLE:
		{
			strcat(wizfi360.CommandBuffer, "0");
			break;
		}
		default:
		{
			break;
		}
	}
	strcat(wizfi360.CommandBuffer, "\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}


/**
 * @brief	Connects to an AP.
 * @note	Module must be in station mode.
 * @note	There must be no ongoing AT command.
 * @param	ssid   string parameter, the SSID of the target AP, MAX: 32 bytes
 * @param	pwd    string parameter, the password of the target AP, MAX: 64-byte ASCII.
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_ConnectToAccessPoint(const char* ssid, const char* pwd)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// If the module is not in station mode...
	if (wizfi360.WifiMode != WIZFI360_MODE_STATION)
	{
		return WIZFI360_ERROR;
	}

	// The length of the ssid
	const int ssidLength = strlen(ssid);

	// If the SSID is too long...
	if (ssidLength > WIZFI360_MAX_AP_SSID_LEN)
	{
		return WIZFI360_ERROR;
	}

	// The length of the password
	const int pwdLength  = strlen(pwd);

	// If the password is too long
	if (pwdLength > WIZFI360_MAX_AP_PWD_LEN)
	{
		return WIZFI360_ERROR;
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
		return WIZFI360_ERROR;
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

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
	return WIZFI360_OK;
}

/**
 * @brief	Sets the Current WiFi mode (Station, Soft-AP or both)
 * @note	There must be no ongoing AT command.
 * @param	mode   The mode to be set.
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_SetWifiMode(WIZFI360_WifiModeTypeDef mode)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command
	const int cmdLength =
		strlen("AT+CWMODE_CUR=")
		+ 1							// the mode is one character (either '1', '2' or '3')
		+ 2;						// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
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
			return WIZFI360_ERROR;
		}
	}

	strcat(wizfi360.CommandBuffer, "\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
	return WIZFI360_OK;
}


/**
 * @brief	Enables/Disables DHCP for the selected mode.
 * @note	There must be no ongoing AT command.
 * @param	mode   The mode to for which DHCP should be enabled/disabled
 * @param	dhcp   The DHCP setting (enabled/disbaled)
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_SetDhcpMode(WIZFI360_WifiModeTypeDef mode, WIZFI360_DhcpModeTypeDef dhcp)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
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
		return WIZFI360_ERROR;
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
			return WIZFI360_ERROR;
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
			return WIZFI360_ERROR;
		}
	}

	// Append <CR><LF>
	strcat(wizfi360.CommandBuffer, "\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}


/**
 * @brief	Gets the SSL certificate of the WizFi module.
 * @note	There must be no ongoing AT command.
 * @param	None
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_SetSSLCertificate()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
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
		return WIZFI360_ERROR;
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

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
	return WIZFI360_OK;
}


/**
 * @brief	Gets the SSL certificate of the WizFi module.
 * @note	There must be no ongoing AT command.
 * @param	None
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_GetSSLCertificate()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command
	const int cmdLength =
		strlen("AT+CASEND?")
		+ 2;						// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
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

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}
	return WIZFI360_OK;
}


/**
 * @brief	Sets the Configuration of MQTT connection.
 * @note	This command should be set before connecting to a broker.
 * @note	There must be no ongoing AT command.
 * @param	userName   	string parameter, User Name used in the broker authentication Max: 50byte
 * @param	pwd     	string parameter, Password used in the broker authentication. Max: 50byte
 * @param	ClientID    string parameter, Client ID connected to the broker. Max: 50byte
 * @param	aliveTime   keep-alive time setting with the broker within the range of 30s~300s.
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_ConfigureMqtt(const char* userName, const char*  pwd,
		const char* clientId, uint16_t aliveTime )
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the user name
	const int userNameLength = strlen(userName);

	// If the userName is too long...
	if (userNameLength > WIZFI360_MAX_MQTT_USERNAME_LEN)
	{
		return WIZFI360_ERROR;
	}

	// The length of the mqtt password
	const int pwdLength  = strlen(pwd);

	// If the password is too long
	if (pwdLength > WIZFI360_MAX_MQTT_PWD_LEN)
	{
		return WIZFI360_ERROR;
	}

	// TODO: Check aliveTime range

	// The length of the client id
	const int clientIdLength = strlen(clientId);

	// If the clientId is too long...
	if (clientIdLength > WIZFI360_MAX_MQTT_CLIENTID_LEN)
	{
		return WIZFI360_ERROR;
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
		return WIZFI360_ERROR;
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

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}


/**
 * @brief	Sets the Topic of Publish and Subscribe
 * @note	This command should be set before connecting to a broker.
 * @note	There must be no ongoing AT command.
 * @param	pubTopic	string parameter, The topic published on the WizFi360
 * @param	subTopic 	string parameter, The topic subscribed by the WizFi360
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_MqttSetTopic(const char* pubTopic, const char*  subTopic)
{

	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command (example: AT+MQTTTOPIC="pubTopic","subTopic1","SubTopic2","SubTopic3"<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTTOPIC=")
		+ 2						// publishTopic is wrapped in quotation marks
		+ 2						// each subscribeTopic is wrapped in quotation marks
		+ 1						// topics are separated by this amount of commas
		+ strlen(pubTopic)
		+ strlen(subTopic)
		+ 2;					// command ends with <CR><LF>
	
	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
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
	strcat(wizfi360.CommandBuffer, subTopic);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, "\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}

/**
 * @brief	Connects to a Broker
 * @note	There must be no ongoing AT command.
 * @note	Whenever messages of subscribe topic is received, it will return as below:
 * 			"<subscribe topic> -> subscribe data"
 * @param	authMode		Decides, whether to connect to a broker with/without authentication
 * @param	brokerAddr		string parameter indicating the broker IP address or domain-name
 * @param	brrokerPort 	the broker port number
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_MqttConnectToBroker(WIZFI360_MqttAuthModeTypeDef authMode,
		const char*  brokerAddr, uint16_t brokerPort)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// If the module is not in connected to wifi...
	if (wizfi360.WifiState != WIZFI360_WIFI_CONNECTED)
	{
		return WIZFI360_ERROR;
	}

	// Build string from alive time
	char sPort[6] = {0};
	sprintf(sPort, "%d", brokerPort);

	// The length of the command (example: AT+MQTTCON=0,"35.156.215.0",1883<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTCON=")
		+ 2						// IP address is wrapped in quotation marks
		+ 2						// the parameters are separated by this amount of commas
		+ 1						// authMode is one character long ('0' or '1')
		+ strlen(sPort)
		+ strlen(brokerAddr)
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
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
		case WIZFI360_MQTT_AUTH_ENABLE:
		{
			strcat(wizfi360.CommandBuffer, "1");
			break;
		}
		default:
		{
			return WIZFI360_ERROR;
		}
	}
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, brokerAddr);
	strcat(wizfi360.CommandBuffer, "\"");
	strcat(wizfi360.CommandBuffer, ",");
	strcat(wizfi360.CommandBuffer, sPort);
	strcat(wizfi360.CommandBuffer, "\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif
	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}

/*
 * TODO: WIZFI360_AT_MqttDisconnectFromBroker must be tested.
 * TODO: Check, what the response to this command might be
 */
/**
 * @brief	Disconnects from a Broker
 * @note	There must be no ongoing AT command.
 * @note	Whenever messages of subscribe topic is received, it will return as below:
 * 			<subscribe topic> -> "subscribe data"
 * @param	authMode		Decides, whether to connect to a broker with/without authentication
 * @param	mqttBrokerIP	string parameter indicating the broker IP address
 * @param	mqttBrokerPort 	the broker port number
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_MqttDisconnectFromBroker()
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}

	// The length of the command (AT+MQTTDIS<CR><LF>)
	const int cmdLength =
			strlen("AT+MQTTDIS")
			+ 2;			// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
	}

	// Write the command id into wizfi360 structure
	wizfi360.CommandId = WIZFI360_CMD_ID_MQTTDIS;

	// Write the command length into wizfi360 structure
	wizfi360.CommandLength = cmdLength;

	// Empty the command buffer string
	wizfi360.CommandBuffer[0] = '\0';

	// Build the command
	strcat(wizfi360.CommandBuffer, "AT+MQTTDIS\r\n");

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}


/**
 * @brief	Publishes a message.
 * @note	- There must be no ongoing AT command.
 * 			- This command can only be used after MQTT Connection is established.
 * 			- Topic of published data is defined by AT+MQTTTOPIC.
 * 			- You should set a topic of publish before connecting to a broker.
 * @param	message	Defines the message to be published (must be '\0' terminated!)
 * @retval	None
 */
WIZFI360_StatusTypeDef WIZFI360_AT_MqttPublishMessage(const char* message)
{
	// If there is an ongoing AT command...
	if (wizfi360.ExpectingResponse)
	{
		return WIZFI360_BUSY;
	}
	// TODO Change to: if is not connected to broker
	// If the module is not in station mode...
	if (wizfi360.WifiMode != WIZFI360_MODE_STATION)
	{
		return WIZFI360_ERROR;
	}

	// The length of the command (example: AT+MQTTPUB="HelloWorld!"<CR><LF>)
	const int cmdLength =
		strlen("AT+MQTTPUB=")
		+ 2						// the message is wrapped in quotation marks
		+ strlen(message)
		+ 2;					// command ends with <CR><LF>

	// If the command is too long...
	if (cmdLength >= WIZFI360_MAX_CMD_LEN)
	{
		return WIZFI360_ERROR;
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

#ifdef WIZFI360_UART_TX_MODE_NON_BLOCKING
	// Send the command
	WIZFI360_UART_SendNonBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength);
#else
	// Send the command
	WIZFI360_UART_SendBlockingMode((uint8_t*) wizfi360.CommandBuffer,
			wizfi360.CommandLength, 10000);
#endif

	// We expect a response for this command.
	wizfi360.ExpectingResponse = 1;

	// If echo mode is enabled...
	if (wizfi360.EchoMode == WIZFI360_ECHO_ENABLE)
	{
		// We expect an echo for this command
		wizfi360.ExpectingEcho = 1;
	}

	return WIZFI360_OK;
}

/*********************************************************************************************/

/**
 * @brief	Executes, when a response to an AT command is received
 * @param	tagId	The response tag, that was received.
 * @retval	None
 */
void WIZFI360_AT_HandleResponse(WIZFI360_TagIdTypeDef tagId)
{
	switch (tagId)
	{
		case WIZFI360_TAG_ID_OK: 		{	HandleResponse_Ok();	break; }
		case WIZFI360_TAG_ID_ERROR:		{	HandleResponse_Error(); break; }
		case WIZFI360_TAG_ID_FAIL:		{	HandleResponse_Fail();	break; }

		case WIZFI360_TAG_ID_SEND_OK:
		case WIZFI360_TAG_ID_SEND_FAIL:
		case WIZFI360_TAG_ID_ALREADY_CONNECTED:
			wizfi360.ExpectingResponse = 0;
			break;

		case WIZFI360_TAG_ID_BUSY_SENDING:
		case WIZFI360_TAG_ID_BUSY_PROCESSING:
		default:
			break;
	}
}

/*********************************************************************************************/
/* Private function definitions -------------------------------------------------------------*/


/**
 * @brief	Executes, if response ok is received after AT command.
 * @retval	None
 */
static inline void HandleResponse_Ok()
{
	wizfi360.ExpectingResponse = 0;

	switch (wizfi360.CommandId)
	{
		case WIZFI360_CMD_ID_ECHO:
		{
			if (strcmp(wizfi360.CommandBuffer, "ATE0") == 0)
			{
				wizfi360.EchoMode = WIZFI360_ECHO_DISABLE;
			}
			else if (strcmp(wizfi360.CommandBuffer, "ATE1") == 0)
			{
				wizfi360.EchoMode = WIZFI360_ECHO_ENABLE;
			}
			else
			{
				// TODO: Error handling.
			}
			break;
		}
		case WIZFI360_CMD_ID_TEST:
		case WIZFI360_CMD_ID_CWMODE_CUR:
		case WIZFI360_CMD_ID_CWJAP_CUR:
		case WIZFI360_CMD_ID_CWDHCP_CUR:
		case WIZFI360_CMD_ID_CASEND_GET:
		case WIZFI360_CMD_ID_CASEND_SET:
		case WIZFI360_CMD_ID_MQTTSET:
		case WIZFI360_CMD_ID_MQTTTOPIC:
		case WIZFI360_CMD_ID_MQTTCON:
		case WIZFI360_CMD_ID_MQTTPUB:
		case WIZFI360_CMD_ID_MQTTDIS:
		case WIZFI360_CMD_ID_RST:
			break;
		default:
			break;

	}

	// Fire Callback.
	wizfi360.CommandOkCallback();
}


/**
 * @brief	Executes, if response error is received after AT command.
 * @retval	None
 */
static inline void HandleResponse_Error()
{
	wizfi360.ExpectingResponse = 0;

	// Fire callback.
	wizfi360.CommandErrorCallback();
}

/**
 * @brief	Executes, if response fail is received after AT command.
 * @note	Fail could also be received, after wifi connection is lost, since
 * 			the module automatically tries to reconnect.
 * @retval	None
 */
static inline void HandleResponse_Fail()
{

	if (wizfi360.CommandId == WIZFI360_CMD_ID_CWJAP_CUR &&
			wizfi360.ExpectingResponse)
	{
		wizfi360.ExpectingResponse = 0;
	}

	// Fire callback.
	wizfi360.WifiConnectFailedCallback();
}

/*********************************************************************************************/

