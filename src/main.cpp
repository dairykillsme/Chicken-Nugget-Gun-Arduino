#include <Arduino.h>
#include <WebSocketConn.h>
#include <CodeChecker.h>

const char * SSID = "SIMPLEWIFI";
const char * PWD = "198Geniuses";

const int NUM_CODES = 5;
const int CODE_LENGTH = 10;

WebSocketsServer webSocket(81);
CodeChecker codes = CodeChecker(NUM_CODES, CODE_LENGTH);

unsigned long lastMillis;

void setup() {
  //Communication Setup
  Serial.begin(9600);
  WifiConnect(SSID, PWD);
  StartWebSocket(webSocket);
  // Code Generation
  codes.Init();
  Serial.println("\nCODES:\n");
  for (int i = 0; i < NUM_CODES; i++)
  {
    Serial.println(codes.getCode(i));
  }
  //Get initial millis
  lastMillis = millis();
}

void loop() {
  webSocket.loop();
}