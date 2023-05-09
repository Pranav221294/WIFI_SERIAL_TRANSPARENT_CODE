#include <Arduino.h>
#include <WiFi.h>
#include "driver/uart.h"


#define NUMERO_PORTA_SERIALE UART_NUM_2
#define BUF_SIZE (1024 * 2)
#define RD_BUF_SIZE (1024)

void UART_ISR_ROUTINE(void *pvParameters);
void UART_2_init();
char* get_string();
//static const char * TAG = "";                  
 
#define U2RXD 16
#define U2TXD 17

