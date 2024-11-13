#include <NetworkClient.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <LiquidCrystal_I2C.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t  i32;

// https://docs.arduino.cc/built-in-examples/basics/ReadAnalogVoltage/
// https://www.arduino.cc/reference/tr/language/functions/analog-io/analogread/
// The max value that the 'analogRead' function can return
const u16 ANALOG_READ_MAX_VALUE = 1023;

#define DEBUG 1
#include "dht11.h"
#include "ph_sensor.h"
#include "pages.h"

const u32 BAUD_RATE = 115200;
const u8  PIN_NUMBER_LED_BUILTIN = 2;
const u8  PIN_NUMBER_PH_SENSOR = 35;
const u8  PIN_NUMBER_DHT11 = 26;
const u8  LCD_COLUMNS = 16;
const u8  LCD_ROWS = 2;

const String NETWORK_SSID = "Sensores Compostera";

LiquidCrystal_I2C lcd(39, LCD_COLUMNS, LCD_ROWS);
NetworkServer server(80);
u64 lastRecordedTime = millis();

void setup()
{
  pinMode(PIN_NUMBER_PH_SENSOR, INPUT);
  pinMode(PIN_NUMBER_LED_BUILTIN, OUTPUT);
  Serial.begin(BAUD_RATE);
  lcd.init();
  lcd.backlight();

  Serial.println("Attempting to create Access Point.");

  u8 attempts = 3;
  do {
    attempts -= 1;
    if (attempts == 0) {
      log_e("Access Point creation failed.\n");
      while (true) { }
    }

  // SSID, password, channel, hide SSID
  } while (!WiFi.softAP(NETWORK_SSID, emptyString, 1, false));

  Serial.println("Access Point creation succeeded.");
  server.begin();
}

void loop()
{
  float humidity, ph = -1, temperature;
  u8 resultValue = readTemperatureHumidity(PIN_NUMBER_DHT11, temperature, humidity);

  u64 currentTime = millis();
  if (lastRecordedTime - currentTime >= 5000) {
    ph = read_ph(PIN_NUMBER_PH_SENSOR);
    lastRecordedTime = currentTime;

    Serial.printf("DHT11 read result code: %hhu\t", resultValue);
    Serial.printf("Temperature: %.2f\t", temperature);
    Serial.printf("Humidity: %.2f\n\n", humidity);

    lcd.clear();
    lcd.setCursor(0,0);

    int dataForLcd = static_cast<int>(humidity);
    lcd.print("Hum: ");
    lcd.print(dataForLcd);

    lcd.print(" pH: ");
    lcd.print(ph);

    lcd.setCursor(0,1);

    dataForLcd = static_cast<int>(temperature);
    lcd.print("Temp: ");
    lcd.print(dataForLcd);
    lcd.print("°C");
  }

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
          if (ph == -1) {
            ph = read_ph(PIN_NUMBER_PH_SENSOR);
          }

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
