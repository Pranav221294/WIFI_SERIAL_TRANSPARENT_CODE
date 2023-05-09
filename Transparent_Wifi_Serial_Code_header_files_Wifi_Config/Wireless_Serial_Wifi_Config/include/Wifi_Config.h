#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>


typedef struct 
{
    char Wifi_User_ID[60];
    char Wifi_Password[60];
    char TCP_Server_IP[50];
    char TCP_Client_IP[40];
    char TCP_Port[20];
    char MAC_Addr[30];
    char Wifi_Packet_length[10];
    char Baud_rate[10];
} Wifi_Param_String;

typedef enum
{
    e_buttonNotCickedInitial,
    e_buttonStartWifiConfig,
    e_buttonClearMemory,
    e_buttonSaveMemory,
    e_buttonReadMemory,
    e_buttonNoValue,
    
}buttonClick_type;


void Wifi_Config_Setting_by_GUI();
void wifi_restore_config();
void wifi_save_config();
void clear_wifi_config_mem();
void wifi_restore_config_to_GUI();
void setButtonState(buttonClick_type state);
buttonClick_type getButtonClickedState(void);
const Wifi_Param_String* getWifiParams() ;
unsigned long int connection_check();
