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

#define BUFFER_SIZE (1024 * 4)

const uint8_t **currentBuffer;
const uint8_t **swapBuffer;

const uint8_t CURRENT_BUFFER[BUFFER_SIZE];
const uint8_t SWAP_BUFFER[BUFFER_SIZE];

// Web server endpoints

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleLogData() {

}

void handleSeeData() {

}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
