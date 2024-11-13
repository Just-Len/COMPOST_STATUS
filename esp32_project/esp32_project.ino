#include <NetworkClient.h>
#include <WiFi.h>
#include <WiFiAP.h>

// https://docs.arduino.cc/built-in-examples/basics/ReadAnalogVoltage/
// https://www.arduino.cc/reference/tr/language/functions/analog-io/analogread/
// The max value that the 'analogRead' function can return
const int ANALOG_READ_MAX_VALUE = 1023;

#define DEBUG 1
#include "dht11.h"
#include "ph_sensor.h"
#include "pages.h"

const int BAUD_RATE = 115200;
const int PIN_NUMBER_LED_BUILTIN = 2;
const int PIN_NUMBER_PH_SENSOR = 26;
const int PIN_NUMBER_DHT11 = 25;

const String NETWORK_SSID = "Sensores Compostera";

NetworkServer server(80);

void setup()
{
  pinMode(PIN_NUMBER_PH_SENSOR, INPUT);
  pinMode(PIN_NUMBER_LED_BUILTIN, OUTPUT);
  Serial.begin(BAUD_RATE);

  Serial.println("Attempting to create Access Point.");

  int attempts = 3;
  do {
    attempts -= 1;
    if (attempts == 0) {
      log_e("Access Point creation failed.\n");
      while (true) { }
    }

  } while (!WiFi.softAP(NETWORK_SSID, emptyString, true, true, 2));

  Serial.println("Access Point creation succeeded.");
}

void loop()
{
  int humidity, temperature;
  float ph = read_ph(PIN_NUMBER_PH_SENSOR);
  int resultValue = readTemperatureHumidity(PIN_NUMBER_DHT11, temperature, humidity);
  
  NetworkClient client = server.accept();

  if (client) {
    Serial.println("New client.");
    bool ignore = false, wasNewLine;
    String firstLine = "";

    while (client.connected() && client.available()) {
      char c = client.read();
      Serial.write(c);
      if (c == '\n') {
        if (wasNewLine) {
          break;
        }

        ignore = true;
        wasNewLine = true;
      } else if (c != '\r' && !ignore) {
        firstLine += c;
        wasNewLine = false;
      }
    }

    if (client.connected() && client.available()) {
      if (firstLine.startsWith("GET /measurements")) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/json");
          client.println();

          client.print(R"({"humidity":)");
          client.print(humidity);
          client.print(R"(,"pH":)");
          client.print(ph);
          client.print(R"(,"temperature":)");
          client.print(temperature);
          client.print("}");

          client.println();
      }
      else if (firstLine.startsWith("GET /")) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();

        client.print(index_page);
        client.println();
      }
    }

    client.stop();
    Serial.println("Client disconnected.");
  }
}
