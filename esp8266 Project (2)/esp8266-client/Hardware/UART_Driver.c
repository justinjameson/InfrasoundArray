
#include "UART_Driver.h"

/*UARTA2 Ring Buffer Global Variables*/
volatile uint8_t UART_DATA[UART_BUFFER_SIZE];
volatile uint32_t uart_tail;
volatile uint32_t uart_head;

#define UART_BUFFER_EMPTY         uart_tail == uart_head ? true : false
#define UART_BUFFER_FULL          (uart_head + 1) % UART_BUFFER_SIZE == uart_tail ? true : false


void UART_Init(EUSCI_A_Type *UART)
{
	// Configure UART

	/*CTLW0 Register,
	 * UCMODEx:00b (UART Mode)
	 * UC7BIT:0b (8bit)
	 * UCSPB:0b (one stop bit)
	 * UCMSB: 0b (LSB First)
	 * UCPEN: 0b (parity disabled)
	*/
	UART->CTLW0 &= ~EUSCI_A_CTLW0_SYNC;			//Turn on Asynchronous Mode
	UART->CTLW0  = EUSCI_A_CTLW0_SWRST;			// Put eUSCI in reset
	UART->CTLW0 |= EUSCI_A_CTLW0_UCSSEL_2;  	// Configure eUSCI clock source: SMCLK

    // Baud Rate calculation
    // 3000000/(16*115200) = 1.6276
    // Fractional portion = 0.6276
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (1.6276-1)*16) = int(10.04) = 10

	UART->BRW = 1;                     			// 3M/(16*115200)
	UART->MCTLW =  (10 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;

	UART->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; 		// Initialize eUSCI

	UART->IFG &= ~EUSCI_A_IFG_RXIFG;    		// Clear eUSCI RX interrupt flag
	UART->IE |= EUSCI_A_IE_RXIE;        		// Enable USCI_A3 RX interrupt

	 // Enable eUSCIA0 interrupt in NVIC module

	if(UART == EUSCI_A0)
		NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
	else if(UART == EUSCI_A2)
		NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
}

void UART_Write(EUSCI_A_Type *UART, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    for(i = 0; i < Size; i++)
    {
    	while(!(UART->IFG & EUSCI_A_IFG_TXIFG)); //busywait
    	UART->TXBUF = Data[i];
    }
}

uint32_t UART_Read( uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    int8_t c;
		
		for(i = 0; i < Size; i++)
		{
			if(UART_BUFFER_EMPTY)
			{
				return i;
			}
			else
			{
				c = UART_DATA[uart_tail];
				uart_tail = (uart_tail + 1) % UART_BUFFER_SIZE;

				Data[i] = c;
			}
		}
	
    return i;
}

uint32_t UART_Available(void)
{
	return (UART_BUFFER_SIZE + uart_head - uart_tail) % UART_BUFFER_SIZE;
}

void UART_Flush(void)
{	
	memset((char *)UART_DATA, 0, strlen((char *)UART_DATA));
	uart_head = uart_tail = 0;
	return;
}

void EUSCIA0_IRQHandler(void)
{
		EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; //clear the flag
}


void EUSCIA2_IRQHandler(void)
{
    uint8_t c;
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
				EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; //clear the flag

				c = EUSCI_A2->RXBUF;

        if(UART_BUFFER_FULL)
         {
             /*TODO: Buffer Overflow, add handler here*/
         }
         else
         {
             UART_DATA[uart_head] = c;
             uart_head = (uart_head + 1) % UART_BUFFER_SIZE;
         }
    }
}
