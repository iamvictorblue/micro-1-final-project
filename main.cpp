#include <Arduino.h>
#include <wifi.h>
#include <PubSubClient.h>

#define PIN_ANALOG_IN_TEMPERATURE 36
#define PIN_ANALOG_IN_INTERRUPTOR 32

// WIFI
#define WIFI_NAME "Ghost Friend"
#define WIFI_PASSWORD "gsYmqcwR3xhv"
#define WIFI_TIMEOUT 10000

// MQTT BROKER
const char *mqttServer = "3.228.111.161";
const char *celcius = "Temperature/Celcius";
const char *farenheit = "Temperature/Farenheit";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

int calibrationTime = 0;
bool recalibrated = false;
float recalibratedVoltage = 0;
float temperatureQuotient = 0;
float recalibratingVals[5];

float recalibrate(float values[5]);
float highestTemp(float values[5]);
float lowestTemp(float values[5]);
void connectToWifi();

void connectToWifi()
{

  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

  unsigned long startAttempTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttempTime < WIFI_TIMEOUT)
  {
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to connect to WiFi");
    return;
  }
  else
  {
    Serial.println("Connected to WiFi");
  }
}

void connectToBroker()
{
  client.setServer(mqttServer, mqttPort);

  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  connectToWifi();
  connectToBroker();
}

void loop()
{
  float adcValue = analogRead(PIN_ANALOG_IN_TEMPERATURE);
  float voltage = (float)adcValue / 4095.0 * 3.3;                 // read ADC pin and convert to voltage
  float Rt = 10 * voltage / (3.3 - voltage);                      // calculate resistance value of thermistor
  double tempK = 1 / (1 / (273.15 + 25) + log(Rt / 10) / 3950.0); // calculate temperature (Kelvin)
  double tempC = tempK - 273.15;                                         // calculate temperature (Celsius)
  double tempF = tempC * 9 / 5 + 32;
  double button = analogRead(PIN_ANALOG_IN_INTERRUPTOR);

  if (temperatureQuotient != 0)
  {
    tempC = tempC * temperatureQuotient;
  }

  if (button == 0 && calibrationTime < 5 && recalibrated == false)
  {
    Serial.println("Recalibrating...");
    recalibratingVals[calibrationTime] = tempC;
    calibrationTime++;
  }

  else if (button != 0 && calibrationTime >= 5)
  {
    temperatureQuotient = recalibrate(recalibratingVals);
    recalibrated = false;
    calibrationTime = 0;

    for (int i = 0; i < 5; i++)
    { // reset array
      recalibratingVals[i] = 0;
    }
    delay(500);
  }
  Serial.printf("Calibration Time: %d, \tTemperature in C: %.2fC, \tTemperature in F: %.2fF\n", calibrationTime, tempC, tempF);
  delay(1000);

  if (!client.connected())
  {
    connectToBroker();
  }
  client.publish(celcius, String(tempC).c_str());
  client.publish(farenheit, String(tempF).c_str());
  client.loop();
}

float recalibrate(float values[])
{
  float lowestTemperature = lowestTemp(values);
  float highestTemperature = highestTemp(values);
  float averageTemperature = (lowestTemperature + highestTemperature) / 2;

  // Linear interpolation to find the temperature of the thermistor
  float interTemp = lowestTemperature + ((5 / 2) - 1) * (highestTemperature - lowestTemperature) / (5 - 1);
  float temperatureQuotient = interTemp / averageTemperature;

  return temperatureQuotient;
}

float lowestTemp(float values[])
{
  float lowestTemperature = values[0];
  for (unsigned int i = 0; i < 5; i++)
  {
    if (values[i] < lowestTemperature)
    {
      lowestTemperature = values[i];
    }
  }
  return lowestTemperature;
}

float highestTemp(float values[])
{
  float highestTemperature = values[0];
  for (unsigned int i = 0; i < 5; i++)
  {
    if (values[i] > highestTemperature)
    {
      highestTemperature = values[i];
    }
  }
  return highestTemperature;
}