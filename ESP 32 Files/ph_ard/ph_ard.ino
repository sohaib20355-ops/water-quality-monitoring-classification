float calibration_value = (7 + (5.70 * 4.58));  // Adjust this during calibration
int phval = 0; 
unsigned long int avgval;
int buffer_arr[10], temp;
const int sensorPin = A0;  // Use A0 for analog input on Arduino

void setup() 
{
  Serial.begin(9600);  // Set baud rate to 9600 for Arduino
  Serial.println("Welcome to Circuit Digest");
  delay(2000);
}

void loop() {
  // Read the sensor values 10 times and store them in buffer_arr
  for(int i = 0; i < 10; i++) 
  { 
    buffer_arr[i] = analogRead(sensorPin);  // Use the defined sensor pin (A0)
    delay(30);
  }

  // Sort the buffer values
  for(int i = 0; i < 9; i++)
  {
    for(int j = i + 1; j < 10; j++)
    {
      if(buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  // Average the middle 6 values to reduce noise
  avgval = 0;
  for(int i = 2; i < 8; i++)
    avgval += buffer_arr[i];

  // Calculate voltage
  float volt = (float)avgval * 5.0 / 1023 / 6;  // Use 5.0V and 10-bit resolution (0-1023)
  
  // Print the voltage for calibration purposes
  Serial.print("Voltage: ");
  Serial.println(volt);

  // Calculate pH value
  float ph_act = -5.70 * volt + calibration_value;

  Serial.print("pH Value: ");
  Serial.println(ph_act);
  
  delay(1000);
}
