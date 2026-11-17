#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_ADS1X15.h>

float voltage, tdsValue, temperature = 25; // Constant temperature of 25°C

String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";    // Enter your Write API key from ThingSpeak
const char *ssid = "YOUR_WIFI_SSID";   // Replace with your WiFi SSID
const char *pass = "YOUR_WIFI_PASSWORD";   // Replace with your WiFi Password
const char* server = "http://api.thingspeak.com/update";

WiFiClient client;

#define TdsSensorPin 32  // Define the TDS sensor pin (using GPIO 32 as an example)

void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT);

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  // Read TDS sensor
  voltage = analogRead(TdsSensorPin); // Read from GPIO 32
  voltage = (voltage / 4096.0) * 3.3; // Convert ADC reading to voltage (assuming 12-bit ADC)

  // Temperature compensation and TDS calculation
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensatedVoltage = voltage / compensationCoefficient;

  // Calculate TDS value
  tdsValue = (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage
             - 255.86 * compensatedVoltage * compensatedVoltage
             + 857.39 * compensatedVoltage) * 0.5;

  // Display temperature and TDS on Serial Monitor
  Serial.print("Temperature (constant): ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("TDS: ");
  Serial.print(tdsValue, 2);
  Serial.println(" ppm");

  // Send data to ThingSpeak
  if (tdsValue > 0) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(tdsValue);
      
      // Print the URL for debugging
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
          Serial.println("Data sent to ThingSpeak successfully");
        } else {
          Serial.println("Error: Data not accepted by ThingSpeak");
        }
      } else {
        Serial.print("Failed to send data. HTTP error code: ");
        Serial.println(httpCode);
      }
      http.end();
    } else {
      Serial.println("WiFi not connected. Unable to send data.");
    }
  } else {
    Serial.println("Invalid TDS Value. Skipping upload.");
  }

  delay(15000); // ThingSpeak requires a minimum delay of 15 seconds between updates
}
