/*
 * mqqt.c
 *
 *  Created on: 08.05.2021
 *      Author: serca
 */


#include "mqqt.h"
#include "wizfi360.h"
#include "mqtt_user.h"


__IO uint16_t lightSensorValue_12b;

static void MqqtStatemachineProcess();

void Mqqt_Initialize()
{
	WIZFI360_Initialize();

	/*##-3- Calibrate ADC then Start the conversion process ####################*/
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) !=  HAL_OK)
	{
		/* ADC Calibration Error */
		Error_Handler();
	}

	if (HAL_ADC_Start(&hadc1) != HAL_OK)
	{
		/* Start Conversation Error */
		__NOP();
	}

	/*##-4- Wait for the end of conversion #####################################*/
	/*  For simplicity reasons, this example is just waiting till the end of the
	  conversion, but application may perform other tasks while conversion
	  operation is ongoing. */
	if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
	{
		/* End Of Conversion flag not set on time */
		__NOP();
	}

	/* Check if the continous conversion of regular channel is finished */
	if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
	{
		/*##-5- Get the converted value of regular channel  ########################*/
		lightSensorValue_12b = HAL_ADC_GetValue(&hadc1);
	}
}


void Mqqt_Process()
{
	WIZFI360_Process();
	MqqtStatemachineProcess();
}


static void MqqtStatemachineProcess()
{
	static uint8_t currentState = 0;
	static uint32_t tick_ms = 0;

	// TODO Reset of wifi module
	// TODO disable echo
	// TODO yakindu state machine
	switch (currentState)
	{
		case 0:
		{
			WIZFI360_ConfigureMode(WIZFI360_MODE_STATION);
			currentState++;
			break;
		}
		case 1:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
			}
			break;
		}
		case 2:
		{
			WIZFI360_ConfigureDhcp(WIZFI360_MODE_STATION, WIZFI360_DHCP_ENABLE);
			currentState++;
			break;
		}
		case 3:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
			}
			break;
		}
		case 4:
		{
			WIZFI360_ConnectToAccessPoint(WIFI_SSID, WIFI_PASSWORD);
			currentState++;
			break;
		}
		case 5:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
			}
			break;
		}
		case 6:
		{
			WIZFI360_MqqtInit(MQQT_USERNAME, MQQT_PASSWORD,
					MQQT_CLIENT_ID, MQQT_ALIVE_TIME);

			currentState++;
			break;
		}
		case 7:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
			}
			break;
		}
		case 8:
		{
			// TODO defines für cases von no. of topics
			WIZFI360_MqqtSetTopic(MQTT_PUB_TOPIC, MQTT_SUBTOPIC_1, NULL, NULL);
			currentState++;
			break;
		}
		case 9:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
			}
			break;
		}
		case 10:
		{
			WIZFI360_MqqtConnectToBroker(WIZFI360_MQQT_AUTH_DISABLE,
					"broker.hivemq.com", 1883);
			currentState++;
			break;
		}
		case 11:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
			}
			break;
		}
		case 12:
		{

			if (HAL_ADC_Start(&hadc1) != HAL_OK)
			{
				/* Start Conversation Error */
				__NOP();
			}
			if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
			{
				/* End Of Conversion flag not set on time */
				__NOP();
			}

			/* Check if the continous conversion of regular channel is finished */
			if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
			{
				/*##-5- Get the converted value of regular channel  ########################*/
				lightSensorValue_12b = HAL_ADC_GetValue(&hadc1);
			}

			char message[32] = {0};
			sprintf(message, "%d", lightSensorValue_12b);

			// TODO try to send JSON format (find library)
			WIZFI360_MqqtPublishMessage(message);

			currentState++;
			break;
		}
		case 13:
		{
			if (WIZFI360_GetState() == WIZFI360_STATE_READY)
			{
				currentState++;
				tick_ms = HAL_GetTick();
			}
			break;
		}
		case 14:
		{
			if(HAL_GetTick() - tick_ms >= 1000)
			{
				currentState = 12;
			}
			break;
		}
	}
}
