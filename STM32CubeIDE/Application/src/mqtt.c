/*
 * mqtt.c
 *
 *  Created on: 08.05.2021
 *      Author: serca
 */


/*********************************************************************************************/

/* Includes ---------------------------------------------------------------------------------*/

// Used for NULL
#include <stddef.h>
#include <stdint.h>

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


/*********************************************************************************************/
/* Private Defines ------------------------------------------------------------------*/

// Maximum amount of timers used simultaneously in statemachine
#define MAX_TIMERS 		4
// Sample time of mqtt process in milliseconds
#define SAMPLE_TIME	  100

/*********************************************************************************************/

/* Private Variables ------------------------------------------------------------------*/

// The message to publish

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

volatile uint32_t uart_error_ctr = 0;

volatile uint32_t timeout_ctr = 0;

uint32_t configureMqttFailed_ctr = 0;
uint32_t connectToAccessPointFailed_ctr = 0;
uint32_t connectToBrokerFailed_ctr = 0;
uint32_t publishTopicFailed_ctr = 0;
uint32_t resetModuleFailed_ctr = 0;
uint32_t setStationModeFailed_ctr = 0;
uint32_t setTopicFailed_ctr = 0;
uint32_t enableDhcpFailed_ctr = 0;


void MqttClient_Publish()
{
	MqttClient_PublishInteger("time", MqttClient_GetTick());
	MqttClient_PublishInteger( "uart", uart_error_ctr);
	MqttClient_PublishInteger( "rst", resetModuleFailed_ctr);
	MqttClient_PublishInteger( "pub", publishTopicFailed_ctr);
	MqttClient_PublishInteger( "ap", connectToAccessPointFailed_ctr);
}

/*********************************************************************************************/

/* Private function prototypes -----------------------------------------------*/

static void mqttClientStatemachine_react_to_events();
static void mqttClientStatemachine_write_inputs();
static void mqttClientStatemachine_handle_reset_source();
static void mqttClientStatemachine_handle_command_request();

/*********************************************************************************************/

/* Public functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the MqttClient
  * @note	This function must be called once in application startup.
  * @retval none
  */
void MqttClient_Initialize()
{
	// Initialize timer services used for state machine timer events
 	sc_timer_service_init(&timer_service, timers, MAX_TIMERS,
	        (sc_raise_time_event_fp) &mqttClientStatemachine_raise_time_event);

 	// Initialize the state machine
	mqttClientStatemachine_init(&sm);

	// Set the sample time of the state machine
	mqttClientStatemachine_set_dT(&sm, SAMPLE_TIME);

	// Set the mqtt publish interval
	mqttClientStatemachine_set_publishInterval(&sm, MQTT_PUBLISH_INTERVAL);

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
	mqttClientStatemachine_handle_reset_source();

	// If the state machine requests a reset...
	if (mqttClientStatemachine_WizFi360_is_raised_resetModule(&sm))
	{
		// Reset the module
		WIZFI360_Reset();
	}
	// else, if there is no reset request...
	else
	{
		// Check for command requests and handle them.
		mqttClientStatemachine_handle_command_request();
	}
}

static void mqttClientStatemachine_handle_reset_source()
{
	if (mqttClientStatemachine_WizFi360_is_raised_configureMqttFailed(&sm))
	{
		configureMqttFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_connectToAccessPointFailed(&sm))
	{
		connectToAccessPointFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_connectToBrokerFailed(&sm))
	{
		connectToBrokerFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_enableDhcpFailed(&sm))
	{
		enableDhcpFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_publishTopicFailed(&sm))
	{
		publishTopicFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_resetModuleFailed(&sm))
	{
		resetModuleFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_setStationModeFailed(&sm))
	{
		setStationModeFailed_ctr++;
	}
	if (mqttClientStatemachine_WizFi360_is_raised_setTopicFailed(&sm))
	{
		setTopicFailed_ctr++;
	}
}

static void mqttClientStatemachine_handle_command_request()
{
	if (mqttClientStatemachine_WizFi360_is_raised_setStationMode(&sm))
		WIZFI360_AT_ConfigureMode(WIZFI360_MODE_STATION);

	else if (mqttClientStatemachine_WizFi360_is_raised_enableDhcp(&sm))
		WIZFI360_AT_ConfigureDhcp(WIZFI360_MODE_STATION, WIZFI360_DHCP_ENABLE);

	else if (mqttClientStatemachine_WizFi360_is_raised_connectToAccessPoint(&sm))
		WIZFI360_AT_ConnectToAccessPoint(WIFI_SSID, WIFI_PASSWORD);

	else if (mqttClientStatemachine_WizFi360_is_raised_configureMqtt(&sm))
		WIZFI360_AT_ConfigureMqtt(MQTT_USERNAME, MQTT_PASSWORD,
				MQTT_CLIENT_ID, MQTT_ALIVE_TIME);

	else if (mqttClientStatemachine_WizFi360_is_raised_setTopic(&sm))
		WIZFI360_AT_MqttSetTopic(MQTT_PUB_TOPIC, MQTT_SUBTOPIC_1, NULL, NULL);

	else if (mqttClientStatemachine_WizFi360_is_raised_connectToBroker(&sm))
		WIZFI360_AT_MqttConnectToBroker(WIZFI360_MQTT_AUTH_DISABLE,
		        MQTT_ADDRESS, MQTT_PORT);

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

/**
  * @brief	Executes when a response from wizfi360 module to an AT-command is received.
  * @retval None
  */
void WIZFI360_CommandCpltCallback(WIZFI360_CommandIdTypeDef command,
        WIZFI360_ResponseTypeDef response)
{
	// If we received OK...
	if (response == WIZFI360_RESPONSE_OK)
	{
		// Raise the statemachine "ok" event.
		mqttClientStatemachine_WizFi360_raise_ok(&sm);
	}
	// If we did not receive OK...
	else
	{
		// Raise the statemachine "error" event.
		mqttClientStatemachine_WizFi360_raise_error(&sm);
	}
}

/**
  * @brief	Executes when a wizfi360 module sends notification, that WiFi connection failed.
  * @retval None
  */
void WIZFI360_WifiConnectFailedCallback()
{
	// Raise the statemachine "fail" event.
	mqttClientStatemachine_WizFi360_raise_fail(&sm);
}

/**
  * @brief	Executes when a wizfi360 module sends notification, that module is ready and listens.
  * @retval None
  */
void WIZFI360_ModuleReadyCallback()
{
	// Raise the statemachine "fail" event.
	mqttClientStatemachine_WizFi360_raise_ready(&sm);
}



/*********************************************************************************************/
