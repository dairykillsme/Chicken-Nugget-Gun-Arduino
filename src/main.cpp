#include <Arduino.h>
#include <WebSocketConn.h>

const char * SSID = "SIMPLEWIFI";
const char * PWD = "198Geniuses";

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

void setup() {
  Serial.begin(9600);
  WifiConnect(SSID, PWD);
  StartWebSocket(webSocket);
}

void loop() {
  webSocket.loop();
}