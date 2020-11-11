#ifndef MSPIO_H_
#define MSPIO_H_


#include <msp.h>
#include <stdarg.h>
#include "Hardware/UART_Driver.h"



void MSPrintf(EUSCI_A_Type *UART, const char *fs, ...);
int MSPgets(EUSCI_A_Type *UART, char *b, int size);

#endif /* MSPIO_H_ */
