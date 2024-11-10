#include <Arduino.h>

static const int DELAY_MS = 500;
static const int ERROR_CHECKSUM = 254;
static const int ERROR_TIMEOUT = 253;
static const int TIMEOUT_DURATION = 1000;

static void startSignal(int);
static byte readByte(int);

/**
 * @param data: An array of bytes where the raw sensor data will be stored.
 *              The array must be at least 5 bytes long, as the DHT11 sensor returns 5 bytes of data.
 * @return: Returns 0 if the data is read successfully and the checksum matches.
 *          Returns DHT11::ERROR_TIMEOUT if the sensor does not respond or communication times out.
 *          Returns DHT11::ERROR_CHECKSUM if the data is read but the checksum does not match.
 */
int readRawData(int pinNumber, byte data[5])
{
  delay(DELAY_MS);
  startSignal(pinNumber);
  unsigned long timeout_start = millis();

  while (digitalRead(pinNumber) == HIGH)
  {
    if (millis() - timeout_start > TIMEOUT_DURATION)
    {
      return ERROR_TIMEOUT;
    }
  }

  if (digitalRead(pinNumber) == LOW)
  {
    delayMicroseconds(80);
    if (digitalRead(pinNumber) == HIGH)
    {
      delayMicroseconds(80);
      for (int i = 0; i < 5; i++)
      {
        data[i] = readByte(pinNumber);
        if (data[i] == ERROR_TIMEOUT)
        {
          return ERROR_TIMEOUT;
        }
      }

      if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
      {
        return 0;
      }
      else
      {
        return ERROR_CHECKSUM;
      }
    }
  }
  return ERROR_TIMEOUT;
}

static byte readByte(int pinNumber)
{
  byte value = 0;

  for (int i = 0; i < 8; i++)
  {
    while (digitalRead(pinNumber) == LOW);

    delayMicroseconds(30);
    if (digitalRead(pinNumber) == HIGH)
    {
      value |= (1 << (7 - i));
    }

    while (digitalRead(pinNumber) == HIGH);
  }
  return value;
}

/**
 * Sends a start signal to the DHT11 sensor to initiate a data read.
 * This involves setting the data pin low for a specific duration, then high,
 * and finally setting it to input mode to read the data.
 */
static void startSignal(int pinNumber)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, LOW);
  delay(18);
  digitalWrite(pinNumber, HIGH);
  delayMicroseconds(40);
  pinMode(pinNumber, INPUT);
}

/**
 * @param temperature: Reference to a variable where the temperature value will be stored.
 * @param humidity: Reference to a variable where the humidity value will be stored.
 * @return: An integer representing the status of the read operation.
 *          Returns 0 if the reading is successful, DHT11::ERROR_TIMEOUT if a timeout occurs,
 *          or DHT11::ERROR_CHECKSUM if a checksum error occurs.
 */
int readTemperatureHumidity(int pinNumber, int &temperature, int &humidity)
{
  byte data[5];
  int error = readRawData(pinNumber, data);
  if (error != 0)
  {
    return error;
  }
  humidity = data[0];
  temperature = data[2];
  return 0;
}