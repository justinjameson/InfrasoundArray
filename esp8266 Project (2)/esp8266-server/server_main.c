
#include "msp.h"
#include "main.h"
#include "Devices/MSPIO.h"
#include "Devices/ESP8266.h"
#include "Hardware/UART_Driver.h"

#define   CPU_Freq          3000000
#define   pc_uart           EUSCI_A0

//Pointer to ESP8266 global buffer
enum{CREATE_AP, JOIN_AP, YES, NO, CLIENT, SERVER};

uint8_t esp_config = JOIN_AP;
uint8_t List_AP = NO;
uint8_t Set_Station_IP = NO;
uint8_t connection_mode = SERVER;
uint8_t check_AP = NO;

char *esp866_data;
bool DEBUG = true;

// Station (STA) details
const char router_ssid[] = "EE-Route";
const char router_pwd[]  = "bcf73726";
const char station_IP[] = "192.168.10.12";


// Access-Poinr(softAP) details
const char ssid[] = "Esp-Master";
const char pwd[] =  "abcd1234";

// Define Variables
const char port[] = "80";
const uint8_t wifi_mode = 1;       //wife_mode= 1:Station, 2:softAP, 3:Both

//Sensor Variables
volatile uint32_t Systick_Cnt = 0;
char name[5], value_buff[6];
uint16_t value1, value2, value3;



int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW|WDT_A_CTL_HOLD;

    /*RGB Leds*/
    P2->DIR = BIT2 | BIT1 | BIT0;
    P2->OUT &= ~(BIT2 | BIT1 | BIT0);

    //configure UART port pins
    P1->SEL0 |= BIT2 | BIT3;                // for UARTA0
    P1->SEL1 &= ~(BIT2+BIT3);

    P3->SEL0 |= BIT2 | BIT3;                //for UARTA2
    P3->SEL1 &= ~(BIT2 | BIT3);


    //----  Functions Calls  ------//

    set3MhzClock();             //set clock at 3Mhz
    Systick_Setup();
    UART_Init(pc_uart);
    UART_Init(EUSCI_A2);

    //----  Starting Sequence  ------//
    P2->OUT = BLUE;     delay(250);     P2->OUT &= ~WHITE;  delay(250);
    P2->OUT = GREEN;    delay(250);     P2->OUT &= ~WHITE;  delay(250);
    P2->OUT = WHITE;    delay(250);     P2->OUT &= ~WHITE;  delay(250);

    // Enable global interrupt
    __enable_irq();

    //Print Hello Message
    if(DEBUG) MSPrintf(pc_uart, "******** WELCOME! **********   \r\n");

    Initialize_Esp8266();
    memset(esp866_data, 0, strlen(esp866_data));


    //------------------------- Task- Watchdog timer=16-sec intervel ------------------------//

     WDT_A->CTL = WDT_A_CTL_PW|              // Password
                  WDT_A_CTL_CNTCL|
                  WDT_A_CTL_SSEL__ACLK |     // clock- source = 32.728kHz
                  WDT_A_CTL_IS_3;            // 16-SEC Interval

    while(1)
    {
        WDT_A->CTL = WDT_A_CTL_PW + 0x002B;     // Reset WDT_A Counter

        if(ESP8266_ReceiveData()){

            GetDataFromBuffer ("{", "}", esp866_data, name);
            GetDataFromBuffer ("[", "]", esp866_data, value_buff);
            value1 = atoi(value_buff);

            if(DEBUG) MSPrintf(pc_uart, "%s\r\n", esp866_data);
            if(DEBUG) MSPrintf(pc_uart, "From:%s value:%i\r\n", name,value1);
            memset(esp866_data, 0, strlen(esp866_data));    //clear buffer

        }

        P2->OUT ^= BIT1;
        delay(1);
    }

}

void Initialize_Esp8266(void)
{
    esp866_data = ESP8266_GetBuffer();

    //--------------------------- Task-0 ------------------------------------//

    ESP8266_HardReset();            //Hard Reset ESP8266
    delay(3000);                    //3 seconds delay_sec
    UART_Flush();                   //flush reset data


ESP8266_RESPONSE_ERROR:

    //--------------------------- Task-1 ------------------------------------//

    if(!ESP8266_CheckConnection())                      //Cmd: AT
    {
        delay(3000);        //3 seconds delay

        if(DEBUG) MSPrintf(pc_uart, "Error connection ESP8266...\r\n");
        goto ESP8266_RESPONSE_ERROR;
    }

    if(DEBUG) MSPrintf(pc_uart, "Connected to ESP8266..!\r\n");
    delay(500);


    //--------------------------- Task-2 ----------------------------------//

    if(!ESP8266_SetMode(wifi_mode))     //Cmd: AT+CWMODE=x
    {
        delay(3000);

        if(DEBUG) MSPrintf(pc_uart, "Failed wifi-config Mode:%i\r\n", wifi_mode);
        goto ESP8266_RESPONSE_ERROR;
    }

    if(DEBUG) MSPrintf(pc_uart, "Wifi-Config mode:%i is Set..!\r\n",wifi_mode);
    delay(500);


WIFI_CONFIG_ERROR:

    //--------------------------- Task-3 ----------------------------------//

    if(esp_config == CREATE_AP){

        if(!ESP8266_ProduceAP(ssid, pwd))      //Cmd: AT+CWSAP="ssid","password"
        {
            delay(3000);

            if(DEBUG) MSPrintf(pc_uart, "Failed to set ESP8266 as softAP!\r\n");
            goto WIFI_CONFIG_ERROR;
        }

        if(DEBUG) MSPrintf(pc_uart,"softAp: ssid=\"%s\", pwd=\"%s\" is Set..!\r\n", ssid, pwd);
        delay(500);
    }

    else if(esp_config == JOIN_AP){

        if(!ESP8266_ConnectToAP(router_ssid, router_pwd))    //Cmd: AT+CWSAP="ssid","password"
        {
            delay(3000);

            if(DEBUG) MSPrintf(pc_uart, "Failed to Connect to Router!\r\n");
            goto WIFI_CONFIG_ERROR;
        }
        if(DEBUG) MSPrintf(pc_uart, "Connected to Router..! \r\n");
        delay(500);
        UART_Flush();
    }

UPDATE_STATION_IP_ERROR:

    //--------------------------- Task-4 ----------------------------------//

    if(Set_Station_IP == YES){

         if(!ESP8266_SetStaticIP(station_IP))   //Cmd: AT+CIPSTA="192.168.10.12"
         {
             delay(3000);
             if(DEBUG) MSPrintf(pc_uart, "Failed to set Station IP = %s\r\n", station_IP);
             goto UPDATE_STATION_IP_ERROR;
         }

         if(DEBUG) MSPrintf(pc_uart, "Station IP =%s is updated!\r\n", station_IP);
         delay(500);
         UART_Flush();
     }


CHECK_AP_ERROR:

    //--------------------------- Task-5 ----------------------------------//

    if(check_AP == YES){

         if(!ESP8266_CheckAP())  //Cmd: AT+CWJAP?
         {
             delay(3000);
             if(DEBUG) MSPrintf(pc_uart, "AP details: Error \r\n");
             goto CHECK_AP_ERROR;
         }

         if(DEBUG) MSPrintf(pc_uart, "Connected AP: \r\n%s \r\n\r\n",esp866_data);
         UART_Flush();
         delay(500);
     }


LIST_OF_IPs_ERROR:

    //--------------------------- Task-6 ----------------------------------//

    if(List_AP == YES){

        if(!ESP8266_AssociatedIPs())      //Cmd: AT+CIFSR
        {
            delay(3000);

            if(DEBUG) MSPrintf(pc_uart, "Local IPs List: Error \r\n");
            goto LIST_OF_IPs_ERROR;
        }

        if(DEBUG) MSPrintf(pc_uart, "Local Associated IPs are: \r\n%s \r\n",esp866_data);
        UART_Flush();
        delay(500);
      }


MULTI_CONNECTION_ERROR:


    //--------------------------- Task-7 ----------------------------------//

    if(!ESP8266_EnableMultipleConnections(true))  //Cmd: AT+CIPMUX=1
    {
        delay(5000);

        if(DEBUG) MSPrintf(pc_uart, "Failed Multi-Connection\r\n");
        goto MULTI_CONNECTION_ERROR;
    }

    if(DEBUG) MSPrintf(pc_uart, "Multi-Connection is ON..!\r\n");
    UART_Flush();
    delay(500);


    //--------------------------- Task-8 ----------------------------------//

    if(connection_mode == SERVER){

        if(!ESP8266_StartServer(port))    //Cmd: AT+CIPSERVER=1
        {
            delay(5000);
            if(DEBUG) MSPrintf(pc_uart, "Failed to start Server\r\n", port);
            goto MULTI_CONNECTION_ERROR;
        }

        if(DEBUG) MSPrintf(pc_uart, "Server is ON at port:%s\r\n", port);
        UART_Flush();
        delay(500);
    }

}



//--------------------------- Fucntion-Systick Timer Interrupt-----------------------//

void Systick_Setup(void)
{
        // Enable SysTick Module
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; //clk: Core clock, systick: Enable

        // Set SysTick period = 3M/3k = 1000 tick, 1msec
        SysTick->LOAD = 3000 - 1;       //24bit register

        // Clear the SysTick current value register by writing
        // a dummy value
        SysTick->VAL = 0x01;           //current 24bit value, RW register

        // Enable SysTick interrupt
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(void)
{
    SysTick->VAL = 0x01;
    Systick_Cnt++;
}

//--------------------------- Fucntion-delay() ----------------------------------//

void delay(uint16_t  ms)
{
    uint32_t pre_Ticks = Systick_Cnt;
    while ((Systick_Cnt - pre_Ticks) < ms) ;
    Systick_Cnt = 0;            //Reset Counter to avoid Overflow
}


//--------------------------- Fucntion- Clock system CS() ----------------------------------//
void set3MhzClock(void)
{
    CS->KEY = CS_KEY_VAL;            // Unlock CS module for register access
    CS->CTL0 = 0;                   // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_1;   // Set DCO to 3MHz (nominal, center of 8-16MHz range)

    CS->CTL1 = CS_CTL1_SELA__REFOCLK |      // ACLK = REFO(Int. ref Low fre = 32.728kHz)
               CS_CTL1_SELS__DCOCLK |       // SMCLK = DCO (3MHz)
               CS_CTL1_SELM__DCOCLK;        // MCLK = DCO
    CS->KEY = 0;
}


