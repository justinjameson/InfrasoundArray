
#ifndef DEVICES_ESP8266_H_
#define DEVICES_ESP8266_H_

#include <msp.h>
#include "main.h"


#include "Hardware/UART_Driver.h"
#include "Devices/MSPIO.h"

#define esp8266_uart	EUSCI_A2
#define delay_ms(x)		delay(x)	

#define ESP8266_BUFFER_SIZE     2048
#define ESP8266_RECEIVE_TRIES   10

#define TCP                     0
#define UDP                     1



bool ESP8266_CheckConnection(void);
bool ESP8266_SetMode(uint8_t mode);
bool ESP8266_AvailableAPs(void);
bool ESP8266_AssociatedIPs(void);
bool ESP8266_ConnectToAP(char *SSID, char *Password);
bool ESP8266_CheckAP(void);
bool ESP8266_ProduceAP(char *SSID, char *Password);
bool ESP8266_EstablishConnection(char ID, char *type, char *address, char *port);
bool ESP8266_EnableMultipleConnections(bool Enable);
bool ESP8266_StartServer(char *port);
bool ESP8266_SetStaticIP(char *IP);
bool ESP8266_SendData(char ID, char *Data, uint32_t DataSize);
bool ESP8266_ReceiveData(void);

void ESP8266_SendATCommand(char *ATcommand);
void ESP8266_Terminal(void);
char *ESP8266_GetBuffer(void);
void ESP8266_HardReset(void);


void GetDataFromBuffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto);

#endif /* DEVICES_ESP8266_H_ */
