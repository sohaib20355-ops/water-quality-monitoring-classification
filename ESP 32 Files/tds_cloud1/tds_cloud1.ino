#include <WiFi.h>
#include <HTTPClient.h>

#define TdsSensorPin 27       // TDS sensor connected to GPIO 27
#define VREF 3.3              // Analog reference voltage (Volts) of the ADC
#define SCOUNT  30            // Number of sample points

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
const char* server = "http://api.thingspeak.com/update";

int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (int i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT);
  analogSetWidth(12);                // Set ADC resolution to 12 bits
  analogSetAttenuation(ADC_11db);    // Handle input voltage up to ~3.3V

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {
    analogSampleTimepoint = millis();
    int rawValue = analogRead(TdsSensorPin);
    analogBuffer[analogBufferIndex] = rawValue;
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }

    Serial.print("Raw ADC Value: ");
    Serial.println(rawValue);
  }

  static unsigned long uploadTimepoint = millis();
  if (millis() - uploadTimepoint > 15000U) {
    uploadTimepoint = millis();

    for (int i = 0; i < SCOUNT; i++) {
      analogBufferTemp[i] = analogBuffer[i];
    }

    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * VREF / 4096.0;
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVoltage = averageVoltage / compensationCoefficient;

    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage 
               - 255.86 * compensationVoltage * compensationVoltage 
               + 857.39 * compensationVoltage) * 0.5;

    Serial.print("Compensated Voltage: ");
    Serial.println(compensationVoltage, 2);
    Serial.print("TDS Value: ");
    Serial.println(tdsValue, 0);

    if (tdsValue > 0) {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(tdsValue);
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
          Serial.println("Data sent to ThingSpeak successfully");
        } else {
          Serial.println("Error sending data to ThingSpeak");
        }
        http.end();
      } else {
        Serial.println("WiFi not connected. Unable to send data.");
      }
    } else {
      Serial.println("Invalid TDS Value. Skipping upload.");
    }
  }
}
