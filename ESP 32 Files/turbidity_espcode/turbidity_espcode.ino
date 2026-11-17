int sensor = 0; // variable for averaging
int n = 25; // number of samples to average
int sensorValue = 0;
float voltage = 0.00;
float turbidity = 0.00;
float Vclear = 1.716; // Fixed Vclear value (calibrated value)

void setup() {
  Serial.begin(115200);
  
  // No need to initialize EEPROM or read Vclear from EEPROM anymore
  
  Serial.print("Using Vclear value: ");
  Serial.println(Vclear, 3);
  
  delay(3000); // Pause for 3 seconds
}

void loop() {
  // Sensing routine:
  for (int i = 0; i < n; i++) {
    sensor += analogRead(35); // Use GPIO 35 for the analog input (adjust if needed)
    delay(10);
  }
  sensorValue = sensor / n; // average the n values
  voltage = sensorValue * (3.300 / 4095.000); // Convert analog (0-4095) to voltage (0 - 3.3V)

  // Debugging output for voltage and Vclear
  Serial.print("Calculated voltage: ");
  Serial.println(voltage, 3);
  Serial.print("Vclear: ");
  Serial.println(Vclear, 3);

  // Protect against invalid Vclear
  if (Vclear <= 0.0 || Vclear > 3.3) {
    Serial.println("Error: Invalid Vclear value.");
    return; // Exit if Vclear is invalid
  }

  // Check if voltage is greater than Vclear
  if (voltage > Vclear) {
    Serial.println("Error: Voltage exceeds Vclear.");
    turbidity = 0; // Set turbidity to 0 or handle the error case
  } else {
    turbidity = 20.00 * (100.00 - (voltage / Vclear) * 100.00); // Calculate turbidity using fixed Vclear
  }

  // Serial display of voltage and turbidity
  Serial.print("Raw voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V, Turbidity: ");
  Serial.print(turbidity, 3);
  Serial.println(" NTU");

  sensor = 0; // resets for averaging
  delay(1000); // Pause for 1 second. // sampling rate
}
