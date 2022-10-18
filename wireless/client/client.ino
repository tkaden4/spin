#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define INPUT_PIN D6

static bool state = false;

void ICACHE_RAM_ATTR onTurn() {
  state = !state;
}


void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), onTurn, CHANGE);
}


void loop() {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
}
