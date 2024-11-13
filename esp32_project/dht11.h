#include <Arduino.h>

static const u16 DELAY_MS = 500;
static const u8  ERROR_CHECKSUM = 254;
static const u8  ERROR_TIMEOUT = 253;
static const u16 TIMEOUT_DURATION = 1000;

static void startSignal(u8);
static u8 readByte(u8);

/**
 * @param data: An array of bytes where the raw sensor data will be stored.
 *              The array must be at least 5 bytes long, as the DHT11 sensor returns 5 bytes of data.
 * @return: Returns 0 if the data is read successfully and the checksum matches.
 *          Returns DHT11::ERROR_TIMEOUT if the sensor does not respond or communication times out.
 *          Returns DHT11::ERROR_CHECKSUM if the data is read but the checksum does not match.
 */
u8 readRawData(u8 pinNumber, u8 data[5])
{
  delay(DELAY_MS);
  startSignal(pinNumber);
  u64 timeout_start = millis();

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
      for (u8 i = 0; i < 5; i++)
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

static u8 readByte(u8 pinNumber)
{
  u8 value = 0;

  for (u8 i = 0; i < 8; i++)
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
static void startSignal(u8 pinNumber)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, LOW);
  delay(18);
  digitalWrite(pinNumber, HIGH);
  delayMicroseconds(40);
  pinMode(pinNumber, INPUT);
}

/**
 * @return: An integer representing the status of the read operation.
 *          Returns 0 if the reading is successful, DHT11::ERROR_TIMEOUT if a timeout occurs,
 *          or DHT11::ERROR_CHECKSUM if a checksum error occurs.
 */
u8 readTemperatureHumidity(u8 pinNumber, float &temperature, float &humidity)
{
  const float decimal_part_scale = 256;

  u8 data[5];
  u8 error = readRawData(pinNumber, data);
  if (error != 0)
  {
    return error;
  }
  humidity = data[0] + (data[1] / decimal_part_scale);
  temperature = data[2] + (data[3] / decimal_part_scale);
  return 0;
}