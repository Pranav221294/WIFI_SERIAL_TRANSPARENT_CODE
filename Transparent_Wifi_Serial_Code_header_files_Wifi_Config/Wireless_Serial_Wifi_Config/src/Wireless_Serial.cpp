
#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "Esp_UART_1.h"
#include "Esp_UART_2.h"
#include "Wireless_Serial.h"
#include "Wifi_Config.h"


Wifi_Param_String convert_wifi_param_live_mode;
//#define WIFI_NETWORK "AIRTELDI_EXT"
//#define WIFI_PASSWORD "9911227439dk"


//const char* WIFI_NETWORK     = "AIRTELDI_EXT";
//const char* WIFI_PASSWORD = "9911227439dk";
//#define WIFI_NETWORK "Galaxy M115463"
//#define WIFI_PASSWORD "31072020"


//#define WIFI_NETWORK "AIRTELDI"
//#define WIFI_PASSWORD "9911227439dk"


//extern IPAddress serverIP ;
// const char* WIFI_NETWORK     = "Krishna pg";
// const char* WIFI_PASSWORD = "9999922556";

const char* WIFI_NETWORK     = "000000000";
const char* WIFI_PASSWORD = "00000000";

//IPAddress serverIP = IPAddress(192, 168, 195, 171); 
//IPAddress serverIP = IPAddress(192, 168, 1, 6);   // change to server IP

//IPAddress serverIP = IPAddress(192, 168, 0, 111);
//IPAddress serverIP = IPAddress(192, 168, 1, 6);

//#define port 100
AsyncClient tcpClient;
unsigned long lastTick;

//IPAddress serverIP = IPAddress(192, 168, 0, 198);
IPAddress serverIP;
int port;
unsigned long wifireconnect_startTime = 0;


char tcp_read[256]={0};
bool connected = false;

const uint16_t ack_timeout = 5000; // 5 seconds
const uint16_t recon_timeout = 10000; // 10 seconds

bool tcp_connected = false;
unsigned long last_ack = 0;
uint32_t lastAckTime = 0; 
uint8_t  count = 0;

extern int8_t rxbuf_1[256];
extern uint8_t uart_f_1;
uint8_t tcp_f = 0;

extern uint8_t UART2_data[128];
extern uint8_t uart_f_2;

 e_tcp_reve_data tcp_data_status = e_tcp_no_data;

void sendStringToServer( String sendMsg )
{
    if( !tcpClient.connected()) { return; }
    if( !tcpClient.canSend() )  { return; }
    
    if ( tcpClient.space() > sendMsg.length() ) 
    {
          tcpClient.add( sendMsg.c_str() , sendMsg.length() );
          tcpClient.send();
    }
}

void handleData(void* arg, AsyncClient* client, void *data, size_t len)
{
    
 // convert data to a string
   
    sprintf(tcp_read, "%s", (char*)data);
    Serial.write((uint8_t*)tcp_read, len);
    settcprecvstate(e_tcp_data_coming);
    // transmit data over UART
   //uart_tx_chars(EX_UART_NUM, tcp_read, strlen(tcp_read));
   
}





void handleError(void* arg, AsyncClient* client, int8_t error) {
  Serial.println("Error in TCP connection");
  Serial.print("[TCP] Error: ");
  Serial.println(error);
  tcp_connected = false;
  client->close();
}

void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) 
{
    Serial.println("[CALLBACK] ACK timeout");

    Serial.println("[TCP] ACK timeout");
    tcp_connected = false;
    client->close();
}

void handleDisconnect(void* arg, AsyncClient* client) 
{
    Serial.println("[TCP] Disconnected");
  tcp_connected = false;
  client->close();
  if (WiFi.isConnected()) {
    Serial.println("[TCP] Reconnecting...");
   // delay(recon_timeout);
   tcpClient.connect( serverIP , port );
  }
}

void handleConnect(void* arg, AsyncClient* client) 
{
   Serial.println("[TCP] Connected");
  tcp_connected = true;
}


void wifiEventHandler(WiFiEvent_t event)
{
    switch(event) {
        case SYSTEM_EVENT_STA_GOT_IP:
            // WiFi connected and got IP address
            Serial.println("WiFi connected and got IP address");
           wifireconnect_startTime = millis();
            if (!connected) 
            {
                if (tcpClient.connect( serverIP , port )) 
                {
                    connected = true;
                   // Serial.println("Connected to server");
                } else {
                    Serial.println("Failed to connect to server");
                }
            }
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            // WiFi disconnected, trying to reconnect
            Serial.println("WiFi disconnected, trying to reconnect1");
            if (millis() - wifireconnect_startTime >= 30000)
           {
              wifireconnect_startTime = millis();
            Serial.println("WiFi disconnected, trying to reconnect11112");

            tcpClient.onData(NULL, NULL);
            tcpClient.onError(NULL, NULL);
            tcpClient.onTimeout(NULL, NULL);
            tcpClient.onDisconnect(NULL, NULL);
            tcpClient.onConnect(NULL, NULL);

             tcpClient.close();
             tcpClient = AsyncClient();
             tcpClient.onData(&handleData, &tcpClient);
             tcpClient.onError(&handleError, &tcpClient);
           //  tcpClient.onTimeout(&handleTimeOut, &tcpClient);
             tcpClient.onDisconnect(&handleDisconnect, &tcpClient);
             tcpClient.onConnect(&handleConnect,&tcpClient);
  
            connected = false;
            WiFi.begin(WIFI_NETWORK,WIFI_PASSWORD);
            }

            break;
           default:
            break;
    }
}

void onAckCallback(void* arg, AsyncClient* client, size_t len, uint32_t time) {
  Serial.print("ACK received for packet of length ");
  Serial.print(len);
  Serial.print(" at time ");
  Serial.println(time);
  lastAckTime = time;
}

void convert_wifi_parma_read_running_mode()
{
    byte server_ip_bytes[4];
    int index = 0;

     const Wifi_Param_String* wifi_params = getWifiParams();
     convert_wifi_param_live_mode = *wifi_params;
     Serial.printf("Wifi_User_ID: %s\n", convert_wifi_param_live_mode.Wifi_User_ID);
     Serial.printf("Wifi_User_IDasd: %s\n", convert_wifi_param_live_mode.TCP_Server_IP);

      WIFI_NETWORK     = convert_wifi_param_live_mode.Wifi_User_ID;
      WIFI_PASSWORD =  convert_wifi_param_live_mode.Wifi_Password; 
     Serial.printf("WIFI_NETWORK: %s\n",WIFI_NETWORK);
     Serial.printf("WIFI_PASSWORD: %s\n",WIFI_PASSWORD);
      
      char *tok = strtok(convert_wifi_param_live_mode.TCP_Server_IP, ",") ;
      while (tok != NULL && index < 4) 
      {
       server_ip_bytes[index] = atoi(tok);
       index++;
       tok = strtok(NULL, ",");
      }

    Serial.printf(" Server ip byte%d", server_ip_bytes[0]);
    Serial.printf(" %d", server_ip_bytes[1]);
    Serial.printf(" %d", server_ip_bytes[2]);
    Serial.printf(" %d\n", server_ip_bytes[3]);
    
   serverIP = IPAddress(server_ip_bytes[0], server_ip_bytes[1], server_ip_bytes[2], server_ip_bytes[3]);
   port  =  atoi(convert_wifi_param_live_mode.TCP_Port);

   Serial.println(serverIP);   
   Serial.println(port);
   Serial.printf("WIFI_NETWORK: %s\n",WIFI_NETWORK);
  Serial.printf("WIFI_PASSWORD: %s\n",WIFI_PASSWORD);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

}

void wifi_connection()
{

   WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
    
    unsigned long startTime = millis();
while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000)
 {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  // Check if WiFi connection was successful
  if (WiFi.status() != WL_CONNECTED)
   {
    Serial.println("Failed to connect to WiFi network.");
    // Take some action, such as restarting the device or trying again later
  } 
  else
   {
    Serial.println("Connected to WiFi network.");
    Serial.print("Local IP address: ");
    Serial.println(WiFi.localIP());

    tcpClient.onData(&handleData, NULL);
    tcpClient.onError(&handleError, NULL);
   
     tcpClient.onDisconnect(&handleDisconnect, NULL);
     tcpClient.onConnect(&handleConnect,NULL);
    
     Serial.printf("serverIP");
     //Serial.printf(" port %d", serverIP);
     Serial.println(serverIP); 
     Serial.printf("Port");
     Serial.println(port);
    tcpClient.connect( serverIP , port );

    WiFi.onEvent(wifiEventHandler);
    


    // Do other things, such as connecting to a server, etc.
  }



}


void tcp_client_off()
{
   
    tcpClient.onData(NULL, NULL);
    tcpClient.onError(NULL, NULL);
    tcpClient.onTimeout(NULL, NULL);
    tcpClient.onDisconnect(NULL, NULL);
    tcpClient.onConnect(NULL, NULL);

    tcpClient.close();
             
}

void tcp_client_on()
{

           tcpClient.onData(NULL, NULL);
            tcpClient.onError(NULL, NULL);
            tcpClient.onTimeout(NULL, NULL);
            tcpClient.onDisconnect(NULL, NULL);
            tcpClient.onConnect(NULL, NULL);

             tcpClient.close();
             tcpClient = AsyncClient();
             tcpClient.onData(&handleData, &tcpClient);
             tcpClient.onError(&handleError, &tcpClient);
           //  tcpClient.onTimeout(&handleTimeOut, &tcpClient);
             tcpClient.onDisconnect(&handleDisconnect, &tcpClient);
             tcpClient.onConnect(&handleConnect,&tcpClient);



}


char* getTcpReadData()
{
  static char str[128];
  snprintf(str, sizeof(str), "%s", (const char*)tcp_read);
  memset(tcp_read, 0, sizeof(tcp_read)); // clear UART2_data
  return str;
 
}

void settcprecvstate(e_tcp_reve_data state)
{
    tcp_data_status = state ;
}

e_tcp_reve_data gettcprecvstate(void)
{
    return tcp_data_status ;
}