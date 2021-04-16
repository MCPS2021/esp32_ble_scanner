#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <PubSubClient.h>

#include <WiFi.h>
#include "wifi_config.h"

#define SCAN_LENGTH 30               // scan length
#define MQTT_SERVER "192.168.1.100"               // MQTT Broker URL
#define MQTT_PORT 1883               //MQTT Broker PORT
#define MQTT_NAME "Station1"
#define TOPIC1 "station1/UUIDs"      // MQTT topic 1
#define TOPIC2 "station1/totalPeople" //MQTT topic 2

//MQTT 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//BLE Callback
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice device)
    {
      String dname = device.getName().c_str();
      String addr = device.getAddress().toString().c_str();
      String rssi = (String) device.getRSSI();

      if (dname =="SafeSkiing"){
        Serial.println("Found " + dname + " with " + addr + " with RSSI: " + rssi);
        
        mqttClient.publish(TOPIC1, "Hello from ESP32");
      }
    }
};

void setup() {
  Serial.begin(9600);
  
  //connecting to wifi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  //turning on BLE
  BLEDevice::init("");
  Serial.println("BLE Enabled");

  //connecting to MQTT
  Serial.print("Connecting to MQTT...");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  while (!mqttClient.connected()) {
      Serial.print(".");
   
      if (mqttClient.connect(MQTT_NAME)) {
        Serial.println("\nMQTT Connected!");  
      } else {
        Serial.println("\nCannot Connect to MQTT Server");
        Serial.println(mqttClient.state());
        delay(2000);
      }
  }
}

void loop() {
  BLEScan *pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);

  BLEScanResults foundDevices = pBLEScan->start(SCAN_LENGTH, false);
  int count = foundDevices.getCount();
}
