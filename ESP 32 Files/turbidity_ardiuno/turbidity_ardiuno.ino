void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  pinMode(A0, INPUT); // Ensure A0 is set as an input
}

void loop() {
  const int numSamples = 10; // Number of samples for averaging
  int total = 0;

  // Take multiple readings and calculate average
  for (int i = 0; i < numSamples; i++) {
    int sensorValue = analogRead(A0);
    total += sensorValue;
    delay(50); // Short delay between samples
  }

  int averageValue = total / numSamples; // Calculate average
  float voltage = averageValue * (5.0 / 1024.0); // Convert to voltage
  
  // Debugging information
  Serial.print("Raw ADC Value: ");
  Serial.print(averageValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);

  delay(500); // Wait before next reading
}
