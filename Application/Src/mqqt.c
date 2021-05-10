/*
 * mqqt.c
 *
 *  Created on: 08.05.2021
 *      Author: serca
 */


#include "mqqt.h"
#include "wizfi360.h"

#define WIFI_SSID			"Blub"
#define WIFI_PASSWORD		"542684521489358"
#define MQQT_USERNAME		"irge"
#define MQQT_PASSWORD		"wasA"
#define MQQT_CLIENT_ID		"1234"
#define MQQT_ALIVE_TIME		31

static void MqqtStatemachineProcess();

void Mqqt_Initialize()
{
	WIZFI360_Initialize();
}


void Mqqt_Process()
{
	WIZFI360_Process();
	MqqtStatemachineProcess();
}


static void MqqtStatemachineProcess()
{
	static uint8_t currentState = 0;

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
			WIZFI360_MqqtSetTopic("blabla", "hahaha");
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
			WIZFI360_MqqtConnectToBroker(0, "35.156.215.0", 1883);
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
	}
}
