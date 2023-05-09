#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>


typedef enum
{
    e_tcp_data_coming,
    e_tcp_no_data,
    
}e_tcp_reve_data;


void sendStringToServer( String sendMsg); 
void handleData(void* arg, AsyncClient* client, void *data, size_t len);
void handleError(void* arg, AsyncClient* client, int8_t error);
void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) ;
void handleDisconnect(void* arg, AsyncClient* client) ;
void handleConnect(void* arg, AsyncClient* client) ;
void wifiEventHandler(WiFiEvent_t event);
void onAckCallback(void* arg, AsyncClient* client, size_t len, uint32_t time) ;
void convert_wifi_parma_read_running_mode();
void wifi_connection();
void tcp_client_off();
void tcp_client_on();
char* getTcpReadData();
void settcprecvstate(e_tcp_reve_data state);
e_tcp_reve_data gettcprecvstate(void);