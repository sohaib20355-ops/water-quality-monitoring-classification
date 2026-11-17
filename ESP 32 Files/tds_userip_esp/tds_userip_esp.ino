#define TdsSensorPin 27
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample points

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;       // current temperature for compensation

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
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
  Serial.println("Enter 'y' to take a new reading or 'n' to cancel.");
}

void loop() {
  static bool userConfirmed = false;

  // Check for user input
  if (Serial.available() > 0) {
    char userInput = Serial.read();
    if (userInput == 'y' || userInput == 'Y') {
      userConfirmed = true;
    } else if (userInput == 'n' || userInput == 'N') {
      userConfirmed = false;
      Serial.println("Measurement aborted by user.");
      Serial.println("Enter 'y' to take a new reading or 'n' to cancel.");
    }
  }

  // Only proceed if the user has confirmed
  if (userConfirmed) {
    // Collect samples every 40 milliseconds
    static unsigned long analogSampleTimepoint = millis();
    if (millis() - analogSampleTimepoint > 40U) {
      analogSampleTimepoint = millis();
      analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); // Read and store analog value in buffer
      analogBufferIndex++;
      if (analogBufferIndex == SCOUNT) {
        analogBufferIndex = 0;
      }
    }

    // Calculate TDS value every 800 milliseconds
    static unsigned long printTimepoint = millis();
    if (millis() - printTimepoint > 800U) {
      printTimepoint = millis();

      for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
        analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      }

      // Calculate median filtered voltage
      averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;

      // Temperature compensation
      float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
      float compensationVoltage = averageVoltage / compensationCoefficient;

      // Convert voltage to TDS value
      tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
                - 255.86 * compensationVoltage * compensationVoltage
                + 857.39 * compensationVoltage) * 0.5;

      Serial.print("TDS Value: ");
      Serial.print(tdsValue, 0);
      Serial.println(" ppm");

      // Reset user confirmation flag and prompt for next input
      userConfirmed = false;
      Serial.println("Enter 'y' to take a new reading or 'n' to cancel.");
    }
  }
}
