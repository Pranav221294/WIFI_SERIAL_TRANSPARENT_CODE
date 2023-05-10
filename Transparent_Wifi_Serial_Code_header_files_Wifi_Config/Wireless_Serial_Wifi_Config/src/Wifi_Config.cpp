 
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "Esp_UART_1.h"
#include "Esp_UART_2.h"
#include "Wireless_Serial.h"
#include "Wifi_Config.h"
#include <Preferences.h>

Wifi_Param_String wifi_write_cmd;
Wifi_Param_String wifi_read_cmd;
Wifi_Param_String wifi_read_cmd_for_GUI;

 extern uint8_t hello;
   
buttonClick_type eButtonClick = e_buttonNotCickedInitial;
static unsigned long int connection_ckeck_milli;
extern Preferences preferences;
static void extract_name(const char* input, char* name) ;
static void Wifi_Config_Rx(void);



static void Wifi_Config_Rx(void)
{
  char* my_string; 
  char ch1 ;
  char ch2 ;
  char mac_addr[18];
  my_string = get_string();

 ch1 = my_string[2]; 
 ch2 = my_string[3];

  int combined = (ch1 << 8) | ch2;
   hello = 0;
 switch (combined) 
  {
    case ((int)'0' << 8) | '1':
         extract_name((const char*) my_string, wifi_write_cmd.Wifi_User_ID) ;
          Serial.print("Wifi User id ");
          Serial.println(wifi_write_cmd.Wifi_User_ID);
          combined = 0;
         break;
  
     case ((int)'0' << 8) | '2':
          extract_name((const char*) my_string, wifi_write_cmd.Wifi_Password) ;
          Serial.print("Wifi pass ");
          Serial.println(wifi_write_cmd.Wifi_Password);
          combined = 0;
          break;

    case ((int)'0' << 8) | '3':
         extract_name((const char*) my_string, wifi_write_cmd.TCP_Server_IP) ; 
         Serial.print("TCP Server ");
         Serial.println(wifi_write_cmd.TCP_Server_IP);
         combined = 0;
         break;
  case ((int)'0' << 8) | '4':
        extract_name((const char*) my_string, wifi_write_cmd.TCP_Client_IP) ; 
        Serial.print("TCP Client ");
        Serial.println(wifi_write_cmd.TCP_Client_IP);
        combined = 0;
        break;
  case ((int)'0' << 8) | '5':
       extract_name((const char*) my_string, wifi_write_cmd.TCP_Port) ;
       Serial.print("TCP Port ");
       Serial.println(wifi_write_cmd.TCP_Port);
       combined = 0;
       break;
   case ((int)'0' << 8) | '6':
       // extract_name((const char*) my_string, wifi_write_cmd.MAC_Addr) ;
        strcpy(mac_addr, WiFi.macAddress().c_str());
        strcpy(wifi_write_cmd.MAC_Addr, mac_addr);
        Serial.print("MAC Addr ");
        Serial.println(wifi_write_cmd.MAC_Addr);
        combined = 0;
        break;
    case ((int)'0' << 8) | '7':
         extract_name((const char*) my_string, wifi_write_cmd.Wifi_Packet_length) ;
         Serial.print("Packet Length ");
        Serial.println(wifi_write_cmd.Wifi_Packet_length);
        combined = 0;
         break;
    case ((int)'0' << 8) | '8':
         extract_name((const char*) my_string, wifi_write_cmd.Baud_rate) ;
         Serial.print("Baud Rate ");
         Serial.println(wifi_write_cmd.Baud_rate);
         combined = 0;
         break;     
    case ((int)'S' << 8) | 'T':
        setButtonState(e_buttonStartWifiConfig); 
         Serial.println("#SSTWCON@");
         break;   
     case ((int)'C' << 8) | 'M':
          setButtonState(e_buttonClearMemory); 
         Serial.println("#CCMEM@");
         break;
    case ((int)'S' << 8) | 'M':
         setButtonState(e_buttonSaveMemory);
         Serial.println("#SSMEM@");
         break;
    case ((int)'R' << 8) | 'M':
         setButtonState(e_buttonReadMemory);
         Serial.println("#RRMEM@");
         break;  
    case ((int)'C' << 8) | 'O':
         Serial.println("#CCON@");
         connection_ckeck_milli=millis();
         break;                           
  default:
    
    break;   

}

memset(my_string, 0, strlen(my_string));


}

static void extract_name(const char* input, char* name) 
{
    const char *name_ptr = input + 7;  // 7 is the index of the first character of the name
    int i = 0;
    char c;
    
    // Copy characters from the name pointer to the name array until the '@' character is encountered
    while ((c = *(name_ptr + i)) != '@' && c != '\0') 
    {
        *(name + i) = c;
        i++;
    }
    
    *(name + i) = '\0';  // Add a null terminator to the end of the name string
}

void Wifi_Config_Setting_by_GUI()
{
      Wifi_Config_Rx();
}

 void wifi_save_config()
{
 preferences.begin("wifi_config", false);  
 preferences.putBytes("config", &wifi_write_cmd, sizeof(wifi_write_cmd));
 preferences.end();
 Serial.printf("SSID: %s\n", wifi_read_cmd.Wifi_User_ID);
}

void wifi_restore_config()
{
 preferences.begin("wifi_config", false);    
 preferences.getBytes("config", &wifi_read_cmd, sizeof(wifi_read_cmd));
 preferences.end();

  Serial.printf("SSID: %s\n", wifi_read_cmd.Wifi_User_ID);
  Serial.printf("Password: %s\n", wifi_read_cmd.Wifi_Password);
  Serial.printf("TCP Server IP: %s\n", wifi_read_cmd.TCP_Server_IP);
  Serial.printf("TCP Client IP: %s\n", wifi_read_cmd.TCP_Client_IP);
  Serial.printf("TCP Port: %s\n", wifi_read_cmd.TCP_Port);
  Serial.printf("MAC Address: %s\n", wifi_read_cmd.MAC_Addr);
  Serial.printf("WiFi Packet Length: %s\n", wifi_read_cmd.Wifi_Packet_length);
  Serial.printf("Baud Rate: %s\n", wifi_read_cmd.Baud_rate);
 
}


void wifi_restore_config_to_GUI()
{
  preferences.begin("wifi_config", false);    
 preferences.getBytes("config", &wifi_read_cmd, sizeof(wifi_read_cmd));
 preferences.end();

 sprintf(wifi_read_cmd_for_GUI.Wifi_User_ID,"@RWU,%s#\r\n\0",wifi_read_cmd.Wifi_User_ID);
 uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.Wifi_User_ID, strlen(wifi_read_cmd_for_GUI.Wifi_User_ID));
 
 sprintf(wifi_read_cmd_for_GUI.Wifi_Password,"@RWP,%s#\r\n\0",wifi_read_cmd.Wifi_Password);
 uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.Wifi_Password, strlen(wifi_read_cmd_for_GUI.Wifi_Password));
 
 sprintf(wifi_read_cmd_for_GUI.TCP_Server_IP,"@RTS,%s#\r\n\0",wifi_read_cmd.TCP_Server_IP);
 uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.TCP_Server_IP, strlen(wifi_read_cmd_for_GUI.TCP_Server_IP));

 sprintf(wifi_read_cmd_for_GUI.TCP_Client_IP,"@RTC,%s#\r\n\0",wifi_read_cmd.TCP_Client_IP);
 uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.TCP_Client_IP, strlen(wifi_read_cmd_for_GUI.TCP_Client_IP));

sprintf(wifi_read_cmd_for_GUI.TCP_Port,"@RTP,%s#\r\n\0",wifi_read_cmd.TCP_Port);
uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.TCP_Port, strlen(wifi_read_cmd_for_GUI.TCP_Port));

sprintf(wifi_read_cmd_for_GUI.MAC_Addr,"@RMC,%s#\r\n\0",wifi_read_cmd.MAC_Addr);
uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.MAC_Addr, strlen(wifi_read_cmd_for_GUI.MAC_Addr));

sprintf(wifi_read_cmd_for_GUI.Wifi_Packet_length,"@RPL,%s#\r\n\0",wifi_read_cmd.Wifi_Packet_length);
uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.Wifi_Packet_length, strlen(wifi_read_cmd_for_GUI.Wifi_Packet_length));

sprintf(wifi_read_cmd_for_GUI.Baud_rate,"@RBR,%s#\r\n\0",wifi_read_cmd.Baud_rate);
uart_write_bytes(UART_NUM_2, (const char*)wifi_read_cmd_for_GUI.Baud_rate, strlen(wifi_read_cmd_for_GUI.Baud_rate));

memset(&wifi_read_cmd_for_GUI, 0, sizeof(wifi_read_cmd_for_GUI));
 memset(&wifi_read_cmd, 0, sizeof(wifi_read_cmd));

}

void clear_wifi_config_mem()
{
 preferences.begin("wifi_config", false);
 preferences.remove("config");
 //preferences.clear();
 preferences.end();
}

void setButtonState(buttonClick_type state)
{
    eButtonClick = state ;
}

buttonClick_type getButtonClickedState(void)
{
    return eButtonClick ;
}

const Wifi_Param_String* getWifiParams() 
{
    return &wifi_read_cmd;
}

unsigned long int connection_check()
{
   
   return connection_ckeck_milli;
}