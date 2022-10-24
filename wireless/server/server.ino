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
  Serial.printf("{ \"rotations\": %d, \"wheel\": \"%s\" }\n", data, wheel);
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
  Serial.println("handling data");
  String logData = server.arg("log");
  String wheel = server.arg("wheel");
  int count = countParameter.toInt();
  Serial.printf("%s: %s\n", wheel, logData);
  server.send(200, "text/html", "");
  status(LOW);
}

void handleLogData() {
  status(HIGH);
  Serial.println("handling data");
  String countParameter = server.arg("count");
  int count = countParameter.toInt();
  String wheel = server.arg("wheel");
  countedRotations += count;
  data(count);
  server.send(200, "text/html", "");
  status(LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setup starting");
  
  pinMode(LED_BUILTIN, OUTPUT);
    
  // Set up routes
  server.on("/", handleRoot);
  server.on("/data", HTTP_GET, handleLogData);
  server.on("/log", HTTP_GET, handleLog);
  server.begin();

  Serial.println("setup finished");
}

void loop() {
  while(!WiFi.softAP(ssid, password)) {
    Serial.println("setting up wifi...");
    status(HIGH);
    delay(100);
    status(LOW);
    delay(100);
  }
  server.handleClient();
}
