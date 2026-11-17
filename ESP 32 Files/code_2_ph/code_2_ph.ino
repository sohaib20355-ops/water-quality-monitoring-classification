#include <Arduino.h>

// Calibration value for the pH sensor (adjust this based on your calibration procedure)
float calibration_value = 20.24 - 0.7;  // You may need to adjust this based on your sensor
int buffer_arr[10], temp;
float ph_act;

void setup() {
  // Start Serial communication
  Serial.begin(9600);  
  delay(2000);  // Wait a bit before starting to give the sensor time to initialize
}

void loop() {
  unsigned long avgval = 0;

  // Read 10 samples from the pH sensor
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(35);  // Read from ADC pin 35 (change if necessary)
    delay(30);  // Small delay to allow the sensor time to stabilize
  }

  // Sort the samples to remove outliers
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  // Calculate the average of the middle 6 sorted values
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }

  // Convert the average ADC value to voltage (ESP32 uses a 12-bit ADC)
  float voltage = (float)avgval * 3.3 / 4096.0 / 6;  // 3.3V reference and 12-bit ADC resolution
  
  // Print the raw ADC value and voltage to Serial Monitor
  Serial.print("Raw ADC Value: ");
  Serial.print(buffer_arr[0]);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);

  // Calculate pH value from the voltage (formula depends on your specific sensor)
  ph_act = (voltage - 2.5) / (-0.05916);
  // Adjust the multiplier and calibration value as needed

  // Print the pH value to Serial Monitor
  Serial.print("pH Value: ");
  Serial.println(ph_act);

  delay(1000);  // Wait for 1 second before taking the next reading
}
