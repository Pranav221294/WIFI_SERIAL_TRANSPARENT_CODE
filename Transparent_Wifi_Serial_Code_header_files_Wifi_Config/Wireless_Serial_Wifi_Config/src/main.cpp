#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "Esp_UART_1.h"
#include "Esp_UART_2.h"
#include "Wireless_Serial.h"
#include "Wifi_Config.h"
#include <Preferences.h>

#define MIN_CONNECTED_TIMEOUT 2000 // 2 seconds
#define MAX_CONNECTED_TIMEOUT 30000
typedef enum
{
	e_main_app,
  e_wifi_config,
}e_states_type;

e_states_type state = e_main_app;

buttonClick_type ebutton_in_main = e_buttonNotCickedInitial;
e_tcp_reve_data tcp_data_status_f = e_tcp_no_data;
e_UART_1_reve_data UART_1_data_status_f = e_UART_1_no_data;
//extern const char* WIFI_NETWORK ;
//extern const char* WIFI_PASSWORD;

extern AsyncClient tcpClient;

extern IPAddress serverIP;
extern char port;

extern int8_t rxbuf_1[256];
extern uint8_t uart_f_1;
uint8_t hello =0;
//uint8_t tcp_f = 0;

extern uint8_t UART2_data[128];
extern uint8_t uart_f_2;

extern char tcp_read[256];

unsigned int count12  = 0;
const char* MSG = "@OK#\r\n";
Preferences preferences;
unsigned long int connection_ckeck_milli_1;

char * tcp_uart_str; 
char * uart_tcp_str;

void setup() 
{
  Serial.begin(115200);
  
   UART_1_init();
   UART_2_init();
   //preferences.begin("wifi_config", false);
   wifi_restore_config();// it will retrive from info from eeprom
   convert_wifi_parma_read_running_mode();
   wifi_connection();
   
       
   


   

    // tcpClient.onData(&handleData, NULL);
    // tcpClient.onError(&handleError, NULL);
   
    // tcpClient.onDisconnect(&handleDisconnect, NULL);
    // tcpClient.onConnect(&handleConnect,NULL);
    
   

   // tcpClient.connect( serverIP , port );

    WiFi.onEvent(wifiEventHandler);
    
}

void loop()
{

     
        
  switch (state)
  {
  case e_main_app:
        Wifi_Config_Setting_by_GUI();
        ebutton_in_main = getButtonClickedState();
        tcp_data_status_f = gettcprecvstate();
        UART_1_data_status_f =  getUART1recvstate();

        if(ebutton_in_main == e_buttonStartWifiConfig)
           {
              state = e_wifi_config;
              tcp_client_off(); 
              uart_write_bytes(UART_NUM_2,(const char*)MSG, strlen(MSG));
              setButtonState(e_buttonNotCickedInitial);
           }
         if(tcp_data_status_f == e_tcp_data_coming) 
         {
             tcp_uart_str = getTcpReadData();
             uart_tx_chars(EX_UART_NUM, tcp_uart_str, strlen(tcp_uart_str));
             settcprecvstate(e_tcp_no_data);

         }
        if(UART_1_data_status_f == e_UART_1_data_coming)
          {
             Serial.println("uart_tcp_str__");
             Serial.println(uart_tcp_str);
             uart_tcp_str = get_UART_1_Read_Data();
             String stringObject((const char*)uart_tcp_str);
            sendStringToServer(stringObject);
            stringObject.remove(0);
            setUART1recvstate(e_UART_1_no_data);

          }
      
    break;
  case e_wifi_config:   
       Wifi_Config_Setting_by_GUI();
       ebutton_in_main = getButtonClickedState();
       connection_ckeck_milli_1 = connection_check();
    //   Serial.println("connection_ckeck_milli_1");Serial.println(connection_ckeck_milli_1);
       if(ebutton_in_main == e_buttonSaveMemory)
       {
           wifi_save_config();
           state = e_main_app;
           setButtonState(e_buttonNotCickedInitial);
           Serial.println("e_main_app");
            ESP.restart();
       }
       if(ebutton_in_main == e_buttonClearMemory)
       {
           clear_wifi_config_mem();
           setButtonState(e_buttonNotCickedInitial);
       }
       if(ebutton_in_main == e_buttonReadMemory)
          {
           
            wifi_restore_config_to_GUI();
            setButtonState(e_buttonNotCickedInitial);
         
          }
        if(millis()-connection_ckeck_milli_1>MAX_CONNECTED_TIMEOUT)
           {
             count12++;Serial.println("count");Serial.println(count12);
             if(count12 > 10)
             {
              state = e_main_app;
               tcp_client_on();
               Serial.println(connection_ckeck_milli_1);
               Serial.println("main");
                count12 = 0;
                ESP.restart();
                
             }

           }  
    
    break;

  default:
    break;
  }



}
// void loop() 
// {
 
// // if(hello==0)
//  {
//   test_fun();
//    //Serial.println("hello =0");
//  // wifi_save_config();
//   hello = 1;
//  } 
//   if(uart_f_1 == 1)
//     {
     
//      String stringObject((const char*)rxbuf_1);
//      sendStringToServer(stringObject);
//      memset(rxbuf_1, '\0', sizeof(rxbuf_1));
//      stringObject.remove(0);
//      Serial.println("uart_f_1 =1");
//      uart_f_1 = 0;
    
//     }
// }