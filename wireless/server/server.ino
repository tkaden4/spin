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

static const char *id_file_name = "id.txt";
static const char *data_file_name = "data.csv";
static const char *log_file_name = "log.txt";

///////////// GLOBALS ////////////////

// ERRORS
static client_error errors[N_ERRORS] = {NO_ERROR};
static unsigned int currentErrors = 0;

// DATA
static int countedRotations = 0;

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
  for(int i = 0; i < N_ERRORS; ++i) {
    if(errors[i] == error) {
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

static File check_file;


int initialize_sd()
{
  id_file.close();
  data_file.close();
  check_file.close();
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
  if(result) {
    *file = result;
    return 1;
  } else {
    return 0;
  }
}

int write_data(String data)
{
  if(!open_file(data_file_name, &data_file, true)) {
    Serial.println("couldnt open file");
    return 0;
  }
  String result = data + "\n";
  if(!data_file.write(result.c_str(), result.length())) {
    Serial.println("couldnt write to file");
    return 0;
  }
  return 1;
}

static File data_file_read;

int read_data()
{
  if(!open_file(data_file_name, &data_file_read, false))
  {
    return 0;
  }
  int i = 0;
  while(data_file_read.available()){
    data_file_read.readStringUntil('\n');
    ++i;
  }
  Serial.printf("%d\n", i);
  data_file_read.close();
  return 0;
}

/////////// Web Utilities //////////////

void handleRoot()
{
  server.send(200, "text/html", "Mouse Endpoint Available<br>Counted Rotations: " + String(countedRotations));
}

void handleLog()
{
  status(HIGH);
  Serial.println("handling log");
  String logData = server.arg("log");
  String wheel = server.arg("wheel");
  Serial.printf("%s: %s\r\n", wheel, logData);
  server.send(200, "text/html", "");
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
  server.send(200, "text/html", "");
  status(LOW);
}

/////////////// Arduino Code ///////////////////

int sd_card_error = 0;

void blink(int times, int d, int after=0) {
  while(times-- > 0) {
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
    blink
  }

  // Set up SD Card
  sd_card_error = !initialize_sd();

  // Set up routes
  server.on("/", handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/log", HTTP_GET, handleLog);
  server.begin();

  Serial.println("setup finished");
}

void loop()
{
  server.handleClient();
  if(sd_card_error) {
    blink(6, 100, 500);
    Serial.println("SD Card Error - retrying");
    sd_card_error = !initialize_sd();
  } else {
    sd_card_error = !write_data("baz");
    data_file.flush();
    data_file.close();
    read_data();
    delay(500);
  }
}
