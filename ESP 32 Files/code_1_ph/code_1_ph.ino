// Pin configuration
int phSensorPin = 34;        // pH sensor connected to GPIO34 (analog input)
int turbiditySensorPin = 36; // Turbidity sensor connected to GPIO36 (analog input)

// Calibration value for pH sensor
float phCalibrationValue = 21.34 + 0.7;

// Variables for storing readings
unsigned long int avgValue; // Store the average value of pH sensor readings
int buf[10], temp;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate
  delay(1000); // Allow time for serial monitor initialization
  Serial.println("Select the option:");
  Serial.println("1: pH Reading");
  Serial.println("2: Turbidity Reading");
}

void loop() {
  // Check if the user has entered a choice
  if (Serial.available() > 0) {
    char choice = Serial.read(); // Read the user's choice

    if (choice == '1') {
      readPH(); // Function to read and display pH value
    } else if (choice == '2') {
      readTurbidity(); // Function to read and display turbidity value
    } else {
      Serial.println("Invalid choice. Please enter 1 for pH or 2 for Turbidity.");
    }

    // Prompt for the next choice
    Serial.println("\nSelect the option:");
    Serial.println("1: pH Reading");
    Serial.println("2: Turbidity Reading");
  }
}

// Function to read and display pH value
void readPH() {
  // Get 10 samples from the pH sensor for smoothing
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(phSensorPin);
    delay(10);
  }

  // Sort the samples from smallest to largest
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  // Average the middle 6 samples
  avgValue = 0;
  for (int i = 2; i < 8; i++) avgValue += buf[i];

  // Convert the analog reading to a pH value
  float phValue = (float)avgValue * 3.3 / 4095 / 6; // Convert to voltage (ESP32 is 12-bit, 3.3V)
  phValue = -5.70 * phValue + phCalibrationValue; // Convert to pH value

  // Display pH value on the Serial Monitor
  Serial.print("pH Value: ");
  Serial.println(phValue);

  // Display pH status based on the value
  if (phValue < 4) {
    Serial.println("Status: Very Acidic");
  } else if (phValue >= 4 && phValue < 5) {
    Serial.println("Status: Acidic");
  } else if (phValue >= 5 && phValue < 7) {
    Serial.println("Status: Acidic-ish");
  } else if (phValue >= 7 && phValue < 8) {
    Serial.println("Status: Neutral");
  } else if (phValue >= 8 && phValue < 10) {
    Serial.println("Status: Alkaline-ish");
  } else if (phValue >= 10 && phValue < 11) {
    Serial.println("Status: Alkaline");
  } else if (phValue >= 11) {
    Serial.println("Status: Very Alkaline");
  }
}

// Function to read and display turbidity value
void readTurbidity() {
  int sensorValue = analogRead(turbiditySensorPin); // Read the analog input from turbidity sensor
  Serial.println("Raw Turbidity Sensor Value: " + String(sensorValue)); // Print raw sensor value for debugging
  
  // Map the sensor value to a turbidity range (0-100%)
  int turbidity = map(sensorValue, 0, 750, 100, 0); // Mapping values based on expected sensor output

  // Display turbidity on the Serial Monitor
  Serial.print("Turbidity: ");
  Serial.print(turbidity);
  Serial.println("%");

  // Determine water quality based on turbidity value
  if (turbidity < 20) {
    Serial.println("Status: CLEAR");
  } else if (turbidity >= 20 && turbidity < 50) {
    Serial.println("Status: CLOUDY");
  } else if (turbidity >= 50) {
    Serial.println("Status: DIRTY");
  }
}
