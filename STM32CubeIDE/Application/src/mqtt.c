/*
 * mqtt.c
 *
 *  Created on: 08.05.2021
 *      Author: serca
 */


/*********************************************************************************************/

/* Includes ---------------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../includes/mqtt.h"
#include "../includes/mqtt_required.h"
#include "../includes/mqtt_user.h"

// WizFi360 module driver
#include "../includes/wizfi360.h"

// Yakindu Statechart
#include "../src-gen/MqttClientStatemachine.h"
#include "../src-gen/sc_timer_service.h"

// JSON Write library
#include "../thirdparty/jWrite.h"
#include "../thirdparty/jRead.h"


/*********************************************************************************************/
/* Private Defines ------------------------------------------------------------------*/

// Maximum amount of timers used simultaneously in statemachine
#define MAX_TIMERS 		4
// Sample time of mqtt process in milliseconds
#define SAMPLE_TIME	  100

#ifdef MQTT_SUBSCRIBE_MODE
#ifdef MQTT_PUBLISH_MODE
#error("Only select one mode! Cannot publish and subscribe simultaneously!");
#endif
#endif

#ifndef MQTT_SUBSCRIBE_MODE
#ifndef MQTT_PUBLISH_MODE
#error("One mode must be selected. Either define MQTT_PUBLISH_MODE or MQTT_SUBSCRIBE_MODE!");
#endif
#endif

#ifdef MQTT_PUBLISH_MODE
#ifndef MQTT_PUBTOPIC
	#error("MQTT_PUBTOPIC must be defined!");
#endif
#endif

#ifdef MQTT_SUBSCRIBE_MODE
#ifndef MQTT_SUBTOPIC_FILTER
	#error("MQTT_SUBTOPIC_FILTER must be defined!");
#endif
#endif

#ifdef MQTT_PUBLISH_MODE
#define MQTT_SUBTOPIC_FILTER 	"VrqHi6yYlQPTIgbHkbUT"
#endif

#ifdef MQTT_SUBSCRIBE_MODE
#define MQTT_PUBTOPIC 			"dhP6PdgaLcKY3vKcDZPM"
#define MQTT_PUBLISH_INTERVAL	0
#define MQTT_PUBLISHING_ENABLED false
#endif

/*********************************************************************************************/

/* Private Variables ------------------------------------------------------------------*/

// The message to publish

// TODO: Cleanup global variables in mqtt.c (e.g. create data structure / handler)

#define MAX_PUB_MSG_SIZE	128
static char message[MAX_PUB_MSG_SIZE] = { 0 };


// Some time variables (in ms) used for  mqtt process scheduling
static uint32_t current_time = 0;
static uint32_t last_time = 0;
static uint32_t elapsed_time = 0;

// The statemachine handler (from Yakindu)
static MqttClientStatemachine sm;

// Timer handlers used for statemachine events
static sc_timer_t timers[MAX_TIMERS];

// The timers are managed by a timer service.
static sc_timer_service_t timer_service;

/*********************************************************************************************/

/* Private function prototypes -----------------------------------------------*/

void JRead_BuildIdentifier(const char* description, char* ident_out, uint8_t layer);

static void mqttClientStatemachine_react_to_events();
static void mqttClientStatemachine_write_inputs();

static void ATCommandOkCallback();
static void ATCommandErrorCallback();
static void WifiConnectFailedCallback();
static void WlanModuleReadyCallback();

/*********************************************************************************************/

/* Public functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the state-machine-driven MqttClient.
  * @note	This function must be called once in application startup.
  * @retval none
  */
void MqttClient_Initialize()
{
	WIZFI360_Initialize();

	WIZFI360_RegisterCommandOkCallback(ATCommandOkCallback);
	WIZFI360_RegisterCommandErrorCallback(ATCommandErrorCallback);
	WIZFI360_RegisterReadyCallback(WlanModuleReadyCallback);
	WIZFI360_RegisterWifiConnectFailedCallback(WifiConnectFailedCallback);

	MqttClient_RegisterUserCallbacks();

	// Initialize timer services used for state machine timer events
 	sc_timer_service_init(&timer_service, timers, MAX_TIMERS,
	        (sc_raise_time_event_fp) &mqttClientStatemachine_raise_time_event);

 	// Initialize the state machine
	mqttClientStatemachine_init(&sm);

	// Set the sample time of the state machine
	mqttClientStatemachine_set_dT(&sm, SAMPLE_TIME);

	// Set the mqtt publish interval
	mqttClientStatemachine_set_publishInterval(&sm, MQTT_PUBLISH_INTERVAL);

#ifdef MQTT_PUBLISH_MODE
	mqttClientStatemachine_set_publishingEnabled(&sm, true);
#else
	mqttClientStatemachine_set_publishingEnabled(&sm, false);
#endif
	// Enter the state machine
	mqttClientStatemachine_enter(&sm);

	// Update time variable (used for state machine scheduling)
	last_time = MqttClient_GetTick();
}


/**
  * @brief  The mqtt client process.
  * @note   This function must be called continuously in the main application.
  * @retval none
  */
void MqttClient_Process()
{
	// Execute wizfi360 task
	WIZFI360_Process();

	// Get elapsed time
	current_time = MqttClient_GetTick();
	elapsed_time = current_time - last_time;
	
	// If task is ready...
	if (current_time - last_time >= SAMPLE_TIME)
	{
		// Update statemachine times.
		sc_timer_service_proceed(&timer_service, elapsed_time);

		// Write statemachine variables.
		mqttClientStatemachine_write_inputs();

		// Execute statemachine task (one cycle)
		mqttClientStatemachine_run_cycle(&sm);

		// React to events which were fired in statemachine cycle
		mqttClientStatemachine_react_to_events();

		// Update time variable
		last_time = current_time;
	}
}

/**
  * @brief  Writes an integer entry into the JSON publish buffer to be sent.
  * @note   This function has no effect when called outside of MqttClient_Publish().
  * @param	description The description of the JSON entry (must be a '\0' terminated string!)
  * @param	value 		The value of the JSON entry
  * @retval none
  */
void MqttClient_PublishInteger(const char* description, const int value)
{
	jwObj_int( (char*) description, value);
}


/**
  * @brief  Writes a string entry into the JSON publish buffer to be sent.
  * @note   This function has no effect when called outside of MqttClient_Publish().
  * @param	description The description of the JSON entry (must be a '\0' terminated string!)
  * @param	value 		The value of the JSON entry (must be a '\0' terminated string!)
  * @retval none
  */
void MqttClient_PublishString(const char* description, const char* value)
{
	jwObj_string( (char*) description, (char*) value);
}


/**
  * @brief  Writes an double entry into the JSON publish buffer to be sent.
  * @note   This function has no effect when called outside of MqttClient_Publish().
  * @param	description The description of the JSON entry (must be a '\0' terminated string!)
  * @param	value 		The value of the JSON entry
  * @retval none
  */
void MqttClient_PublishDouble(const char* description, const double value)
{
	jwObj_double( (char*) description, value);
}


/**
  * @brief  Writes an boolean entry into the JSON publish buffer to be sent.
  * @note   This function has no effect when called outside of MqttClient_Publish().
  * @param	description The description of the JSON entry (must be a '\0' terminated string!)
  * @param	value 		The value of the JSON entry
  * @retval none
  */
void MqttClient_PublishBoolean(const char* description, const int oneOrZero)
{
	jwObj_bool( (char*) description, oneOrZero);
}

/**
  * @brief  Extracts integer value from MQTT message
  * @note   Description is case-sensitive
  * @param	message The received message from the MQTT broker
  * @param	description The full description of the JSON entry 
  * 					this has to include every parent entry name
  * 					(must be a '\0' terminated string!)
  * @retval Integer value that is held in entry with name 'description'
  */
int MqttClient_ReadInteger(const char* message, const char* description)
{
	const int layer = 0; // Only layer depth 0 supported
	// Estimate identifier length
	const int identLen =
		(layer+1) * MQTT_JSON_FORMATTING_OVERHEAD + 
				// taking into account formatting starting at lowest layer
		strlen(description) +
				// length of the full entry path
		1;		// length of trailing '
	char identifier[identLen];
	
	// Build identifier according to description and layer depth	
	JRead_BuildIdentifier(description, identifier, layer);
	
	// Query parameter is NULL (normal query)
	int data = jRead_int( (char*)message, identifier, NULL );
	return data;
}


/**
  * @brief  Extracts string value from MQTT message
  * @note   Description is case-sensitive
  * @param	message The received message from the MQTT broker
  * @param	description The full description of the JSON entry 
  * 					this has to include every parent entry name
  * 					(must be a '\0' terminated string!)
  * @param	destLen The preallocated length of the result char array
  * @param	result The destination char array to the String value
  * 		that is held in entry with name 'description' into
  * @retval 1 if success, -1 if read error
  */
int MqttClient_ReadString(const char* message, const char* description, int destlen, char* pDest)
{
	const int layer = 0; // Only layer depth 0 supported
	// Estimate identifier length
	const int identLen =
		(layer+1) * MQTT_JSON_FORMATTING_OVERHEAD + 
				// taking into account formatting starting at lowest layer
		strlen(description) +
				// length of the full entry path
		1;		// length of trailing '
	char identifier[identLen];
	
	// Build identifier according to description and layer depth	
	JRead_BuildIdentifier(description, identifier, layer);

	// Read jRead result struct
	struct jReadElement jResult;
	jRead( (char*)message, identifier, &jResult);

	// Check for memory error (+1 for \0-Termination)
	if(jResult.bytelen + 1 > destlen)
	{
		// TODO: Error Handling (Memory Error)
		return -1;
	}

	for(int i = 0; i < jResult.bytelen; i++)
	{
		pDest[i] = ((char*)jResult.pValue)[i];
	}

	// Make sure to teminate the string with '\0'
	pDest[jResult.bytelen] = '\0';
	return 1;
}


/**
  * @brief  Extracts double value from MQTT message
  * @note   Description is case-sensitive
  * @param	message The received message from the MQTT broker
  * @param	description The full description of the JSON entry 
  * 					this has to include every parent entry name
  * 					(must be a '\0' terminated string!)
  * @retval Double value that is held in entry with name 'description'
  */
double MqttClient_ReadDouble(const char* message, const char* description)
{
	const int layer = 0; // Only layer depth 0 supported
	// Estimate identifier length
	const int identLen =
		(layer+1) * MQTT_JSON_FORMATTING_OVERHEAD + 
				// taking into account formatting starting at lowest layer
		strlen(description) +
				// length of the full entry path
		1;		// length of trailing '
	char identifier[identLen];
	
	// Build identifier according to description and layer depth	
	JRead_BuildIdentifier(description, identifier, layer);
	
	// Query parameter is NULL (normal query)
	double data = jRead_double( (char*)message, identifier, NULL );
	return data;
}


/**
  * @brief  Builds a jRead identifier with the required syntax
  * @note   This function needs to be handed the correct layer value of the JSON entry
  * @param	description The description of the JSON entry (must be a '\0' terminated string!)
  * @param	ident_out Composed identifier matching the description & layer
  * @param	layer The layer depth of the JSON entry, e.g. {0 {1 {2 {3 } } } }  
  */
void JRead_BuildIdentifier(const char* description, char* ident_out, uint8_t layer)
{
	// Make sure to start at position 0 of string
	ident_out[0] = '\0';
	char* prefix;
	char* suffix;

	// Compose control prefix & suffix according to layer depth
	switch(layer)
	{
		case 0:
			prefix = "{'";
			suffix = "'";
			break;
		case 1:
			// TODO: Parse JSON entries of multi layer nested JSONs
			break;
		case 2:
			break;
		default:
			break;
	}

	// Concatenate description with control prefix & suffix
	strcat(ident_out, prefix);
	strcat(ident_out, description);
	strcat(ident_out, suffix);
}


/**
  * @brief  Binds a function call to the receival of a topic message
  * @note   Function func is called with each message of topic
  * @param	topic Name of the topic to bind the function to
  * @param	func The function that is called
  * @retval none
  */
void MqttClient_RegisterSubscribeCallback(const char* topic, void (*func)(const char*))
{
	WIZFI360_RegisterSubTopicCallback(topic, func);
}

/*********************************************************************************************/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Writes the inputs of the statemachine.
  * @note   This function should be called before executing the statemachine cycle.
  * @retval none
  */
static void mqttClientStatemachine_write_inputs()
{
	// If wifi is connected...
	if (WIZFI360_GetWifiState() == WIZFI360_WIFI_CONNECTED)
	{
		// Write true to statemachine variable.
		mqttClientStatemachine_WizFi360_set_wifiConnected(&sm, bool_true);
	}
	// else if wifi is not connected...
	else
	{
		// Write false to statemachine variable.
		mqttClientStatemachine_WizFi360_set_wifiConnected(&sm, bool_false);
	}
}

/**
  * @brief  This function handles output events that were raised by the statemachine.
  * @note   This function should be called after executing the statemachine cycle.
  * @retval none
  */
static void mqttClientStatemachine_react_to_events()
{
	if (mqttClientStatemachine_System_is_raised_reset(&sm))
	{
		MqttClient_HandleSystemResetRequest();
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_resetModule(&sm))
	{
		WIZFI360_Reset();
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_start(&sm))
	{
		WIZFI360_Start();
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_stop(&sm))
	{
		WIZFI360_Stop();
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_testModule(&sm))
	{
		WIZFI360_AT_Test();
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_restartModule(&sm))
	{
		WIZFI360_AT_Restart();
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_setStationMode(&sm))
	{
		WIZFI360_AT_SetWifiMode(WIZFI360_MODE_STATION);
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_enableDhcp(&sm))
	{
		WIZFI360_AT_SetDhcpMode(WIZFI360_MODE_STATION, WIZFI360_DHCP_ENABLE);
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_connectToAccessPoint(&sm))
	{
		WIZFI360_AT_ConnectToAccessPoint(WIFI_SSID, WIFI_PASSWORD);
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_configureMqtt(&sm))
	{
		WIZFI360_AT_ConfigureMqtt(MQTT_USERNAME, MQTT_PASSWORD,
				MQTT_CLIENT_ID, MQTT_ALIVE_TIME);
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_setTopic(&sm))
	{
		WIZFI360_AT_MqttSetTopic(MQTT_PUBTOPIC, MQTT_SUBTOPIC_FILTER);
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_connectToBroker(&sm))
	{
		WIZFI360_AT_MqttConnectToBroker(WIZFI360_MQTT_AUTH_DISABLE,
		        MQTT_ADDRESS, MQTT_PORT);
	}
	else if (mqttClientStatemachine_WizFi360_is_raised_publishTopic(&sm))
	{
		jwOpen(message, MAX_PUB_MSG_SIZE, JW_OBJECT, JW_COMPACT);

		// Write the JSON entries (must be implemented by user!)
		MqttClient_Publish();

		jwEnd();

		WIZFI360_AT_MqttPublishMessage(message);
	}
}

/*********************************************************************************************/

/* Requried function implementations ----------------------------------------------------*/

/**
  * @brief  This function sets up timers for the time events that are required by the state machine.
  * @param	evid An unique identifier of the event.
  * @param	time_ms The time in milliseconds
  * @param 	periodic Indicates the the time event must be raised periodically until the timer is unset
  * @note   This function is required in order to compile the statemachine code.
  * @retval none
  */
void mqttClientStatemachine_set_timer(MqttClientStatemachine *handle,
        const sc_eventid evid, const sc_integer time_ms, const sc_boolean periodic)
{
	sc_timer_set(&timer_service, handle, evid, time_ms, periodic);
}

/**
  * @brief  This function unsets timers for the time events that are required by the state machine
  * @param	evid An unique identifier of the event.
  * @note   This function is required in order to compile the statemachine code.
  * @retval none
  */
void mqttClientStatemachine_unset_timer(MqttClientStatemachine *handle,
        const sc_eventid evid)
{
	sc_timer_unset(&timer_service, evid);
}

/*********************************************************************************************/

/* Callback functions ---------------------------------------------------------*/

static void ATCommandOkCallback()
{
	mqttClientStatemachine_WizFi360_raise_ok(&sm);
}

static void ATCommandErrorCallback()
{
	mqttClientStatemachine_WizFi360_raise_error(&sm);
}

/**
  * @brief	Executes when a wizfi360 module sends notification, that WiFi connection failed.
  * @retval None
  */
static void WifiConnectFailedCallback()
{
	// Raise the statemachine "fail" event.
	mqttClientStatemachine_WizFi360_raise_fail(&sm);
}

/**
  * @brief	Executes when a wizfi360 module sends notification, that module is ready and listens.
  * @retval None
  */
static void WlanModuleReadyCallback()
{
	// Raise the statemachine "fail" event.
	mqttClientStatemachine_WizFi360_raise_ready(&sm);
}

/*********************************************************************************************/
