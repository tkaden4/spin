/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SD.h>
#include <SPI.h>

////////// CONSTANTS ////////////

#ifndef APSSID
#define APSSID "WWUMouseWheel"
#define APPSK "MusMusculus360"
static const char *ssid = APSSID;
static const char *password = APPSK;
#endif

typedef enum client_error_e
{
  NO_ERROR,
  SD_START_ERROR,
  N_ERRORS
} client_error;

static const char *data_file_name = "data.csv";

///////////// GLOBALS ////////////////

// ERRORS
static client_error errors[N_ERRORS] = {NO_ERROR};
static unsigned int currentErrors = 0;

// DATA
static int countedRotations = 0;
static String currentTime = "UNKNOWN";

// WEB
static ESP8266WebServer server(80);

//////////// Error Utilities ////////////////

void set_error(client_error error)
{
  if (currentErrors + 1 == N_ERRORS)
  {
    return;
  }
  for (int i = 0; i < N_ERRORS; ++i)
  {
    if (errors[i] == NO_ERROR)
    {
      errors[i] = error;
      ++currentErrors;
      return;
    }
    if (errors[i] == error)
    {
      return;
    }
  }
}

void clear_error(client_error error)
{
  for (int i = 0; i < N_ERRORS; ++i)
  {
    if (errors[i] == error)
    {
      errors[i] = NO_ERROR;
      currentErrors--;
    }
  }
}

bool has_error(client_error error)
{
  for (int i = 0; i < N_ERRORS; ++i)
  {
    if (errors[i] == error)
    {
      return true;
    }
  }
  return false;
}

/////////// General Utilities /////////////////////

void status(int s)
{
  digitalWrite(LED_BUILTIN, !s);
}

//////////// SD Card Utilities ///////////////

static File id_file;
static File data_file;
static File data_file_read;

int initialize_sd()
{
  id_file.close();
  data_file.close();
  data_file_read.close();
  SD.end();
  if (!SD.begin(SS))
  {
    return 0;
  }
  return 1;
}

int open_file(String name, File *file, bool write)
{
  File result = SD.open(name, write ? FILE_WRITE : FILE_READ);
  if (result)
  {
    *file = result;
    return 1;
  }
  else
  {
    return 0;
  }
}

int write_data(String data)
{
  if (!open_file(data_file_name, &data_file, true))
  {
    Serial.println("couldnt open file");
    return 0;
  }
  String result = data + "\n";
  if (!data_file.write(result.c_str(), result.length()))
  {
    Serial.println("couldnt write to file");
    return 0;
  }
  return 1;
}

/////////// Web Utilities //////////////

int sd_card_error = 0;

void handleRoot()
{
  server.send(200, "text/html", "Mouse Endpoint Available<br>Counted Rotations: " + String(countedRotations));
}

void handleTime()
{
  status(HIGH);
  Serial.println("Handling new time update");
  String newTimeParameter = server.arg("time");
  if (newtimeParameter.length())
  {
    currentTime = newTimeParameter;
  }
  server.send(200, "text/plain", "set time");
  status(LOW);
}

void handleReset()
{
  status(HIGH);
  Serial.println("Handling reset") if (SD.exists(data_file_name))
  {
    if (!SD.remove(data_file_name))
    {
      server.send(500, "text/plain", String("error removing ") + String(data_file_name));
    }
    else
    {
      server.send(200, "text/plain", "reset data");
    }
  }
  else
  {
    server.send(404, "text/plain", String(data_file_name) + " does not exist.");
  }
  status(LOW);
}

void handleGetData()
{
  status(HIGH);
  Serial.println("Handling get data");

  sd_card_error = !initialize_sd();
  if (!open_file(data_file_name, &data_file_read, false))
  {
    server.send(500, "text/plain", "Internal SD Card error");
    return;
  }

  server.chunkedResponseModeStart(200, "text/csv");
  while (data_file_read.available())
  {
    server.sendContent(data_file_read.readStringUntil('\n') + "\n");
  }
  data_file_read.close();
  server.chunkedResponseFinalize();
  status(LOW);
}

void handleData()
{
  status(HIGH);
  Serial.println("handling data");
  String countParameter = server.arg("count");
  int count = countParameter.toInt();
  String wheel = server.arg("wheel");
  countedRotations += count;
  write_data(wheel + ", " + count);
  Serial.printf("%s, %s, %d, %d\n", currentTime, wheel, count, countedRotations);
  server.send(200, "text/html", "");
  status(LOW);
}

/////////////// Arduino Code ///////////////////

void blink(int times, int d, int after = 0)
{
  while (times-- > 0)
  {
    status(HIGH);
    delay(d);
    status(LOW);
    delay(d);
  }
  delay(after);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Setup starting");
  pinMode(LED_BUILTIN, OUTPUT);

  while (!WiFi.softAP(ssid, password))
  {
    Serial.println("setting up wifi...");
    blink(2, 500);
  }

  // Set up SD Card
  Serial.println("setting up SD card...");
  sd_card_error = !initialize_sd();

  // Set up routes
  server.on("/", handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/file", HTTP_GET, handleGetData);
  server.on("/reset", HTTP_GET, handleReset);
  server.on("/setTime", HTTP_GET, handleTime);
  server.begin();

  Serial.println("setup finished");
}

void loop()
{
  server.handleClient();
  if (sd_card_error)
  {
    blink(3, 100, 500);
    Serial.println("SD Card Error - retrying");
    sd_card_error = !initialize_sd();
  }
}
