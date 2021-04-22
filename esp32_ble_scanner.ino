#include <Arduino.h>
#include <cstring>
#include <iostream>
#include <esp.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include <PubSubClient.h>

#include <WiFi.h>
#include "wifi_config.h"

#define SCAN_LENGTH 10               // scan length
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
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  //turning on BLE
  BLEDevice::init("");
  Serial.println("BLE Enabled");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);

  //connecting to MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  while (!mqttClient.connected()) {
   
      if (!mqttClient.connect(MQTT_NAME)) {
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
  
  String fullmsg = "";
  int total = 0;
  for (uint8_t i=0; i<total; i++){
      BLEAdvertisedDevice device = devices.getDevice(i);
      String dname = device.getName().c_str();
      
      if (dname =="SafeSkiing"){
        String addr = device.getAddress().toString().c_str();
        fullmsg += (addr + (String)",");
        total++;
      }
  }
  //publish to topics
  const char * fullmsg2 = fullmsg.c_str();
  mqttClient.publish(TOPIC1, fullmsg2);
  
  
  char msg[7];
  String empty;
  ((String)total).toCharArray(msg, 7);
  mqttClient.publish(TOPIC2, msg);

  ESP.restart();
}
