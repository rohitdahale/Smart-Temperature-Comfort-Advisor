#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "DHT.h"

// ---------- DHT SENSOR ----------
#define DHTPIN 4       // GPIO2 (D4 on NodeMCU)
#define DHTTYPE DHT22  // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// ---------- WIFI CONFIG ----------
#define WIFI_SSID "Rohit1123"
#define WIFI_PASSWORD "12121212"

// ---------- FIREBASE CONFIG ----------
#define FIREBASE_HOST "tempadvisor-default-rtdb.firebaseio.com"   // without https://
#define FIREBASE_AUTH "6WVFZ2E3bTMzin8vAVPVswIqPiQ7cS4cHfwqvCtk" // DB secret or token

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // Firebase configuration
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
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

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" °C");

  // Create JSON object
  FirebaseJson json;
  json.set("humidity", h);
  json.set("temperature", t);
  json.set("timestamp", millis());  // or replace with real time from NTP

  // Push JSON under DHT22 (Firebase will create unique key instance)
  if (Firebase.pushJSON(firebaseData, "/DHT22", json)) {
    Serial.println("✅ Data stored at path: " + firebaseData.dataPath());
  } else {
    Serial.println("❌ Data failed: " + firebaseData.errorReason());
  }

  delay(2000); // wait 2s
}
