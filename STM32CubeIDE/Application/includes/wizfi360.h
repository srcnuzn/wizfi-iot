/*
 * wizfi360.h
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 *
 *  TODO: Add Comment for wizfi360.h
 *
 */

#ifndef INCLUDES_WIZFI360_H_
#define INCLUDES_WIZFI360_H_

/*********************************************************************************************/
/* Includes ---------------------------------------------------------------------------------*/

#include <stdint.h>
#include "./wizfi360_opts.h"
#include "../thirdparty/ringbuffer.h"

/*********************************************************************************************/
/* Defines ----------------------------------------------------------------------------------*/

#define WIZFI360_MAX_AP_PWD_LEN 			 64
#define WIZFI360_MAX_AP_SSID_LEN 			 32
#define WIZFI360_MAX_MQTT_USERNAME_LEN		 50
#define WIZFI360_MAX_MQTT_PWD_LEN			 50
#define WIZFI360_MAX_MQTT_CLIENTID_LEN		 50

#define WIZFI360_MAX_SUBTOPIC_SCAN_LEN		WIZFI360_MAX_SUBTOPIC_LEN + 4

#define WIZFI360_NUM_TAGS 	18

#define WIZFI360_TAG_ERROR				 	"\r\nERROR\r\n"
#define WIZFI360_TAG_ALREADY_CONNECTED	 	"\r\nALREADY CONNECTED\r\n"
#define WIZFI360_TAG_SEND_FAIL			 	"\r\nSEND FAIL\r\n"
#define WIZFI360_TAG_OK					 	"\r\nOK\r\n"
#define WIZFI360_TAG_SEND_OK				"\r\nSEND OK\r\n"
#define WIZFI360_TAG_FAIL					"\r\nFAIL\r\n"
#define WIZFI360_TAG_READY				 	"\r\nready\r\n"
#define WIZFI360_TAG_WIFI_CONNECTED		 	"WIFI CONNECTED\r\n"
#define WIZFI360_TAG_WIFI_GOT_IP			"WIFI GOT IP\r\n"
#define WIZFI360_TAG_WIFI_DISCONNECT	 	"WIFI DISCONNECT\r\n"		/*!< Note:	Datasheet states "WIFI DISCONNECTED\r\n"
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 				but module sends "WIFI DISCONNECT\r\n" */
#define WIZFI360_TAG_BUSY_SENDING		 	"busy s...\r\n"
#define WIZFI360_TAG_BUSY_PROCESSING		"busy p...\r\n"
#define WIZFI360_TAG_LINK_ID_CONNECT		", CONNECT"
#define WIZFI360_TAG_LINK_ID_CLOSED			", CLOSED"
#define WIZFI360_TAG_NETWORK_DATA_RECEIVED	"+IPD"
#define WIZFI360_TAG_STA_CONNECTED			"+STA_CONNECTED:"
#define WIZFI360_TAG_DIST_STA_IP			"+DIST_STA_IP:"
#define WIZFI360_TAG_STA_DISCONNECTED		"+STA_DISCONNECTED:"


/*********************************************************************************************/
/* Type definitions -------------------------------------------------------------------------*/

/**
  * @brief 	WIZFI360 tag id definition
  * @note	enum members must be without custom identifiers
  */
typedef enum
{
	WIZFI360_TAG_ID_ERROR,					/*!< AT command input error or execution error */
	WIZFI360_TAG_ID_ALREADY_CONNECTED,		/*!< The TCP, UDP or SSL connection is already established. */
	WIZFI360_TAG_ID_SEND_FAIL,				/*!< The network data transmission is failed. */
	WIZFI360_TAG_ID_OK,						/*!< Without Payload: 	Set command is executed correctly.
												 With Payload:		1]  +<Command>: <para1>,<para2>, ..,<paran>
																		OK

																		Query or Execute command is executed
																		correctly and return the parameter value.

																	2]	<description>
																		OK

																		Query or Execute command is executed
																		correctly and return the specific value.
																		<description> means the returned values, and
																		the returned values are different for each
																		command.
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 */
	WIZFI360_TAG_ID_SEND_OK,				/*!< The network data transmission is success. */
	WIZFI360_TAG_ID_FAIL,					/*!< Connecting to AP failed or connection is lost. */
	WIZFI360_TAG_ID_READY,					/*!< The AT firmware is ready. */
	WIZFI360_TAG_ID_WIFI_CONNECTED,			/*!< WizFi360 Station connected to the AP */
	WIZFI360_TAG_ID_WIFI_GOT_IP,			/*!< WizFi360 Station got IP address from the AP */
	WIZFI360_TAG_ID_WIFI_DISCONNECT,		/*!< WizFi360 Station disconnected from the AP */
	WIZFI360_TAG_ID_BUSY_SENDING,			/*!< It means busy sending. WizFi360 is sending for previous input, cannot
											 	 response to the new input. */
	WIZFI360_TAG_ID_BUSY_PROCESSING,		/*!< It means busy processing. WizFi360 is processing for previous input,
												 cannot response to the new input. */
	WIZFI360_TAG_ID_LINK_ID_CONNECT,		/*!< A network connection of which <Link ID> */
	WIZFI360_TAG_ID_LINK_ID_CLOSED,			/*!< A network close of which <Link ID> */
	WIZFI360_TAG_ID_NETWORK_DATA_RECEIVED,	/*!< Received network data. */
	WIZFI360_TAG_ID_STA_CONNECTED,			/*!< A Station connects to the WizFi360 SoftAP */
	WIZFI360_TAG_ID_DIST_STA_IP,			/*!< WizFi360 SoftAP distributes an IP address to the Station connected. */
	WIZFI360_TAG_ID_STA_DISCONNECTED,		/*!< A Station disconnects from the WizFi360 SoftAP. */
} WIZFI360_TagIdTypeDef;

/**
  * @brief WIZFI360 Status definitions
  */
typedef enum
{
	WIZFI360_OK,			/*!< Disable Command Echos */
	WIZFI360_BUSY,			/*!< Disable Command Echos */
	WIZFI360_ERROR,			/*!< Enable Command Echos */
	WIZFI360_TIMEOUT,		/*!< Enable Command Echos */
}WIZFI360_StatusTypeDef;


/**
  * @brief WIZFI360 mode definition
  */
typedef enum
{
	WIZFI360_MODE_STATION,				/*!< Station Mode */
	WIZFI360_MODE_SOFT_AP,				/*!<  SoftAP mode (factory default) */
	WIZFI360_MODE_BOTH,					/*!<  Station + SoftAP mode */
} WIZFI360_WifiModeTypeDef;


/**
  * @brief WIZFI360 mode definition
  */
typedef enum
{
	WIZFI360_STATE_READY,				/*!< The module is ready for AT command reception. */
	WIZFI360_STATE_BUSY,				/*!< The module is busy. An AT command is already ongoing. */
	WIZFI360_STATE_UNKNOWN,				/*!< The modules state is unknown. */
} WIZFI360_State;

/**
  * @brief WIZFI360 mode definition
  */
typedef enum
{
	WIZFI360_WIFI_DISCONNECTED,				/*!< Wifi is not connected. */
	WIZFI360_WIFI_CONNECTED,				/*!< Wifi is connected.*/
} WIZFI360_WifiState;


/**
  * @brief WIZFI360 DHCP mode definition
  */
typedef enum
{
	WIZFI360_DHCP_ENABLE,				/*!< Enable DHCP */
	WIZFI360_DHCP_DISABLE				/*!< Disable DHCP */
}WIZFI360_DhcpModeTypeDef;

/**
  * @brief WIZFI360 Echo mode definition
  */
typedef enum
{
	WIZFI360_ECHO_DISABLE,				/*!< Disable Command Echos */
	WIZFI360_ECHO_ENABLE,				/*!< Enable Command Echos */
}WIZFI360_EchoModeTypeDef;


/**
  * @brief WIZFI360 MQTT authentication mode definition
  */
typedef enum
{
	WIZFI360_MQTT_AUTH_ENABLE,				/*!< Enables MQTT authentication (userName/password needed) */
	WIZFI360_MQTT_AUTH_DISABLE				/*!< Disable MQTT authentication (userName/password not needed) */
}WIZFI360_MqttAuthModeTypeDef;


/**
  * @brief WIZFI360 command id definition
  */
typedef enum
{
	WIZFI360_CMD_ID_CWMODE_CUR,			/*!< 	Sets the Current WiFi mode; Not Saved in the Flash
												Usage:		AT+CWMODE_CUR=<mode>
															<mode>: 1: Station mode
																	2: SoftAP mode (factory default)
																	3: Station + SoftAP mode
	 	 	 	 	 	 	 	 	 	 	 	Response:	OK
	 	 	 	 	 	 	 	 	 	*/
	WIZFI360_CMD_ID_CWJAP_CUR,			/*!<	Connects to an AP; Configuration Not Saved in the Flash
												Usage:		AT+CWJAP_CUR=<ssid>,<pwd>[,<bssid>]
															<ssid>:	 string parameter, the SSID of the target AP, MAX: 32 bytes
															<pwd>:	 string parameter, the password of the target AP, MAX: 64-byte ASCII.
															<bssid>: string parameter, the MAC address of the target AP, used when multiple
																	 APs have the same SSID.
												Responses:	1]	OK
															2]	+CWJAP:<error code> FAIL"
																<error code>	1: connection timeout.
																				2: wrong password.
																				3: cannot find the target AP.
																				4: connection failed.
										*/
	WIZFI360_CMD_ID_CWDHCP_CUR,			/*!< 	Enables/Disables DHCP; Configuration Not Saved in the Flash
												Usage:		AT+CWDHCP_CUR=<mode>,<en>
															<mode>	0: Sets WizFi360 SoftAP
																	1: Sets WizFi360 Station
																	2: Sets both SoftAP and Station
															<en>
																	0: Disables DHCP
																	1: Enables DHCP
												Response:	OK
										*/
	WIZFI360_CMD_ID_CASEND_GET,			/*!< 	Get the SSL certificate
													Usage:		AT+CASEND?
													Response:	Certificate or ERROR
										*/
	WIZFI360_CMD_ID_CASEND_SET,			/*!< 	Set the SSL certificate
													Usage:		AT+CASEND=<mode>
																<mode>	0: Delete a certificate
																		1: Generate a certificate
													Response:	>, then put in cert, then OK
										*/
	WIZFI360_CMD_ID_MQTTSET,			/*!<	Sets the Configuration of MQTT connection.
												Usage:		AT+MQTTSET=<User Name>,<Password>,<ClientID>,<AliveTime>
															<User Name> string parameter, User Name used in the broker
																		authentication Max: 50byte
															<Password>	string parameter, Password used in the broker
																		authentication. Max: 50byte
															<ClientID>	string parameter, Client ID connected to the broker.
																		Max: 50byte
															<AliveTime>	keep-alive time setting with the broker within the
																		range of 30s~300s.
												Response:	OK
										*/
	WIZFI360_CMD_ID_MQTTTOPIC,			/*!<	Sets the Topic of Publish and Subscribe.
												Usage:		AT+MQTTTOPIC=<publish topic>,<subscribe topic1>[,<subscribe topic2>][,<subscribe topic3>]
															<publish topic>: string parameter, The topic published on the WizFi360
															<subscribe topic1>: string parameter, The topic subscribed by the WizFi360
															[<subscribe topic2>]: string parameter, The topic subscribed by the WizFi360
															[<subscribe topic3>]: string parameter, The topic subscribed by the WizFi360
												Response:	OK
										*/
	WIZFI360_CMD_ID_MQTTCON,			/*!< 	Connects to a Broker
												Usage:		AT+MQTTCON=<enable>,<broker IP>,<broker port>
															<enable>:
																• 0: Connect to a broker without authentication
																• 1: Connect to a broker with authentication
															<broker IP>: string parameter indicating the broker IP address
															<broker port>: the broker port number
												Response:	OK
										*/
	WIZFI360_CMD_ID_MQTTPUB,			/*!< 	Publish a message
												Usage: 	  AT+MQTTDIS
												Response: CLOSE
	 	 	 	 	 	 	 	 	 	*/
	WIZFI360_CMD_ID_MQTTDIS,			/*!< 	Disconnects from a Broker
												Usage: 	  	AT+MQTTPUB=<message>
															<message>: string parameter, Publish the message to subscribed Client
												Response: 	OK
	 	 	 	 	 	 	 	 	 	*/
	WIZFI360_CMD_ID_TEST,				/*!< 	Tests AT Startup
												Usage: 	  AT
												Response: OK
	 	 	 	 	 	 	 	 	 	 */
	WIZFI360_CMD_ID_RST,				/*!< 	: Restarts the module
												Usage: 	  AT+RST
												Response: OK
														  ready
										*/
	WIZFI360_CMD_ID_ECHO,				/*!< 	Configure AT Commands Echoing
												Usage:	ATE<enable>
														<enable>:
															• 0: Disable echoing
															• 1: Enable echoing (default)
	 	 	 	 	 	 	 	 	 	 */
} WIZFI360_CommandIdTypeDef;


/**
  * @brief  WIZFI360 handle structure definition
  */
typedef struct __WIZFI360_HandlerTypedef
{
	WIZFI360_WifiModeTypeDef WifiMode;					/*!< Indicates, weather the module is in Station-Mode, Soft-AP-Mode or
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 both (Station-Mode & Soft-AP-Mode). */
	WIZFI360_EchoModeTypeDef EchoMode;					/*!< Indicates, weather the echo mode is enabled. */

	uint8_t ExpectingEcho;								/*!< Indicates, weather an echo is expected in the receive buffer.
															 Echo is expected, after AT command is sent, while echo mode is enabled.
															 	 0: echo is not expected
															 	 1: echo is expected */
	uint8_t ExpectingResponse;							/*!< Indicates, weather a response is expected in the receive buffer.
															 Response is expected, after AT command is sent. AT commands may not
															 be sent, while response is expected.
															 	 0: response is not expected
															 	 1: response is expected */
	char CommandBuffer[WIZFI360_MAX_CMD_LEN];			/*!< String buffer holds the ongoing/last AT command to be transmitted via UART */

	uint8_t CommandLength;								/*!< The length of the AT command in CommandBuffer */

	WIZFI360_CommandIdTypeDef CommandId;				/*!< Identifies the ongoing/last AT command */

	uint8_t TagCharsReceived[WIZFI360_NUM_TAGS];		/*!< The amount of consecutive tag characters found in UartRxBuffer. */

	uint8_t EchoCharsReceived;							/*!< The amount of consecutive echo characters found in UartRxBuffer. */

	uint8_t SubTopicCharsReceived[
		WIZFI360_MAX_SUBTOPIC_CALLBACKS];				/*!< Amount of consecutive topic characters found in UartRxBuffer. */

	uint8_t WifiState;									/*!< Indicates, weather the module is connected to an AP or not. */

	ring_buffer_t UartRxBuffer;							/*!< Ring-buffer for received UART data. */

	uint8_t MessageIncoming;							/*!< Indicates, that a subscribe topic was found in UartRxBuffer.  */

	uint8_t MessageStartIndex;							/*!< The index, at which the message starts  */

	uint8_t NumSubTopicCallbacks;						/*!< Number of subscribe-topics, that we listen to */

	char SubTopics[WIZFI360_MAX_SUBTOPIC_CALLBACKS]		/*!< List of subscribe-topic strings, that we listen to. */
					  [WIZFI360_MAX_SUBTOPIC_SCAN_LEN];

	int receivingTopic;

	int cr_found;


	/* Callbacks ----------------------------------------------------------------------------------------------------------*/

	void (*SubTopicCallbacks[
		WIZFI360_MAX_SUBTOPIC_CALLBACKS]) (char*);		/*!< List of callback functions (mapped to SubTopics) */

	void (*CommandOkCallback)(void);

	void (*CommandErrorCallback)(void);

	void (*ReadyCallback)(void);

	void (*WifiConnectFailedCallback)(void);

	void (*WifiConnectedCallback)(void);

	void (*WifiDisconnectedCallback)(void);


} WIZFI360_HandlerTypedef;

/*********************************************************************************************/

/* Public function prototypes  ---------------------------------------------------------*/

void WIZFI360_Initialize();

void WIZFI360_Start();

void WIZFI360_Stop();

void WIZFI360_Process();

void WIZFI360_Reset();

WIZFI360_State WIZFI360_GetState();

WIZFI360_WifiState WIZFI360_GetWifiState();

WIZFI360_StatusTypeDef WIZFI360_AT_Test();

WIZFI360_StatusTypeDef WIZFI360_AT_Restart();

WIZFI360_StatusTypeDef WIZFI360_AT_SetEchoMode(WIZFI360_EchoModeTypeDef mode);

WIZFI360_StatusTypeDef WIZFI360_AT_ConnectToAccessPoint(const char* ssid, const char* password);

WIZFI360_StatusTypeDef WIZFI360_AT_SetWifiMode(WIZFI360_WifiModeTypeDef mode);

WIZFI360_StatusTypeDef WIZFI360_AT_SetDhcpMode(WIZFI360_WifiModeTypeDef mode, WIZFI360_DhcpModeTypeDef dhcp);

WIZFI360_StatusTypeDef WIZFI360_AT_GetSSLCertificate();

WIZFI360_StatusTypeDef WIZFI360_AT_SetSSLCertificate();

WIZFI360_StatusTypeDef WIZFI360_AT_ConfigureMqtt(const char* userName, const char*  pwd,
		const char* clientId, uint16_t aliveTime );

WIZFI360_StatusTypeDef WIZFI360_AT_MqttSetTopic(const char* pubTopic, const char*  subTopic);

WIZFI360_StatusTypeDef WIZFI360_AT_MqttConnectToBroker(WIZFI360_MqttAuthModeTypeDef authMode, const char*  brokerAddr,
		uint16_t brokerPort);

WIZFI360_StatusTypeDef WIZFI360_AT_MqttDisconnectFromBroker();

WIZFI360_StatusTypeDef WIZFI360_AT_MqttPublishMessage(const char* message);

void WIZFI360_AT_HandleResponse(WIZFI360_TagIdTypeDef tagId);

void WIZFI360_UART_BytesReceived(const char *data, ring_buffer_size_t size);

void WIZFI360_RegisterSubTopicCallback(const char* topic, void (*func)(char*));
void WIZFI360_RegisterCommandOkCallback(void (*func)(void));
void WIZFI360_RegisterCommandErrorCallback(void (*func)(void));
void WIZFI360_RegisterReadyCallback(void (*func)(void));
void WIZFI360_RegisterWifiConnectFailedCallback(void (*func)(void));
void WIZFI360_RegisterWifiConnectedCallback(void (*func)(void));
void WIZFI360_RegisterWifiDisconnectedCallback(void (*func)(void));

/*********************************************************************************************/

#endif /* INCLUDES_WIZFI360_H_ */
