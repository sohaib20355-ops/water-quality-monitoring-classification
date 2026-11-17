#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 15

// WiFi credentials
const char *ssid = "YOUR_WIFI_SSID"; 
const char *pass = "YOUR_WIFI_PASSWORD";  
const char *server = "http://api.thingspeak.com/update";
String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";    // Enter your Write API key from ThingSpeak

// Setup OneWire instance for temperature sensor (DS18B20)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient client;
float tempC = 25.0; // Placeholder for temperature

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Start up the temperature sensor
  sensors.begin();
}

void loop() {
  // Request temperature from DS18B20 sensor
  sensors.requestTemperatures();  // Send the command to get temperatures
  tempC = sensors.getTempCByIndex(0); // Read temperature from the first sensor
  
  // Display temperature on Serial Monitor
  Serial.print("Temperature from DS18B20: ");
  Serial.println(tempC);

  // Send temperature data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + 
                 "&field2=" + String(tempC);  // Send temperature to Field 1

    // Print the URL for debugging
    Serial.print("URL: ");
    Serial.println(url);
    
    http.begin(url);
    int httpCode = http.GET();
    .
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

  delay(15000); 
}
