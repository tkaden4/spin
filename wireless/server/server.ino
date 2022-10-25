/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "WWUMouseWheel"
#define APPSK  "MusMusculus360"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

static int countedRotations = 0;

void data(int data, String wheel) {
  Serial.printf("{ \"rotations\": %d, \"wheel\": \"%s\" }\r\n", data, wheel);
}

void status(int s) {
  digitalWrite(LED_BUILTIN, !s);
} 

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "Mouse Endpoint Available<br\>Counted Rotations: " + String(countedRotations));
}

void handleLog() {
  status(HIGH);
  Serial.println("handling log");
  String logData = server.arg("log");
  String wheel = server.arg("wheel");
  Serial.printf("%s: %s\r\n", wheel, logData);
  server.send(200, "text/html", "");
  status(LOW);
}

void handleData() {
  status(HIGH);
  Serial.println("handling data");
  String countParameter = server.arg("count");
  int count = countParameter.toInt();
  String wheel = server.arg("wheel");
  countedRotations += count;
  data(count, wheel);
  server.send(200, "text/html", "");
  status(LOW);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Setup starting");
  pinMode(LED_BUILTIN, OUTPUT);

  while(!WiFi.softAP(ssid, password)) {
    Serial.println("setting up wifi...");
    status(HIGH);
    delay(250);
    status(LOW);
    delay(250);
  }
    
  // Set up routes
  server.on("/", handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/log", HTTP_GET, handleLog);
  server.begin();

  Serial.println("setup finished");
}

void loop() {
  server.handleClient();
}
