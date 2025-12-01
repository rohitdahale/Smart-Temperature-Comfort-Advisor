# 🌾 Smart Agriculture & Crop Prediction System (SmartFarm)

IoT-based embedded system using **ESP8266 (NodeMCU)** with **Soil Moisture Sensor, DHT11, and pH Sensor** to continuously monitor soil conditions.  
Data is uploaded in real-time to **Firebase Realtime Database** for cloud analytics and crop prediction.  
Includes complete project documentation, SDG mapping, methodology, and hardware details.

---

## 📘 Project Overview

The **Smart Agriculture & Crop Prediction System** monitors environmental and soil parameters such as:

- 🌡️ **Temperature**  
- 💧 **Humidity**  
- 🌱 **Soil Moisture**  
- ⚗️ **Soil pH**

The collected data is used for:

- Real-time soil monitoring  
- Cloud-based storage and visualization  
- Crop suitability prediction  
- Smart farming recommendations  

This system assists farmers in choosing the **best crop** for their land, improving productivity and sustainability.

---

## 🧩 Hardware Components

| Component | Function |
|----------|----------|
| **ESP8266 (NodeMCU)** | Main IoT controller with Wi-Fi |
| **DHT11 Sensor** | Measures temperature & humidity |
| **Soil Moisture Sensor** | Tracks water content in soil |
| **pH Sensor** | Measures soil acidity/alkalinity |
| **Jumper Wires, Breadboard** | Circuit setup |
| **Power Supply / Battery** | Powers the ESP8266 |

---

## ⚙️ Working Principle

### **1. Data Collection**
Sensors measure soil moisture, pH, temperature, and humidity at regular intervals.

### **2. Data Processing**
Sensor values are filtered, calibrated, and converted into meaningful physical quantities.

### **3. Data Transmission**
ESP8266 connects to Wi-Fi and sends the processed data to **Firebase Realtime Database**.

### **4. Cloud Storage**
Firebase stores readings in a structured JSON tree for analysis, graphs, and dashboards.

### **5. Crop Prediction**
Based on:
- Soil moisture  
- pH range  
- Temperature  

The system compares values with crop requirement datasets and recommends the most suitable crops.

---

## 💻 Arduino Code Summary

```cpp
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

#define WIFI_SSID "YourWiFi"
#define WIFI_PASSWORD "YourPassword"

#define API_KEY "YourFirebaseAPIKey"
#define DATABASE_URL "yourfirebase.firebaseio.com"

#define DHTPIN D4
#define DHTTYPE DHT11
#define ANALOG_PIN A0

DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.signUp(&config, &auth, "", "");
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

float readPH() {
  int samples = 10;
  float total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(ANALOG_PIN);
    delay(10);
  }
  float avgADC = total / samples;
  float voltage = avgADC * (3.3 / 1023.0);
  float phValue = 7 + ((2.5 - voltage) / 0.18);
  return phValue;
}

int readMoisture() {
  int val = analogRead(ANALOG_PIN);
  return map(val, 1023, 0, 0, 100);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int moisture = readMoisture();
  float pH = readPH();

  String path = "/SmartFarm/Readings/" + String(millis());

  Firebase.RTDB.setFloat(&fbdo, path + "/Temperature", temperature);
  Firebase.RTDB.setFloat(&fbdo, path + "/Humidity", humidity);
  Firebase.RTDB.setInt(&fbdo, path + "/Moisture", moisture);
  Firebase.RTDB.setFloat(&fbdo, path + "/pH", pH);

  delay(5000);
}
