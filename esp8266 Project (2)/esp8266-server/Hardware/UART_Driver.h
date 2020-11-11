#ifndef HARDWARE_UART_DRIVER_H_
#define HARDWARE_UART_DRIVER_H_

#include <msp.h>
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define UART_BUFFER_SIZE 	  2048

void UART_Init(EUSCI_A_Type *UART);
void UART_Write(EUSCI_A_Type *UART, uint8_t *Data, uint32_t Size);
uint32_t UART_Read(uint8_t *Data, uint32_t Size);
uint32_t UART_Available(void);
void UART_Flush(void);

#endif /* HARDWARE_UART_DRIVER_H_ */
