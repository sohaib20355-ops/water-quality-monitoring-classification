void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
}

void loop() {
  int sensorValue = analogRead(35); // Read the input on analog pin 35
  float voltage = sensorValue * (3.3 / 4095.0); // Convert the analog reading to a voltage (0 - 3.3V)
  Serial.println(voltage); // Print out the voltage value
  delay(500); // Delay for 500ms
}
