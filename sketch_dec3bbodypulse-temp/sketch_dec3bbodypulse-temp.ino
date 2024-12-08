#include <PulseSensorPlayground.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Pulse sensor pin
#define PULSE_SENSOR_PIN A0

// Temperature sensor setup (DS18B20)
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

// DHT11 setup
#define DHTPIN 3      // Pin connected to DHT11 data pin
#define DHTTYPE DHT11 // Sensor type DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pulse sensor instance
PulseSensorPlayground pulseSensor;

// Define body temperature limits
float lowerBodyTempLimit = 35.0; // Minimum body temperature in °C (hypothermia)
float upperBodyTempLimit = 38.5; // Maximum body temperature in °C (fever)

// Define suitable pulse rate limits
int lowerPulseLimit = 60;  // Minimum suitable pulse rate in BPM
int upperPulseLimit = 100; // Maximum suitable pulse rate in BPM

// Define DHT11 temperature limits
float lowerDHTTempLimit = 20.0; // Minimum DHT11 temperature in °C
float upperDHTTempLimit = 45.0; // Maximum DHT11 temperature in °C

void setup() {
  // Start serial communication
  Serial.begin(9600);
  Serial.println("Pulse Sensor, DS18B20, and DHT11 Sensor Test");

  // Initialize the pulse sensor
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.begin();
  pulseSensor.setThreshold(550); // Adjust the threshold if necessary

  // Initialize the temperature sensor (DS18B20)
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) {
    Serial.println("Unable to find address for Temperature Sensor");
  }
  sensors.setResolution(insideThermometer, 9);

  // Initialize the DHT11 sensor
  dht.begin();
}

void loop() {
  // Read and print the raw value from the pulse sensor
  int pulseRawValue = analogRead(PULSE_SENSOR_PIN);
  Serial.print("\nRaw Pulse Value: ");
  Serial.println(pulseRawValue);

  // Get the pulse rate (BPM) from the pulse sensor
  int pulseRate = pulseSensor.getBeatsPerMinute();
  if (pulseRate > 0) {
    Serial.print("Pulse Rate (BPM): ");
    Serial.println(pulseRate);

    // Check if pulse rate is within normal limits
    if (pulseRate < lowerPulseLimit) {
      Serial.println("ALERT: Pulse rate is TOO LOW! .");
      Serial.println("Action: You may want to consult a healthcare professional.");
    } else if (pulseRate > upperPulseLimit) {
      Serial.println("ALERT: Pulse rate is TOO HIGH! .");
      Serial.println("Action: You should consider consulting a doctor immediately.");
    } else {
      Serial.println("Pulse rate is ok.");
    }
  } else {
    Serial.println("No pulse detected");
  }

  // Request temperature from the DS18B20 sensor
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(insideThermometer);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
  } else {
    Serial.print("\nDS18B20 Temperature (C): ");
    Serial.print(tempC);
    Serial.print(" Temperature (F): ");
    Serial.println(DallasTemperature::toFahrenheit(tempC));

    // Check if the body temperature is out of the acceptable range
    if (tempC < lowerBodyTempLimit) {
      Serial.println("ALERT: Body temperature is TOO LOW! .");
      Serial.println("Action: Seek immediate medical assistance.");
    } else if (tempC > upperBodyTempLimit) {
      Serial.println("ALERT: Body temperature is TOO HIGH! .");
      Serial.println("Action: You should consult a doctor immediately.");
    } else {
      Serial.println("Body temperature is normal.");
    }
  }

  // Read temperature and humidity from DHT11 sensor
  float dhtTemp = dht.readTemperature();
  float dhtHumidity = dht.readHumidity();

  if (isnan(dhtTemp) || isnan(dhtHumidity)) {
    Serial.println("\nError: Could not read from DHT11 sensor");
  } else {
    Serial.print("\nDHT11 Temperature (C): ");
    Serial.print(dhtTemp);
    Serial.print(" Humidity (%): ");
    Serial.println(dhtHumidity);

    // Check if DHT11 temperature is out of the acceptable range
    if (dhtTemp > upperDHTTempLimit) {
      Serial.println("ALERT:  Temperature is TOO HIGH! Not suitable for survival.");
    } else if (dhtTemp < lowerDHTTempLimit) {
      Serial.println("ALERT: Temperature is TOO LOW! Not suitable for survival.");
    } else {
      Serial.println("Temperature is suitable for survival.");
    }
  }

  // Additional message for staying safe and healthy
  Serial.println("Stay safe, Stay Healthy");
  Serial.println("----------------------------------------------------------------------------------------------------------------------------");

  // Wait a moment before the next loop
  delay(2000);
}
