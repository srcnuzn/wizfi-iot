/*
 * test.h
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

#ifndef INCLUDES_WIZFI360_H_
#define INCLUDES_WIZFI360_H_

/*********************************************************************************************/

/* Includes ---------------------------------------------------------*/

#include "../includes/wizfi360_def.h"
#include <stdint.h>

/*********************************************************************************************/

/* Defines ---------------------------------------------------------*/

#define WIZFI360_MAX_CMD_LEN 				128
#define WIZFI360_MAX_AP_PWD_LEN 			 64
#define WIZFI360_MAX_AP_SSID_LEN 			 32
#define WIZFI360_MAX_MQQT_USERNAME_LEN		 50
#define WIZFI360_MAX_MQQT_PWD_LEN			 50
#define WIZFI360_MAX_MQQT_CLIENTID_LEN		 50

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

/* Macro definitions ---------------------------------------------------------*/

/** @brief  Checks if the tag id is a response tag.
  * @note	Response tags are expected, when AT command is evaluated
  * 		by wizfi360 module
  * @note	WIZFI360_TAG_ID_FAIL can also be received as message,
  * 		when connection to AP is lost (--> not as AT response)
  * @param  __TAG_ID__ The tag id to check for.
  * @retval TRUE if response tag, else false
  */
#define __WIZFI360_IS_RESPONSE_TAG(__TAG_ID__)        			\
	(															\
		(__TAG_ID__ == WIZFI360_TAG_ID_ERROR)				||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_ALREADY_CONNECTED)	||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_SEND_FAIL)			||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_OK)					||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_SEND_OK)				||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_FAIL)					\
	)

/** @brief  Checks if the tag id is a message tag.
  * @note	Message tags can be received at any time.
  * @param  __TAG_ID__ The tag id to check for.
  * @retval TRUE if message tag, else false
  */
#define __WIZFI360_IS_MESSAGE_TAG(__TAG_ID__)        			\
	(															\
		(__TAG_ID__ == WIZFI360_TAG_ID_READY)				||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_WIFI_CONNECTED)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_WIFI_GOT_IP)			||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_WIFI_DISCONNECT)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_BUSY_SENDING)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_BUSY_PROCESSING)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_LINK_ID_CONNECT)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_LINK_ID_CLOSED)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_STA_CONNECTED)		||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_DIST_STA_IP)			||	\
		(__TAG_ID__ == WIZFI360_TAG_ID_STA_DISCONNECTED)		\
	)


/*********************************************************************************************/

/* Type definitions ---------------------------------------------------------*/

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
	WIZFI360_TAG_ID_STA_DISCONNECTED,		/*!< A Station disconnects to the WizFi360 SoftAP. */
} WIZFI360_TagIdTypeDef;


/**
  * @brief WIZFI360 mode definition
  */
typedef enum
{
	WIZFI360_MODE_STATION,				/*!< Station Mode */
	WIZFI360_MODE_SOFT_AP,				/*!<  SoftAP mode (factory default) */
	WIZFI360_MODE_BOTH,					/*!<  Station + SoftAP mode */
} WIZFI360_ModeTypeDef;


/**
  * @brief WIZFI360 mode definition
  */
typedef enum
{
	WIZFI360_STATE_READY,				/*!< The module is ready for AT command reception. */
	WIZFI360_STATE_BUSY,				/*!< The module is busy. An AT command is already ongoing. */
	WIZFI360_STATE_UNKNOWN,				/*!< The modules state is unknown. */
} WIZFI360_State;

typedef enum
{
	WIZFI360_RESPONSE_OK,
	WIZFI360_RESPONSE_ERROR
} WIZFI360_ResponseTypeDef;

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
  * @brief WIZFI360 MQQT authentication mode definition
  */
typedef enum
{
	WIZFI360_MQQT_AUTH_ENABLE,				/*!< Enables MQQT authentication (userName/password needed) */
	WIZFI360_MQTT_AUTH_DISABLE				/*!< Disable MQQT authentication (userName/password not needed) */
}WIZFI360_MqqtAuthModeTypeDef;


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
	WIZFI360_CMD_ID_MQTTPUB,			/*!< 	TODO COMMENT */
} WIZFI360_CommandIdTypeDef;


/**
  * @brief  WIZFI360 handle structure definition
  */
typedef struct __WIZFI360_HandlerTypedef
{
	WIZFI360_ModeTypeDef Mode;							/*!< Indicates, weather the module is in Station-Mode, Soft-AP-Mode or
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 both (Station-Mode & Soft-AP-Mode). */
	uint8_t EchoEnabled;								/*!< Indicates, weather the echo mode is enabled.
															 	 0: echo is not enabled
															 	 1: echo is enabled (default) */
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
	uint8_t TagCharsReceived[WIZFI360_NUM_TAGS];		/*!< The amount of consecutive tag characters found in UART receive buffer. */
	uint8_t EchoCharsReceived;							/*!< The amount of consecutive echo characters found in UART receive buffer. */
	uint8_t WifiState;									/*!< Indicates, weather the module is connected to an AP or not. */
} WIZFI360_HandlerTypedef;

/*********************************************************************************************/

/* Public function prototypes  ---------------------------------------------------------*/

void WIZFI360_Initialize();

void WIZFI360_Process();

WIZFI360_State WIZFI360_GetState();

WIZFI360_WifiState WIZFI360_GetWifiState();

void WIZFI360_ConnectToAccessPoint(const char* ssid, const char* password);

void WIZFI360_ConfigureMode(WIZFI360_ModeTypeDef mode);

void WIZFI360_ConfigureDhcp(WIZFI360_ModeTypeDef mode, WIZFI360_DhcpModeTypeDef dhcp);

void WIZFI360_MqttInit(const char* userName, const char*  pwd,
		const char* clientId, uint16_t aliveTime );

void WIZFI360_MqttSetTopic(const char* pubTopic, const char*  subTopic1,
		const char* subTopic2, const char* subTopic3);

void WIZFI360_MqttConnectToBroker(uint8_t enable, const char*  mqttBrokerIP,
		uint16_t mqttBrokerPort);

void WIZFI360_MqttPublishMessage(const char* message);

void WIZFI360_CommandCpltCallback(WIZFI360_CommandIdTypeDef command,
		WIZFI360_ResponseTypeDef response);

void WIZFI360_WifiConnectedCallback();

void WIZFI360_WifiConnectFailedCallback();

/*********************************************************************************************/

#endif /* INCLUDES_WIZFI360_H_ */
