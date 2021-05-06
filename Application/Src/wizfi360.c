/*
 * test.c
 *
 *  Created on: 29.04.2021
 *      Author: soenk
 */

#include <wizfi360_uart.h>
#include "wizfi360.h"
#include "string.h"

#define MAX_CMD_LEN 128
#define NUM_TAGS 11

uint8_t isBusy = 0;
uint8_t echo_enabled = 1;
char command[MAX_CMD_LEN];
uint8_t matchCounter[NUM_TAGS] =
	{ 0 };

const char *WIZFI360_TAGS[NUM_TAGS] =
{
	// List of AT command returns
	"\r\nERROR\r\n",
	"\r\nALREADY CONNECTED\r\n",
	"\r\nSEND FAIL\r\n",
	"\r\nOK\r\n",
	"\r\nSEND OK\r\n",
	// List of Message tags
	"\r\nready\r\n",
	"\r\nWIFI CONNECTED\r\n",
	"\r\nWIFI GOT IP\r\n",
	"\r\nWIFI DISCONNECTED\r\n",
	"\r\nbusy s...\r\n",
	"\r\nbusy p...\r\n"
};

void EvaluatePayload(uint8_t *payload, uint8_t length);
void SendCommand(char *cmd);
void ScanBufferForTags();

/**
 *	TODO: Comment
 */
void WIZFI360_Initialize()
{
	WIZFI360_UART_Initialize();
}

/**
 *	TODO: Comment
 */
void WIZFI360_Process()
{
	static uint8_t currentState = 0;
	switch(currentState)
	{
		case 0:
		{
			SendCommand("AT+CWMODE_CUR=1\r\n");
			currentState++;
			break;
		}
		case 1:
		{
			if(!isBusy)
			{
				currentState++;
			}
			break;
		}
		case 2:
		{
			SendCommand("AT+CWDHCP_CUR=1,1\r\n");
			currentState++;
			break;
		}
		case 3:
		{
			if(!isBusy)
			{
				currentState++;
			}
			break;
		}
		case 4:
		{
			SendCommand("AT+CWJAP_CUR=\"Barefoot_IoT\",\"0919273540346\"\r\n");
			currentState++;
			break;
		}
		case 5:
		{
			if(!isBusy)
			{
				currentState++;
			}
			break;
		}
	}

	// If no UART data from wizfi is available...
	if (!WIZFI360_UART_DataAvailable())
	{
		// Do nothing
		return;
	}

	ScanBufferForTags();

}

/**
 *	TODO: Comment
 */
void EvaluatePayload(uint8_t *payload, uint8_t length)
{
	__NOP();
}

/**
 *	TODO: Comment
 */
void SendCommand(char *cmd)
{
	// TODO: Error if busy

	uint8_t len = strlen(cmd) + 1;
	if (len > MAX_CMD_LEN)
	{
		//TODO Error Handling
	}
	for (int i = 0; i < len; i++)
	{
		command[i] = cmd[i];
	}

	WIZFI360_UART_SendBlockingMode((uint8_t*) command, len - 1, 10000);

	isBusy = 1;
}


void ScanBufferForTags()
{
	// For each available byte in UART receive buffer...
	for (int i = 0; i < WIZFI360_UART_DataAvailable(); i++)
	{
		// A storage for the current byte to be peeked
		uint8_t tmpChar;

		// Peek the i-th byte from the buffer
		WIZFI360_UART_PeekFromBuffer(&tmpChar, i);

		// For each tag that could be received from wizfi module...
		for (int tag = 0; tag < NUM_TAGS; tag++)
		{
			// If we found a consecutive matching character in the tag...
			if (tmpChar == WIZFI360_TAGS[tag][matchCounter[tag]])
			{
				// We found one more matching character
				matchCounter[tag]++;

				// If we found a full tag in UART receive buffer...
				if (matchCounter[tag] >= strlen(WIZFI360_TAGS[tag]))
				{
					if(tag<5)
					{
						// AT response received
						isBusy = 0;
					}
					else
					{
						// AT message received
					}

					// The length of the payload (available bytes - tag size)
					uint8_t lenPayload = i - matchCounter[tag] + 1;

					// If wizfy module sends echo...
					if (echo_enabled)
					{
						// TODO: CHECK AVAILABLE STACK SIZE
						// A buffer for the echo (+ null termination string)
						char echo[strlen(command) + 1];

						// Write the null termination character at last index
						echo[strlen(command)] = '\0';

						// Decrement lenPayload (echo is not part of payload)
						lenPayload -= strlen(command);

						// Read the echo from buffer
						WIZFI360_UART_ReadFromBuffer((uint8_t*) echo,
								strlen(command));

						// If the echo does not match the sent command...
						if (strcmp(echo, command) != 0)
						{
							//TODO ERROR echo ungleich command
						}
						// TODO: Remove (needed for debugging)
						else
						{
							__NOP(); //Kein Echofehler
						}
					}

					// If there is payload data...
					if (lenPayload > 0)
					{
						// TODO: CHECK AVAILABLE STACK SIZE
						// The payload buffer
						uint8_t payload[lenPayload];

						// Read the payload data
						WIZFI360_UART_ReadFromBuffer(payload, lenPayload);

						// Evaluate the payload data
						EvaluatePayload(payload, lenPayload);
					}

					// Remove OK_Tag from ring-buffer, dummy is not used
					// TODO: Improve this.... (ringbuffer clear)
					uint8_t dummy[strlen(WIZFI360_TAGS[tag])];
					WIZFI360_UART_ReadFromBuffer(dummy,
							strlen(WIZFI360_TAGS[tag]));

					// Reset the tag counter.
					matchCounter[tag] = 0;
				}
			}
			// else, if we did found a consecutive matching character but the first of the tag...
			else if (tmpChar == WIZFI360_TAGS[tag][0])
			{
				// Set the matchCounter to one.
				matchCounter[tag] = 1;
			}
			// else, if nothing related to the tag was found...
			else
			{
				// Reset the tag counter.
				matchCounter[tag] = 0;
			}
		}
	}
}
