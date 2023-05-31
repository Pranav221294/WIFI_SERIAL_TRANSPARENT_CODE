
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
static int baud_rate , packet_leng;

/****************************************************************
Function: sendStringToServer

Description:
Sends a string to a server using a TCP client. 
The function checks if the client is connected and if there is enough space to 
send the message before sending it.

Parameters:

sendMsg: a String object that contains the message to send.
Returns:
None.
***************************************************************/


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

/****************************************************************
Function: handleData

Description:
Receives data from a server through a TCP client and converts it to a string. 
The function sets the TCP receive state and transmits the data over UART.

Parameters:
- arg: a void pointer to additional arguments (not used).
- client: a pointer to an AsyncClient object that represents the TCP client.
- data: a void pointer to the received data.
- len: the length of the received data.

Returns:
None.
***************************************************************/


void handleData(void* arg, AsyncClient* client, void *data, size_t len)
{
    
 // convert data to a string
   
    sprintf(tcp_read, "%s", (char*)data);
    Serial.write((uint8_t*)tcp_read, len);
    settcprecvstate(e_tcp_data_coming);//Setting flag that data is recived from TCP server in buffer tcp_read
    // transmit data over UART
   //uart_tx_chars(EX_UART_NUM, tcp_read, strlen(tcp_read));
   
}

/***************************************************************
 * Function Name: handleError
 * 
 * Description: This function is called when an error occurs in the TCP connection.
 * 
 * Parameters: 
 * - arg: a pointer to additional arguments (not used here)
 * - client: a pointer to the AsyncClient object that caused the error
 * - error: the error code that caused the error
 * 
 * Return: None
 ***************************************************************/

void handleError(void* arg, AsyncClient* client, int8_t error) 
{
  Serial.println("Error in TCP connection");
  Serial.print("[TCP] Error: ");
  Serial.println(error);
  tcp_connected = false;
  client->close();
}

/****************************************************************
Function: handleTimeOut
Description: Callback function called when the TCP client ACK times out
Input:

arg: a pointer to the argument
client: a pointer to the TCP client object
time: the time when the timeout occurred
Output: None
****************************************************************/

void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) 
{
    Serial.println("[CALLBACK] ACK timeout");

    Serial.println("[TCP] ACK timeout");
    tcp_connected = false;
    client->close();
}

/****************************************************************
Function: handleDisconnect
Description: Callback function called when the TCP client disconnects
Input:

arg: a pointer to the argument
client: a pointer to the TCP client object
Output: None
****************************************************************/


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

/****************************************************************
Function: handleConnect
Description: Callback function called when the TCP client connects
Input:

arg: a pointer to the argument
client: a pointer to the TCP client object
Output: None
****************************************************************/


void handleConnect(void* arg, AsyncClient* client) 
{
   Serial.println("[TCP] Connected");
  tcp_connected = true;
}

/****************************************************************
 * Function: wifiEventHandler
 * Description: Handles WiFi events such as connected and disconnected events. 
 * Parameters: WiFiEvent_t event - 
 * WiFi event type (SYSTEM_EVENT_STA_GOT_IP or SYSTEM_EVENT_STA_DISCONNECTED)
 * Return: None
 ***************************************************************/
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


/****************************************************************
 * onAckCallback - a callback function triggered when an ACK is 
 * received from the server. It prints the length of the packet
 * for which ACK is received and the time at which ACK was received.
 * 
 * @param arg: a pointer to an argument to be passed to the callback
 * @param client: a pointer to the AsyncClient object
 * @param len: length of the packet for which ACK was received
 * @param time: the time at which ACK was received
 ***************************************************************/

void onAckCallback(void* arg, AsyncClient* client, size_t len, uint32_t time) 
{
  Serial.print("ACK received for packet of length ");
  Serial.print(len);
  Serial.print(" at time ");
  Serial.println(time);
  lastAckTime = time;
}
/*****************************************************************

Function Name: convert_wifi_parma_read_running_mode
Description: This function reads and converts WiFi parameters in running mode.
It gets the WiFi user ID and password,converts the TCP server IP address and
port from string to byte and integer respectively, and sets them up for use.
Parameters: None
Return: None
*****************************************************************/
void convert_wifi_parma_read_running_mode()// It used read from memory when power on the values which durning wifi configuration
{
    byte server_ip_bytes[4];
    int index = 0;

     const Wifi_Param_String* wifi_params = getWifiParams();// It get parameter from Wifi_config.cpp
     convert_wifi_param_live_mode = *wifi_params;
     Serial.printf("Wifi_User_ID: %s\n", convert_wifi_param_live_mode.Wifi_User_ID);
     Serial.printf("Wifi_User_IDasd: %s\n", convert_wifi_param_live_mode.TCP_Server_IP);

      WIFI_NETWORK     = convert_wifi_param_live_mode.Wifi_User_ID;//Wifi User ID
      WIFI_PASSWORD =  convert_wifi_param_live_mode.Wifi_Password; //Password
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
    
   serverIP = IPAddress(server_ip_bytes[0], server_ip_bytes[1], server_ip_bytes[2], server_ip_bytes[3]);//Setting of TCP Server
   port  =  atoi(convert_wifi_param_live_mode.TCP_Port);//Setting TCP port

   Serial.println(serverIP);   
   Serial.println(port);
   Serial.printf("WIFI_NETWORK: %s\n",WIFI_NETWORK);
   Serial.printf("WIFI_PASSWORD: %s\n",WIFI_PASSWORD);
   //WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
              
        baud_rate  =  atoi(convert_wifi_param_live_mode.Baud_rate) ;
        packet_leng = atoi(convert_wifi_param_live_mode.Wifi_Packet_length) ;
}
/****************************************************************
 * Function Name: wifi_connection
 * Description: Connects to a WiFi network and initializes TCP client.
 * Parameters: None
 * Returns: None
 ***************************************************************/
void wifi_connection()
{

   WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);// Setting USEr ID n password (initlsation)
    
    unsigned long startTime = millis();
while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)// every 20 sec wifi connection
 {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  // Check if WiFi connection was successful
  if (WiFi.status() != WL_CONNECTED)
   {
    Serial.println("Failed to connect to WiFi network.");
     UART_1_init();
   UART_2_init();
    // Take some action, such as restarting the device or trying again later
  } 
  else
   {// Setting parameter for TCP Server  on Non blocking using callbacks
    Serial.println("Connected to WiFi network.");
    Serial.print("Local IP address: ");
    Serial.println(WiFi.localIP());

    tcpClient.onData(&handleData, NULL);//On data Handle
    tcpClient.onError(&handleError, NULL);//On Error Handle
     tcpClient.onDisconnect(&handleDisconnect, NULL);//It check if TCP server is avialable or not than it search for TCP server
     tcpClient.onConnect(&handleConnect,NULL);// Also we connected to TCP server
    
     Serial.printf("serverIP");
     //Serial.printf(" port %d", serverIP);
     Serial.println(serverIP); 
     Serial.printf("Port");
     Serial.println(port);
    tcpClient.connect( serverIP , port );//It used to connect paritcular TCP server and port

    WiFi.onEvent(wifiEventHandler);// If wifi is connected it check connection every 30 sec
     UART_1_init();
    UART_2_init();


    // Do other things, such as connecting to a server, etc.
  }



}
/****************************************************************
* Function Name: tcp_client_off
* Description: This function turns off the TCP client and clears all its callback functions.
* Parameters: None
* Return: None
****************************************************************/

void tcp_client_off()
{
   
    tcpClient.onData(NULL, NULL);
    tcpClient.onError(NULL, NULL);
    tcpClient.onTimeout(NULL, NULL);
    tcpClient.onDisconnect(NULL, NULL);
    tcpClient.onConnect(NULL, NULL);

    tcpClient.close();
             
}

/****************************************************************

Resets the tcpClient object and sets its callback functions to
handleData, handleError, handleDisconnect, and handleConnect.
***************************************************************/

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

/*****************************************************************
Function name: getTcpReadData
Input: None
Output: char pointer
Description: This function returns the TCP read data and clears the
 buffer after copying it into a static string. The size of the string 
 is limited to 128 characters.
*******************************************************************/

char* getTcpReadData()
{
  static char str[128];
  snprintf(str, sizeof(str), "%s", (const char*)tcp_read);
  memset(tcp_read, 0, sizeof(tcp_read)); // clear UART2_data
  return str;
 
}

/***************************************************************

Description: Sets the state of receiving TCP data.
Parameters:
state: the state of receiving TCP data (e_tcp_reve_data).
***************************************************************/

void settcprecvstate(e_tcp_reve_data state)
{
    tcp_data_status = state ;
}

/***************************************************************

Description: Returns the state of receiving TCP data.
Return:
The state of receiving TCP data (e_tcp_reve_data).
***************************************************************/

e_tcp_reve_data gettcprecvstate(void)
{
    return tcp_data_status ;
}

/***************************************************************

Description: Returns the baud rate for serial communication.
Return:
The baud rate for serial communication (unsigned int).
***************************************************************/

unsigned int baud_r()
{
    return baud_rate;
   
}

/***************************************************************

Description: Returns the packet length for TCP communication.
Return:
The packet length for TCP communication (unsigned int).
***************************************************************/

unsigned int packet_length()
{
    return packet_leng;
   
}