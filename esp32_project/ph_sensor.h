// El valor de voltaje tiene una correspondencia directa con un valor
// de pH, lo que forma una recta, por lo que se usa la ecuación
// estándar de una recta 'y = mx + b' para calcular el pH 'y' a partir
// del voltaje 'x', y se usa el valor 'm' obtenido de la calibración
// para ajustar la pendiente de la recta.

// TODO: Calcular con la fórmula "(7 - pH4) / (2.5 - V4)"
// pH4 = pH reportado del buffer con pH 4
// V4 = Voltaje reportado del buffer con pH 4
// Ambos valores deben obtenerse despues de haber ajustado
// manualmente la perilla o potenciometro con el buffer de pH 7
// hasta que el voltaje fuese de 2.5
static const float LINE_SLOPE = 1;

static const u8  MEASURE_COUNT = 10;
static const u8  MEASURE_INTERVAL_MS = 20;
static const u16 LOOP_INTERVAL_MS = 1000;

static const float INPUT_VOLTAGE = 5;
static const float MEDIUM_VOLTAGE = 2.5;
static const u8    NEUTRAL_PH = 7;

float to_voltage(u32 value);
float to_ph(float voltage);

float read_ph(u8 pinNumber)
{
  u32 sumValue = 0;
  for(u8 i = 0; i < MEASURE_COUNT; i++) {
    sumValue += analogRead(pinNumber);
    delay(MEASURE_INTERVAL_MS);
  }
 
  float average = sumValue / MEASURE_COUNT;

  float voltage = to_voltage(average);
  float ph = to_ph(voltage);

#if DEBUG
  Serial.printf("PH sensor raw output value: %d\t", sumValue);
  Serial.printf("Voltage: %f\t", voltage);
  Serial.printf("pH: %f\n", ph);
#endif

  return ph;
}

float to_voltage(u32 value)
{
  return value * (INPUT_VOLTAGE / ANALOG_READ_MAX_VALUE);
}

float to_ph(float voltage)
{
  return NEUTRAL_PH - (MEDIUM_VOLTAGE - voltage) * LINE_SLOPE;
}
