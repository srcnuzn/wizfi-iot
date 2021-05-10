/*
 * test.h
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

#ifndef INC_WIZFI360_H_
#define INC_WIZFI360_H_

#include "main.h"
#include "wizfi360_def.h"


#define WIZFI360_MAX_CMD_LEN 			128
#define WIZFI360_MAX_PWD_LEN 			 64
#define WIZFI360_MAX_SSID_LEN 			 32

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
	WIZFI360_TAG_ID_FAIL,					/*!< TODO: COMMENT */
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
	WIZFI360_STATE_READY,				/*!< TODO: Comment */
	WIZFI360_STATE_BUSY,				/*!< TODO: Comment */
	WIZFI360_STATE_UNKNOWN,				/*!< TODO: Comment */
} WIZFI360_State;

/**
  * @brief WIZFI360 DHCP mode definition
  */
typedef enum
{
	WIZFI360_DHCP_ENABLE,				/*!< Enable DHCP */
	WIZFI360_DHCP_DISABLE				/*!< Disable DHCP */
}WIZFI360_DhcpModeTypeDef;

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
} WIZFI360_CommandIdTypeDef;


/**
  * @brief  WIZFI360 handle Structure definition
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
	char CommandBuffer[WIZFI360_MAX_CMD_LEN];			/*!< TODO: COMMENT */
	uint8_t CommandLength;								/*!< TODO: COMMENT */
	WIZFI360_CommandIdTypeDef CommandId;				/*!< TODO: COMMENT */
	uint8_t TagCharsReceived[WIZFI360_NUM_TAGS];		/*!< TODO: COMMENT */
	uint8_t EchoCharsReceived;							/*!< TODO: COMMENT */
} WIZFI360_HandlerTypedef;



void WIZFI360_Initialize();
void WIZFI360_Process();
WIZFI360_State WIZFI360_GetState();
void WIZFI360_ConnectToAccessPoint(const char* ssid, const char* password);
void WIZFI360_ConfigureMode(WIZFI360_ModeTypeDef mode);
void WIZFI360_ConfigureDhcp(WIZFI360_ModeTypeDef mode, WIZFI360_DhcpModeTypeDef dhcp);


#endif /* INC_WIZFI360_H_ */
