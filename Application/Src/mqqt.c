/*
 * mqqt.c
 *
 *  Created on: 08.05.2021
 *      Author: serca
 */

#include "mqqt.h"
#include "wizfi360.h"
#include "mqtt_user.h"
#include "MqttClientStatemachine.h"
#include "sc_timer_service.h"
#include "stdio.h"

#define MAX_TIMERS 4

#define SAMPLE_TIME		 100
#define PUBLISH_TIME	1000

#define BROKER_ADDRESS	"broker.hivemq.com"
#define BROKER_PORT		1883

//! We allocate the desired array of timers.
static sc_timer_t timers[MAX_TIMERS];

char message[32] = { 0 };

//! The timers are managed by a timer service. */
static sc_timer_service_t timer_service;

static uint32_t current_time = 0;
static uint32_t last_time = 0;
static uint32_t elapsed_time = 0;

static uint16_t lightSensorValue_12b;
static void ReadSensor();

/*
 * The statemachine handler
 */
static MqttClientStatemachine sm;

static void mqttClientStatemachine_react_to_events();
static void mqttClientStatemachine_write_inputs();

void Mqqt_Initialize()
{
	WIZFI360_Initialize();

	/*##-3- Calibrate ADC then Start the conversion process ####################*/
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		/* ADC Calibration Error */
	}

	/*! Initializes the timer service */
	sc_timer_service_init(&timer_service, timers, MAX_TIMERS,
	        (sc_raise_time_event_fp) &mqttClientStatemachine_raise_time_event);

	mqttClientStatemachine_init(&sm);

	mqttClientStatemachine_set_dT(&sm, SAMPLE_TIME);

	mqttClientStatemachine_set_publishInterval(&sm, PUBLISH_TIME);

	mqttClientStatemachine_enter(&sm);

	last_time = HAL_GetTick();
}

void Mqqt_Process()
{
	current_time = HAL_GetTick();
	elapsed_time = current_time - last_time;

	if (elapsed_time >= SAMPLE_TIME)
	{
		ReadSensor();

		WIZFI360_Process();

		/*! push the timer service clock forward */
		sc_timer_service_proceed(&timer_service, elapsed_time);

		mqttClientStatemachine_write_inputs();

		mqttClientStatemachine_run_cycle(&sm);

		mqttClientStatemachine_react_to_events();

		last_time = current_time;
	}
}

static void mqttClientStatemachine_write_inputs()
{
	if (WIZFI360_GetWifiState() == WIZFI360_WIFI_CONNECTED)
	{
		mqttClientStatemachine_WizFi360_set_wifiConnected(&sm, bool_true);
	}
	else
	{
		mqttClientStatemachine_WizFi360_set_wifiConnected(&sm, bool_false);
	}
}

static void mqttClientStatemachine_react_to_events()
{
	if (mqttClientStatemachine_WizFi360_is_raised_setStationMode(&sm))
		WIZFI360_ConfigureMode(WIZFI360_MODE_STATION);

	if (mqttClientStatemachine_WizFi360_is_raised_configureDhcp(&sm))
		WIZFI360_ConfigureDhcp(WIZFI360_MODE_STATION, WIZFI360_DHCP_ENABLE);

	if (mqttClientStatemachine_WizFi360_is_raised_connectToAccessPoint(&sm))
		WIZFI360_ConnectToAccessPoint(WIFI_SSID, WIFI_PASSWORD);

	if (mqttClientStatemachine_WizFi360_is_raised_configureMqtt(&sm))
		WIZFI360_MqqtInit(MQQT_USERNAME, MQQT_PASSWORD,
				MQQT_CLIENT_ID, MQQT_ALIVE_TIME);

	if (mqttClientStatemachine_WizFi360_is_raised_setTopic(&sm))
		WIZFI360_MqqtSetTopic(MQTT_PUB_TOPIC, MQTT_SUBTOPIC_1, NULL, NULL);

	if (mqttClientStatemachine_WizFi360_is_raised_connectToBroker(&sm))
		WIZFI360_MqqtConnectToBroker(WIZFI360_MQQT_AUTH_DISABLE,
		        BROKER_ADDRESS, BROKER_PORT);

	if (mqttClientStatemachine_WizFi360_is_raised_publishTopic(&sm))
		WIZFI360_MqqtPublishMessage(message);

	if (mqttClientStatemachine_WizFi360_is_raised_disconnectFromBroker(&sm))
		//TODO: Call Disconnect Command
		#warning("no disconnect command available");
		;
}

void mqttClientStatemachine_set_timer(MqttClientStatemachine *handle,
        const sc_eventid evid, const sc_integer time_ms,
        const sc_boolean periodic)
{
	sc_timer_set(&timer_service, handle, evid, time_ms, periodic);
}

void mqttClientStatemachine_unset_timer(MqttClientStatemachine *handle,
        const sc_eventid evid)
{
	sc_timer_unset(&timer_service, evid);
}

void WIZFI360_CommandCpltCallback(WIZFI360_CommandIdTypeDef command,
        WIZFI360_ResponseTypeDef response)
{
	if (response == WIZFI360_RESPONSE_OK)
	{
		mqttClientStatemachine_WizFi360_raise_ok(&sm);
	}
	else
	{
		mqttClientStatemachine_WizFi360_raise_error(&sm);
	}
}

void WIZFI360_WifiConnectFailedCallback()
{
	mqttClientStatemachine_WizFi360_raise_fail(&sm);
}

static void ReadSensor()
{
	HAL_ADC_Start(&hadc1);

	HAL_ADC_PollForConversion(&hadc1, 100);

	/* Check if the continous conversion of regular channel is finished */
	if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC)
	        == HAL_ADC_STATE_REG_EOC)
	{
		/*##-5- Get the converted value of regular channel  ########################*/
		lightSensorValue_12b = HAL_ADC_GetValue(&hadc1);
	}

	// TODO try to send JSON format (find library)
	sprintf(message, "%d", lightSensorValue_12b);
}

