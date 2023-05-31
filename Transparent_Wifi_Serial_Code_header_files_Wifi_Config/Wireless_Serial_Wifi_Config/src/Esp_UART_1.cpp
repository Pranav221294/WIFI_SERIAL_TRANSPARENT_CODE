#include "Arduino.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include <stdio.h>
#include <string.h>
#include "Esp_UART_1.h"
#include "Wireless_Serial.h"

static intr_handle_t handle_console;
int8_t rxbuf_1[256];
uint16_t urxlen;
uint8_t uart_f_1 = 0;
static int baud_rate_1 ;

e_UART_1_reve_data UART_1_data_status = e_UART_1_no_data;
/****************************************************************
 * Function Name: uart_intr_handle
 * Description: Interrupt handler for UART1
 * Parameters: void *arg - pointer to void argument
 * Return: none
 ***************************************************************/

void IRAM_ATTR uart_intr_handle(void *arg)
{
  uint16_t rx_fifo_len, status;
  uint16_t i=0;
  status = UART1.int_st.val; // read UART interrupt Status
  rx_fifo_len = UART1.status.rxfifo_cnt; // read number of bytes in UART buffer
  while(rx_fifo_len)
  {
   rxbuf_1[i++] = UART1.fifo.rw_byte; // read all bytes
   rx_fifo_len--;
  }
 //uart_clear_intr_status(EX_UART_NUM, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);//clear UART interrupt status
 UART1.int_clr.rxfifo_full = 1; //clear RX full interrupt
 UART1.int_clr.rxfifo_tout = 1; //clear RX timeout interrupt
 uart_f_1 = 1;
 setUART1recvstate(e_UART_1_data_coming);//Set the flag 
 //uart_write_bytes(EX_UART_NUM, (const char*)rxbuf_1, i);
//  //Serial.println("LEN= ");Serial.println(UART2_data_length);
 
//                 Serial.print("DATA= ");
//                 for(byte j=0; j<i;j++) Serial.print((char)rxbuf_1[i]);
//                 Serial.println("");
  //memset(rxbuf_1, '\0', i); 


}

/**************************************************************
 * Function: UART_1_init()
 * ------------------------
 * Initializes UART1 with the specified baud rate, data bits, 
 * parity, stop bits, and flow control. The UART pins are set
 * to GPIO4 (RX) and GPIO5 (TX). The UART driver is installed
 * with a buffer size of BUF_SIZE_1*2 and a pre-registered
 * ISR is freed. A new ISR is registered with the IRAM flag set,
 * and the RX interrupt is enabled.
 **************************************************************/


void UART_1_init()
{
 
  baud_rate_1 = baud_r();
  Serial.println("baud_rate_1");
  Serial.println(baud_rate_1);
  if(baud_rate_1 == 0) baud_rate_1 = 115200;
    
	 uart_config_t uart_config =
  {
    .baud_rate = baud_rate_1,
   // .baud_rate = 19200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(EX_UART_NUM, &uart_config);
 // uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_pin(EX_UART_NUM,GPIO_NUM_4,GPIO_NUM_5,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
  uart_driver_install(EX_UART_NUM, BUF_SIZE_1 * 2, 0, 0, NULL, 0);//Install UART driver, and get the queue.
  uart_isr_free(EX_UART_NUM);// release the pre registered UART handler/subroutine
  uart_isr_register(EX_UART_NUM,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console); // register new UART subroutine
  uart_enable_rx_intr(EX_UART_NUM); // enable RX interrupt
	
	
	
}



char* get_UART_1_Read_Data()// Read function UART 1 data in interrupt
{
  
   
  static char str[128];
  snprintf(str, sizeof(str), "%s", (const char*)rxbuf_1);
  memset(rxbuf_1, 0, sizeof(rxbuf_1)); // clear UART2_data
  return str;
 
}

void setUART1recvstate(e_UART_1_reve_data state)//Using to set flag to receive data using enum
{
    UART_1_data_status = state ;
}

e_UART_1_reve_data getUART1recvstate(void)//Using to get flag to receive data using enum
{
    return UART_1_data_status ;
}

void disable_UART()
{
  uart_disable_rx_intr(EX_UART_NUM); // Disable RX interrupt
}

void UART_1_deinit()
{
  uart_disable_rx_intr(EX_UART_NUM); // Disable RX interrupt
  uart_isr_free(EX_UART_NUM); // Free the ISR
  uart_driver_delete(EX_UART_NUM); // Delete the UART driver and release resources
}