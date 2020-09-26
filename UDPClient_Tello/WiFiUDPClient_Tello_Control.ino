/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
//const char * networkName = "your-ssid";
const char * networkName = "TELLO-C377F8";
const char * networkPswd = "";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.10.1";
const int udpPort = 8889;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

typedef struct
{
  char *cmd;
  int delay_time;
}setCommand;

setCommand telloCommand[] =
{
  {"command",   1000},
  {"takeoff",   10000},
  {"left 50",   5000},
  {"right 50",  5000},
  {"back 50",   5000},
  {"forward 50",5000},
  {"up 50",     5000},
  {"down 50",   5000},
  {"cw 360",    10000},
  {"ccw 360",   10000},
  {"flip f",    5000},
  {"land",      1000},
  {"land",      1000},
  {NULL,        0}
};

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop(){
  //only send data when connected
  if(connected){
    
    for(int i=0; telloCommand[i].cmd != NULL ; i++){
      sendMessage(telloCommand[i].cmd);
      receiveMessage();
      Serial.println(telloCommand[i].cmd);
      delay(telloCommand[i].delay_time);
    }
    connected = 0;
  }
  //Wait for 1 second
  delay(1000);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}

int receiveMessage() {
  int packetSize = 0; 
  int ercd = -1;
  char packetBuffer[255];
  String message = "";
  
  delay(200);
  packetSize = udp.parsePacket();
   
  if (packetSize) {
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      message = (String)packetBuffer;
      if(message.equals("ok")){
         ercd = 1;
      }
      else if(message.equals("error")){
         ercd = 2;
      }
      Serial.println(message);
    }
  }
  else{
 }
  return ercd;
}

void sendMessage(char* Buffer) {
  udp.beginPacket(udpAddress, udpPort);
  udp.printf(Buffer);
  udp.endPacket();
}
