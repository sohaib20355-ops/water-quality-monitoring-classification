void setup() {
  // initialize serial communication at 115200 bits per second for ESP32:
  Serial.begin(115200);
  // allow time for the Serial monitor to start
  delay(1000);
}

// the loop routine runs over and over showing the voltage on A0
void loop() {
  // read the input on analog pin 0 (use GPIO 36 for A1 on ESP32):
  int sensorValue = analogRead(32); // A1 on ESP32 is GPIO 36

  // Convert the analog reading (0 - 4095) to a voltage (0 - 3.3V):
  float voltage = sensorValue * (3.3 / 4095.0);

  // print out the value you read:
  Serial.println(voltage);
  
  delay(300);
}
