#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <CodeChecker.h>
#include <Keypad.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <SamsungCodes.h>

//Websocket and Wifi Constants and Variables
const char *SSID = "SIMPLEWIFI";
const char *PWD = "198Geniuses";

WebSocketsServer webSocket(81);

const int boi = D10;

//Code Constants and Variables
const byte NUM_CODES = 2;
const byte CODE_LENGTH = 5;

CodeChecker codes = CodeChecker(NUM_CODES, CODE_LENGTH);

const byte STANDBY_CODE = 0;
const byte CONSENT_CODE = 1;

//Keypad Constants and Variables
const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {D0, D1, D2, D3}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {D5, D6, D7};     //connect to the column pinouts of the kpd

Keypad kpd = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//IR Remote Setup and Constants
const byte IR_PIN = D4;

IRrecv irrecv(IR_PIN);

decode_results results;

unsigned long lastMillis;

// gun states
bool standby = false;
bool laser = false;
bool reloadLock = true;
bool calibrated = false;
bool consent = false;

//Laser
const byte LASER_PIN = D8;

//Motor Setup
const byte MOTOR_PIN = D10; //motor on TX pin

// function prototypes
void SendCodes();
bool CheckStandby();
bool CheckConsent();
char ReadIR();
void WifiConnect(const String &ssid, const String &password);
void StartWebSocket(WebSocketsServer &webSocket);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

void setup()
{
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
  //Laser and Motor Setup
  pinMode(LASER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  //IR Remote Setup
  irrecv.enableIRIn();
}

void loop()
{
  webSocket.loop();
  digitalWrite(LASER_PIN, laser);
  digitalWrite(MOTOR_PIN, consent);
  if (webSocket.connectedClients() > 0)
  {
    if (CheckStandby())
    {
      if (CheckConsent())
      {
        //Run Motors
      }
    }
  }
}

void SendCodes()
{
  for (int i = 0; i < webSocket.connectedClients(); i++)
  {
    String msg = "CODES," + codes.getCode(STANDBY_CODE) + "," + codes.getCode(CONSENT_CODE);
    Serial.println(msg);
    webSocket.sendTXT(i, msg);
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

bool CheckConsent()
{
  if (consent)
  {
    return true;
  }
  else
  {
    char readKey = ReadIR();
    if (readKey != NO_KEY)
    {
      Serial.println(readKey);
      if (codes.readCode(readKey, CONSENT_CODE))
      {
        consent = true;
        Serial.print("Consent");
        for (int i = 0; i < webSocket.connectedClients(); i++)
        {
          webSocket.sendTXT(i, "CONSENT");
        }
        return true;
      }
    }
    return false;
  }
}

char ReadIR()
{
  if (irrecv.decode(&results)) 
  {
    if (millis() - lastMillis < 500)
    {
      irrecv.resume();
      return NO_KEY;
    }
    serialPrintUint64(results.value, HEX);
    Serial.print(" ");
    lastMillis = millis();
    switch (results.value)
    {
    case HEX_POWER:
      irrecv.resume();  // Receive the next value
      return '#';
      break;
    case HEX_1:
      irrecv.resume();  // Receive the next value
      return '1';
      break;
    case HEX_2:
      irrecv.resume();  // Receive the next value
      return '2';
      break;
    case HEX_3:
      irrecv.resume();  // Receive the next value
      return '3';
      break;
    case HEX_4:
      irrecv.resume();  // Receive the next value
      return '4';
      break;
    case HEX_5:
      irrecv.resume();  // Receive the next value
      return '5';
      break;
    case HEX_6:
      irrecv.resume();  // Receive the next value
      return '6';
      break;
    case HEX_7:
      irrecv.resume();  // Receive the next value
      return '7';
      break;
    case HEX_8:
      irrecv.resume();  // Receive the next value
      return '8';
      break;
    case HEX_9:
      irrecv.resume();  // Receive the next value
      return '9';
      break;
    case HEX_0:
      irrecv.resume();  // Receive the next value
      return '0';
      break;
    default:
      irrecv.resume();  // Receive the next value
      return NO_KEY;
      break;
    }
  }
  else
  {
    return NO_KEY;
  }
}

void WifiConnect(const String &ssid, const String &password)
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(++i);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void StartWebSocket(WebSocketsServer &webSocketConn)
{
  webSocketConn.begin();
  webSocketConn.onEvent(webSocketEvent);
  Serial.println("WebSocket server started.");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    standby = false;
    laser = false;
    consent = false;
    break;
  case WStype_CONNECTED:
    Serial.println("Connection Succeful");
    SendCodes();
    break;
  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, payload);
    String webSocketMessage = (char *) payload;
    if (webSocketMessage == "LASERSIGHTON")
    {
      Serial.println("LASER TIME BRUHHH");
      laser = true;
    }
    else if (webSocketMessage == "CALIBRATE")
    {
      calibrated = true;
    }
    else if (webSocketMessage == "MAGLOCKOFF")
    {
      reloadLock = false;
    }
    else if (webSocketMessage == "MAGLOCKON")
    {
      reloadLock = true;
    }
    break;
  }
}