int sensorPin = 34;  // GPIO32 for analog input

void setup() {
  Serial.begin(115200);  // Set baud rate for ESP32
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  Serial.println("Sensor Value: " + String(sensorValue));

  int turbidity = map(sensorValue, 0, 750, 100, 0);
  delay(100);
  Serial.println("Turbidity: " + String(turbidity));

  if (turbidity < 20) {
    Serial.println("Status: CLEAR");
  } 
  else if (turbidity >= 20 && turbidity <= 50) {
    Serial.println("Status: CLOUDY");
  } 
  else if (turbidity > 50) {
    Serial.println("Status: DIRTY");
  }

  delay(1000);  
}
