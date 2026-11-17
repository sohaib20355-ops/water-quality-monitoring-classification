# IoT-Based Water Quality Monitoring and Classification

Semester 4 Data Science project. ESP32 sensors (pH, TDS, turbidity, temperature) send readings to ThingSpeak; datasets are used to classify water quality / potability.

## Contents
- `ESP 32 Files/` – Arduino sketches for each sensor and the combined ThingSpeak uploader
- `Datasets/` – water quality / potability datasets (the largest CSVs and zip archives are not included because of GitHub's 100 MB limit)
- `Presentation Images/`, `*.pptx`, `*.docx` – presentations and project reports
- `esp32-turbidity-master*`, `water-quality-monitoring-system-main/` – reference projects used

## Configuration
WiFi credentials and the ThingSpeak write API key in the `.ino` files are placeholders (`YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, `YOUR_THINGSPEAK_WRITE_API_KEY`). Replace them before flashing.
