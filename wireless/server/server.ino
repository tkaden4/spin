/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "WWUMouseWheel"
#define APPSK  "MusMusculus360"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

// Web server endpoints

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "Mouse Endpoint Available");
}

void handleLogData() {
  String postBody = server.arg("plain");
  Serial.printf("%s\n", postBody);
  server.send(200, "text/html", "posted contents");
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  while(!Serial.available()) {
    delay(100);
  }
  server.on("/", handleRoot);
  server.on("/data", HTTP_POST, handleLogData);
  server.begin();
}

void loop() {
  while(!WiFi.softAP(ssid, password)) {
    delay(100);
  }
  server.handleClient();
}
