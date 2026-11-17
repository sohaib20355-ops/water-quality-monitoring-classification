#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Analog pin for pH sensor
const int potPin = A0;

// WiFi credentials
const char *ssid = "YOUR_WIFI_SSID";  // Replace with your WiFi SSID
const char *pass = "YOUR_WIFI_PASSWORD";    // Replace with your WiFi Password
const char *server = "http://api.thingspeak.com/update";
String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";   // Enter your Write API key from ThingSpeak

float pH;
float sensorValue = 0;

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
}

void loop() {
  // Read the analog value from the pH sensor
  sensorValue = analogRead(potPin);
  float voltage = sensorValue * (3.3 / 4095.0); // Convert ADC value to voltage (ESP32 ADC is 12-bit)
  pH = 3.3 * voltage; // Custom conversion logic for your pH sensor

  // Print pH value to the Serial Monitor
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V | pH Value: ");
  Serial.println(pH);

  // Send pH data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + "&field3=" + String(pH);

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
        Serial.println("pH data sent to ThingSpeak successfully.");
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

  delay(15000); // ThingSpeak requires a minimum delay of 15 seconds between updates
}
