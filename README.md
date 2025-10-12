# 🌡️ Smart Temperature Comfort Advisor (AglaMausam)

IoT-based embedded system using **ESP8266 (NodeMCU)** and **DHT22** sensor to continuously monitor temperature and humidity.  
Data is uploaded in real-time to **Firebase Realtime Database** for cloud storage and further analysis.  
Includes QA documentation, troubleshooting, and collaboration logs managed through **GitHub Issues**.

---

## 📘 Project Overview

The **Smart Temperature Comfort Advisor** is designed to measure environmental comfort levels by collecting temperature and humidity data from a DHT22 sensor and transmitting it to Firebase via the ESP8266 microcontroller.

This data can later be used by mobile or web applications to:
- Predict comfort levels (heat index)
- Provide voice-based climate tips
- Analyze long-term environmental patterns

---

## 🧩 Hardware Components

| Component | Function |
|------------|-----------|
| **ESP8266 (NodeMCU)** | Main controller; connects to Wi-Fi and uploads data to Firebase |
| **DHT22 Sensor** | Measures temperature (°C) and humidity (%) |
| **Jumper Wires** | For circuit connections |
| **Power Source (USB 5V)** | Powers the ESP8266 board |

---

## ⚙️ Working Principle

### **1. Data Collection**
The DHT22 sensor measures temperature and humidity from the surrounding environment every 2 seconds.

### **2. Data Transmission**
The ESP8266 connects to Wi-Fi (2.4 GHz) and sends the collected data (temperature, humidity, and timestamp) to Firebase Realtime Database.

### **3. Cloud Storage**
Firebase stores the data in structured JSON format for real-time access by other applications.

---

## 💻 Arduino Code Summary

```cpp
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "DHT.h"

#define DHTPIN 4       // D2 on NodeMCU
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"

#define FIREBASE_HOST "yourproject.firebaseio.com"
#define FIREBASE_AUTH "your_firebase_secret"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Failed to read from DHT22 sensor!");
    return;
  }

  Serial.print("Humidity: "); Serial.print(h);
  Serial.print(" %\tTemperature: "); Serial.print(t);
  Serial.println(" °C");

  FirebaseJson json;
  json.set("humidity", h);
  json.set("temperature", t);
  json.set("timestamp", millis());

  if (Firebase.pushJSON(firebaseData, "/DHT22", json))
    Serial.println("✅ Data stored at path: " + firebaseData.dataPath());
  else
    Serial.println("❌ Data failed: " + firebaseData.errorReason());

  delay(2000);
}
