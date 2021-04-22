#include <Arduino.h>
#include <cstring>
#include <iostream>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include <PubSubClient.h>

#include <WiFi.h>
#include "wifi_config.h"

#define SCAN_LENGTH 30               // scan length
#define MQTT_NAME "Station1"
#define TOPIC1 "station1/UUIDs"      // MQTT topic 1
#define TOPIC2 "station1/totalPeople" //MQTT topic 2

//MQTT 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

BLEScan *pBLEScan;

void setup() {
  Serial.begin(9600);
  
  //connecting to wifi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("\nWiFi Connected!");

  //turning on BLE
  BLEDevice::init("");
  Serial.println("BLE Enabled");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);

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
  Serial.println("Started BLE Scan");
  BLEScanResults devices = pBLEScan->start(SCAN_LENGTH, false);
  uint8_t total = devices.getCount();

  Serial.println("Devices: " + (String)total);

  int SafeSkiingDevice = 0;
  
  for (uint8_t i=0; i<total; i++){
    BLEAdvertisedDevice device = devices.getDevice(i);
    String dname = device.getName().c_str();
    if (dname =="SafeSkiing"){
      SafeSkiingDevice++;
    }
  }
  char fullmsg[1000];
  for (uint8_t i=0; i<total; i++){
      BLEAdvertisedDevice device = devices.getDevice(i);
      String dname = device.getName().c_str();
      
      if (dname =="SafeSkiing"){
        Serial.println("Found!");
        std::strcpy(fullmsg, device.getAddress().toString().c_str());
        Serial.println(fullmsg);
      }
  }
  
  //publish to topics
  mqttClient.publish(TOPIC1, fullmsg);
  
  char msg[7];
  String empty;
  ((String)total).toCharArray(msg, 7);
  mqttClient.publish(TOPIC2, msg);
  pBLEScan->clearResults();
}
