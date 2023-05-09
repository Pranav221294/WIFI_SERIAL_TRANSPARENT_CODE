#include <Arduino.h>
#include <WiFi.h>
#include "driver/uart.h"
#include "Esp_UART_2.h"

uint8_t rxbuf[256];     //Buffer di ricezione
uint16_t rx_fifo_len;        //Lunghezza dati
uint8_t UART2_data[128];
QueueHandle_t uart2_queue;
uint8_t uart_f_2 = 0;
void UART_2_init()
{

 
 //Configuro la porta Serial2 (tutti i parametri hanno anche un get per effettuare controlli)
    uart_config_t Configurazione_UART2 = 
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(NUMERO_PORTA_SERIALE, &Configurazione_UART2);
 
 
 
    //Firma: void esp_log_level_set(const char *tag, esp_log_level_tlevel)
    //esp_log_level_set(TAG, ESP_LOG_INFO);
 
 
   
    //Firma: esp_err_tuart_set_pin(uart_port_tuart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
    uart_set_pin(NUMERO_PORTA_SERIALE, U2TXD, U2RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
 
 
    //Firma: uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
    //       uart_driver_install(Numero_porta, RXD_BUFFER, TXD_Buffer, event queue handle and size, flags to allocate an interrupt)
    uart_driver_install(NUMERO_PORTA_SERIALE, BUF_SIZE, BUF_SIZE, 20, &uart2_queue, 0);
 
 
    //Create a task to handler UART event from ISR
    xTaskCreate(UART_ISR_ROUTINE, "UART_ISR_ROUTINE", 2048, NULL, 12, NULL);


}


 void UART_ISR_ROUTINE(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    bool exit_condition = false;
   
    //Infinite loop to run main bulk of task
    while (1) {
     
        //Loop will continually block (i.e. wait) on event messages from the event queue
        if(xQueueReceive(uart2_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
         
            //Handle received event
            if (event.type == UART_DATA) {
 
               // uint8_t UART2_data[128];
                int UART2_data_length = 0;
                ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_2, (size_t*)&UART2_data_length));
                UART2_data_length = uart_read_bytes(UART_NUM_2, UART2_data, UART2_data_length, 100);
                uart_f_2 = 1;
                 //uart_write_bytes(UART_NUM_2, (const char*)UART2_data, UART2_data_length); 
                //  Serial.println("LEN= ");Serial.println(UART2_data_length);
 
                //  Serial.print("DATA= ");
                //  for(byte i=0; i<UART2_data_length;i++) Serial.print((char)UART2_data[i]);
                // Serial.println("");
             
            }
           
            //Handle frame error event
            else if (event.type == UART_FRAME_ERR) {
                //TODO...
            }
           
            //Keep adding else if statements for each UART event you want to support
            //else if (event.type == OTHER EVENT) {
                //TODO...
            //}
           
           
            //Final else statement to act as a default case
            else {
                //TODO...
            }      
        }
       
        //If you want to break out of the loop due to certain conditions, set exit condition to true
        if (exit_condition) {
            break;
        }
    }
   
    //Out side of loop now. Task needs to clean up and self terminate before returning
    vTaskDelete(NULL);
}
 

 char* get_string()
{
  static char str[50];
  snprintf(str, sizeof(str), "%s", (const char*)UART2_data);
  memset(UART2_data, 0, sizeof(UART2_data)); // clear UART2_data
  return str;
}