
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

#define RED     BIT0
#define GREEN   BIT1
#define BLUE    BIT2
#define WHITE   (RED|GREEN|BLUE)

/**** FUNCTIONS PROTOTYPES ********/

void set3MhzClock(void);        //set DC0 to 3mhz

void Systick_Setup(void);

void Initialize_Esp8266(void);

void delay(uint16_t cycles);

void ADC_SETUP(void);
