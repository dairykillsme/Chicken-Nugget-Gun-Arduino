#include <Arduino.h>
#include <WebSocketConn.h>
#include <CodeChecker.h>
#include <Keypad.h>

//Websocket and Wifi Constants and Variables
const char * SSID = "SIMPLEWIFI";
const char * PWD = "198Geniuses";

WebSocketsServer webSocket(81);

//Code Constants and Variables
const byte NUM_CODES = 5;
const byte CODE_LENGTH = 5;

CodeChecker codes = CodeChecker(NUM_CODES, CODE_LENGTH);
bool codesSent = false;

const byte STANDBY_CODE = 0;
const byte RELOAD_CODE = 1;
const byte LASER_CODE = 2;
const byte CALIBRATION_CODE = 3;
const byte CONSENT_CODE = 4;

//Keypad Constants and Variables
const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {D0, D1, D2, D3}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {D5, D6, D7}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

unsigned long lastMillis;

// gun states
bool standby = false;

// function prototypes
void SendCodes();
bool CheckStandby();

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
  if (!codesSent)
  {
    SendCodes();
  }
  else
  {
    if (CheckStandby())
    {

    }
  }
}

void SendCodes()
{
  for (int i = 0; i < webSocket.connectedClients(); i++)
  {
    String msg = "CODES," + codes.getCode(STANDBY_CODE) + "," + codes.getCode(RELOAD_CODE) + "," + codes.getCode(LASER_CODE) + "," 
    + codes.getCode(CALIBRATION_CODE) + "," + codes.getCode(CONSENT_CODE);
    Serial.println(msg);
    webSocket.sendTXT(i, msg);
    codesSent = true;
  }
}

bool CheckStandby()
{
  if (standby)
  {
    return true;
  }
  else
  {
    char readKey = kpd.getKey();
    if (readKey != NO_KEY)
    {
      Serial.println(readKey);
      if (codes.readCode(readKey, STANDBY_CODE))
      {
        standby = true;
        Serial.println("Standby");
        for (int i = 0; i < webSocket.connectedClients(); i++)
        {
          webSocket.sendTXT(i, "STNDBY");
        }
        
        return true;
      }
    }
    return false;
  }
}