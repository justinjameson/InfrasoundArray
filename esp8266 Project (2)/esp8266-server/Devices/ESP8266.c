
#include "ESP8266.h"

#define PIN       			BIT1        
#define GPIO_Port      	P6	

#define AT              "AT\r\n"
#define AT_RST          "AT+RST\r\n"
#define AT_GMR          "AT+GMR\r\n"
#define AT_GSLP         "AT+GSLP\r\n"
#define ATE             "ATE\r\n"
#define AT_CWMODE       "AT+CWMODE"
#define AT_CWJAP        "AT+CWJAP"
#define AT_CWSAP        "AT+CWSAP"
#define AT_CWLAP        "AT+CWLAP\r\n"
#define AT_CWQAP        "AT+CWQAP\r\n"
#define AT_CWLIF        "AT+CWLIF\r\n"
#define AT_CWDHCP       "AT+CWDHCP\r\n"
#define AT_CIPSTAMAC    "AT+CIPSTAMAC\r\n"
#define AT_CIPAPMAC     "AT+CIPAPMAC\r\n"
#define AT_CIPSTA       "AT+CIPSTA"
#define AT_CIPAP        "AT+CIPAP"
#define AT_CIPSTATUS    "AT+CIPSTATUS\r\n"
#define AT_CIPSTART     "AT+CIPSTART"
#define AT_CIPSEND      "AT+CIPSEND"
#define AT_CIPCLOSE     "AT+CIPSEND\r\n"
#define AT_CIFSR        "AT+CIFSR\r\n"
#define AT_CIPMUX       "AT+CIPMUX"
#define AT_CIPSERVER    "AT+CIPSERVER"
#define AT_CIPMODE      "AT+CIPMODE\r\n"
#define AT_CIPSTO       "AT+CIPSTO\r\n"
#define AT_CIUPDATE     "AT+CIUPDATE\r\n"
#define IPD             "+IPD"
#define CONNECT					"CONNECT"


char ESP8266_Buffer[ESP8266_BUFFER_SIZE];


bool ESP8266_WaitForAnswer(uint32_t Tries)
{
    uint32_t c;
    uint32_t i = 0;

    while(Tries)
    {
        if(UART_Available()){
            while(UART_Available())
						{
                UART_Read((uint8_t*)&c, 1);

                if(i > ESP8266_BUFFER_SIZE)
                {
                    return false;
                }
                else
                {
                    ESP8266_Buffer[i++] = c;
                }
            }

            ESP8266_Buffer[i++] = 0;
            return true;
        }
        Tries--;
        delay_ms(1);
    }

    return false;
}


bool ESP8266_CheckConnection(void)
{
    MSPrintf(esp8266_uart, AT);
    delay_ms(500);
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_SetMode(uint8_t mode)
{
    if(mode >3)
    {
        return false;
    }
		
    MSPrintf(esp8266_uart, "%s=%i\r\n", AT_CWMODE, mode);
    delay_ms(500);
	
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_AvailableAPs(void)
{
    MSPrintf(esp8266_uart, AT_CWLAP);
    delay_ms(4000);
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_AssociatedIPs(void)
{
    MSPrintf(esp8266_uart, AT_CIFSR);
    delay_ms(4000);
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_ConnectToAP(char *SSID, char *Password)
{
    MSPrintf(esp8266_uart, "%s=\"%s\",\"%s\"\r\n", AT_CWJAP, SSID, Password);
		
	uint32_t c;
    uint32_t i = 0;
	
	delay_ms(4000);

	CONNECTION:	
	  if(UART_Available())
		{
            while(UART_Available())
            {
                UART_Read((uint8_t*)&c, 1);
                ESP8266_Buffer[i++] = c;

            }
		}
		ESP8266_Buffer[i++] = 0;


    if(strstr(ESP8266_Buffer, "WIFI CONNECTED") == NULL)
    {
        goto CONNECTION;
    }

    return true;
}

bool ESP8266_ProduceAP(char *SSID, char *Password)
{
    MSPrintf(esp8266_uart, "%s=\"%s\",\"%s\",1,3\r\n", AT_CWSAP, SSID, Password);

    delay_ms(500);
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_EnableMultipleConnections(bool Enable)
{
    char c;

    switch(Enable)
    {
    case 0:
        c = '0';
        break;
    case 1:
        c = '1';
        break;
    }

    MSPrintf(esp8266_uart, "%s=%c\r\n", AT_CIPMUX, c);

    delay_ms(500);
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}


bool ESP8266_StartServer(char *port)
{		
    MSPrintf(esp8266_uart, "%s=1,%s\r\n", AT_CIPSERVER, port);
    delay_ms(500);
	
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_SetStaticIP(char *IP)
{		
    MSPrintf(esp8266_uart, "%s=\"%s\"\r\n", AT_CIPSTA, IP);
    delay_ms(500);
	
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

bool ESP8266_EstablishConnection(char ID, char *type, char *address, char *port)
{

    MSPrintf(esp8266_uart, "%s=%c,\"%s\",\"%s\",%s\r\n", AT_CIPSTART, ID, type, address, port);
    //MSPrintf(esp8266_uart, "%s=\"%s\",\"%s\",%s\r\n", AT_CIPSTART, type, address, port);

    delay_ms(1000);

    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "ALREADY CONNECTED"))
    {
        return true;
    }

    else if(strstr(ESP8266_Buffer, "OK"))
    {
        return true;
    }


    return false;
}

bool ESP8266_SendData(char ID, char *Data, uint32_t DataSize)
{

    MSPrintf(esp8266_uart, "%s=%c,%i\r\n", AT_CIPSEND, ID, DataSize);
    //MSPrintf(esp8266_uart, "%s=%i\r\n", AT_CIPSEND, DataSize);

    delay_ms(500);

    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, ">") == NULL)
    {
        return false;
    }

    MSPrintf(esp8266_uart, Data);

    delay_ms(500);

    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "SEND OK") == NULL)
    {
        return false;
    }

    return true;
}

void ESP8266_Terminal(void)
{
    while(1)
    {
        MSPgets(EUSCI_A0, ESP8266_Buffer, 128);
        MSPrintf(esp8266_uart, ESP8266_Buffer);

       delay_ms(2000);
        if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
        {
            MSPrintf(EUSCI_A0, "ESP8266 receive timeout error\r\n");
        }
        else
        {
            MSPrintf(EUSCI_A0, ESP8266_Buffer);
        }

    }
}

void GetDataFromBuffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto)
{
	int startStringLength = strlen (startString);
	int endStringLength   = strlen (endString);
	int so_far = 0;
	int indx = 0;
	int startposition = 0;
	int endposition = 0;

repeat1:
	while (startString[so_far] != buffertocopyfrom[indx]) indx++;
	if (startString[so_far] == buffertocopyfrom[indx])
	{
		while (startString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}

	if (so_far == startStringLength) startposition = indx;
	else
	{
		so_far =0;
		goto repeat1;
	}

	so_far = 0;

repeat2:
	while (endString[so_far] != buffertocopyfrom[indx]) indx++;
	if (endString[so_far] == buffertocopyfrom[indx])
	{
		while (endString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}

	if (so_far == endStringLength) endposition = indx-endStringLength;
	else
	{
		so_far =0;
		goto repeat2;
	}

	so_far = 0;
	indx=0;

	int i;
	for (i=startposition; i<endposition; i++)
	{
		buffertocopyinto[indx] = buffertocopyfrom[i];
		indx++;
	}
}

bool ESP8266_ReceiveData(void)
{
    uint32_t c;
    uint32_t i = 0;

again:
    if(UART_Available())
    {
        while(UART_Available())
        {
            UART_Read((uint8_t*)&c, 1);

            if(i > ESP8266_BUFFER_SIZE)
            {
                return false;
            }
            else
            {
                ESP8266_Buffer[i++] = c;
            }
            delay_ms(1);
        }

        if(strstr(ESP8266_Buffer, "+IPD") == NULL)
        {
            goto again;
        }

        if(strstr(ESP8266_Buffer, "E")){
            return true;
        }
        else
            goto again;
    }

    return false;
}

bool ESP8266_CheckAP(void)
{
    MSPrintf(esp8266_uart, "AT+CWJAP?\r\n");
    delay_ms(4000);
    if(!ESP8266_WaitForAnswer(ESP8266_RECEIVE_TRIES))
    {
        return false;
    }

    if(strstr(ESP8266_Buffer, "OK") == NULL)
    {
        return false;
    }

    return true;
}

char *ESP8266_GetBuffer(void)
{
    return ESP8266_Buffer;
}

void ESP8266_HardReset(void)
{
    GPIO_Port->OUT &= ~(PIN);

    delay_ms(1000);

    GPIO_Port->OUT |= PIN;
}
