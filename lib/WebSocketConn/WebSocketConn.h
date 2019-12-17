#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

void WifiConnect(const String &ssid, const String &password);
void StartWebSocket(WebSocketsServer &webSocket);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);