#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
const char *ssid = "YOUR_WIFI_SSID";  
const char *pass = "YOUR_WIFI_PASSWORD";    
const char *server = "http://api.thingspeak.com/update";
String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";  

// Temperature Sensor (DS18B20)
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float tempC = 25.0; // Placeholder for temperature

// pH Sensor
const int potPin = A0;
float pH;

// TDS and Turbidity Sensors
#define TdsSensorPin 32
#define TurbiditySensorPin 35

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Initialize sensors
  sensors.begin();  // Temperature sensor
  pinMode(TdsSensorPin, INPUT);
  pinMode(TurbiditySensorPin, INPUT);
}

float readTemperature() {
  sensors.requestTemperatures();  // Request temperature from DS18B20
  tempC = sensors.getTempCByIndex(0); // Get temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  return tempC;
}

float readpH() {
  float sensorValue = analogRead(potPin);
  float voltage = sensorValue * (3.3 / 4095.0); // Convert ADC value to voltage
  pH = 3.3 * voltage; // Adjust this formula based on calibration
  Serial.print("pH Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | pH Value: ");
  Serial.println(pH);
  return pH;
}

float readTDS(float temperature) {
  float tdsVoltage = analogRead(TdsSensorPin) * (3.3 / 4095.0); // Convert raw ADC to voltage
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); // Use measured temperature
  float compensatedVoltage = tdsVoltage / compensationCoefficient;
  float tdsValue = (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage
                    - 255.86 * compensatedVoltage * compensatedVoltage
                    + 857.39 * compensatedVoltage) * 0.5;
  Serial.print("TDS Voltage: ");
  Serial.print(tdsVoltage, 2);
  Serial.print(" V | TDS: ");
  Serial.print(tdsValue, 2);
  Serial.println(" ppm");
  return tdsValue;
}

// Updated turbidity reading function using Vclear and averaging
float readTurbidity() {
  int sensor = 0; // variable for averaging
  int n = 25; // number of samples to average
  int sensorValue = 0;
  float voltage = 0.00;
  float turbidity = 0.00;
  float Vclear = 1.716; // Fixed Vclear value (calibrated value)

  // Sensing routine:
  for (int i = 0; i < n; i++) {
    sensor += analogRead(TurbiditySensorPin); // Read analog value from turbidity sensor
    delay(10); // Small delay to stabilize the readings
  }
  sensorValue = sensor / n; // Average the sensor values
  voltage = sensorValue * (3.3 / 4095.0); // Convert raw ADC value to voltage (3.3V scale)

  // Debugging output for voltage and Vclear
  Serial.print("Calculated voltage: ");
  Serial.println(voltage, 3);
  Serial.print("Vclear: ");
  Serial.println(Vclear, 3);

  // Protect against invalid Vclear
  if (Vclear <= 0.0 || Vclear > 3.3) {
    Serial.println("Error: Invalid Vclear value.");
    turbidity = 0; // Set turbidity to 0 if Vclear is invalid
  } else {
    // Check if voltage exceeds Vclear
    if (voltage > Vclear) {
      Serial.println("Error: Voltage exceeds Vclear.");
      turbidity = 0; // Set turbidity to 0 or handle error case
    } else {
      // Calculate turbidity using fixed Vclear
      turbidity = 20.00 * (100.00 - (voltage / Vclear) * 100.00);
    }
  }

  // Serial display of voltage and turbidity
  Serial.print("Raw voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V, Turbidity: ");
  Serial.print(turbidity, 3);
  Serial.println(" NTU");

  return turbidity;
}

void sendDataToThingSpeak(float temperature, float pHValue, float tdsValue, float turbValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(tdsValue) +
                 "&field2=" + String(temperature) +
                 "&field3=" + String(pHValue) +
                 "&field4=" + String(turbValue);

    // Print URL for debugging
    Serial.print("URL: ");
    Serial.println(url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
      String response = http.getString();
      Serial.print("Response from ThingSpeak: ");
      Serial.println(response);

      if (httpCode == 200) {
        Serial.println("Data sent to ThingSpeak successfully.");
      } else {
        Serial.println("Error: Data not accepted by ThingSpeak.");
      }
    } else {
      Serial.print("Failed to send data. HTTP error code: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Unable to send data.");
  }
}

void loop() {
  // Read all sensor values
  float temperature = readTemperature();
  float pHValue = readpH();
  float tdsValue = readTDS(temperature);
  float turbValue = readTurbidity();

  // Send data to ThingSpeak
  sendDataToThingSpeak(temperature, pHValue, tdsValue, turbValue);

  delay(15000); // Send data every 15 seconds
}
