#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define INPUT_PIN D6



#ifndef APSSID
#define APSSID "WWUMouseWheel"
#define APPSK "MusMusculus360"
#define APIP "192.168.4.1"
#define SERVER_PORT 80
#define SERVER_PATH "/data"
#endif

static int count = 0;

void ICACHE_RAM_ATTR onTurn()
{
  ++count;
}

void status(uint8_t state)
{
  digitalWrite(LED_BUILTIN, !state);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), onTurn, CHANGE);
  WiFi.begin(APSSID, APPSK);
  Serial.println("connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
    status(HIGH);
    delay(250);
    status(LOW);
  }
  Serial.println("connected");
  status(LOW);
}

void sendCount(int changeCount) {
  WiFiClient wifi;
  HTTPClient http;
  http.begin(wifi, String("http://") + APIP + ":" + String(SERVER_PORT) + String(SERVER_PATH) + "?count=" + changeCount);
  int httpCode = http.GET();
  http.end();
  Serial.printf("HTTP response: %d\n", httpCode);
}

void loop()
{
  delay(500);
  noInterrupts();
  int savedCount = count;
  count = 0;
  interrupts();
  sendCount(savedCount);
  status(savedCount);
}
