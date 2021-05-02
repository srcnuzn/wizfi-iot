/*
 * test.c
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

#include <wizfi360_uart.h>
#include "wizfi360.h"
#include "string.h"

//Das ist ein OK Tag
const char* OK_TAG = "\r\nOK\r\n";
uint8_t echo_enabled = 1;
char command[32];

void EvaluatePayload(uint8_t* payload, uint8_t length);
void SendCommand(char* cmd);

/**
 *	TODO: Comment
 */
void WIZFI360_Initialize()
{
	WIZFI360_UART_Initialize();
	SendCommand("AT\r\n");
}

/**
 *	TODO: Comment
 */
void WIZFI360_Process()
{
	if(WIZFI360_UART_DataAvailable() > 0)
	{
		uint8_t ctr = 0;

		for(int i = 0; i < WIZFI360_UART_DataAvailable(); i++)
		{
			uint8_t tmp;

			WIZFI360_UART_PeekFromBuffer(&tmp, i);

			if (tmp == OK_TAG[ctr])
			{
				ctr++;

				if(ctr >= strlen(OK_TAG))
				{
					const uint8_t length = i-ctr+1; //Länge der Payload (Echo+Daten)

					uint8_t payload[length];
					uint8_t dummy[strlen(OK_TAG)];

					/*
					 * TODO: Extract Echo if enabled, and then payload, and then tag
					 */

					WIZFI360_UART_ReadFromBuffer(payload, length);

					EvaluatePayload(payload, length);

					WIZFI360_UART_ReadFromBuffer(dummy, strlen(OK_TAG));
				}
			}
			else if (tmp == OK_TAG[0])
			{
				ctr = 1;
			}
			else
			{
				ctr = 0;
			}
		}
	}
}

/**
 *	TODO: Comment
 */
void EvaluatePayload(uint8_t* payload, uint8_t length)
{
	if (echo_enabled)
	{
		int cmd_len = strlen(command);

		if(length != cmd_len)
		{
			// TODO: Error handling
			return;
		}

		for (int i = 0; i < strlen(command); i++)
		{
			if(command[i] != payload[i])
			{
				// TODO: Error handling
				return;

			}
		}
	}
}

/**
 *	TODO: Comment
 */
void SendCommand(char* cmd)
{
	uint8_t len = strlen(cmd);

	for(int i = 0; i < len; i++)
	{
		command[i] = cmd[i];
	}

	WIZFI360_UART_SendBlockingMode((uint8_t*)command, len, 10000);
}
