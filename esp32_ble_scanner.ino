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

  int SafeSkiingDevice = 0;
  
  for (uint8_t i=0; i<total; i++){
    BLEAdvertisedDevice device = devices.getDevice(i);
    String dname = device.getName().c_str();
    if (dname =="SafeSkiing"){
      SafeSkiingDevice++;
    }
  }
  
  Serial.println("SafeSkiing Devices: " + (String)SafeSkiingDevice);
  
  char *fullmsg = (char*)malloc((sizeof(char) * 18 * SafeSkiingDevice) +10);
  int array_position = 0;
  for (uint8_t i=0; i<total; i++){
      BLEAdvertisedDevice device = devices.getDevice(i);
      String dname = device.getName().c_str();
      
      if (dname =="SafeSkiing"){
        const char * addr = device.getAddress().toString().c_str();
        Serial.println("Found! adding in pos " + (String) addr);
        for (uint8_t j = 0; j< 17; j++){
          Serial.print(addr[j]);
          fullmsg[array_position+j] = addr[j];
        }
        Serial.println("");
        fullmsg[array_position+17] = ',';
        array_position += 18;

        for (uint8_t k=0; k<=(18 * SafeSkiingDevice); k++){
          Serial.print(fullmsg[k]);
        }
        Serial.println("");
      }
  }
  //publish to topics
  if (array_position > 0){
    mqttClient.publish(TOPIC1, fullmsg);
  } else {
    mqttClient.publish(TOPIC1, "");
  }
  
  free(fullmsg);
  
  char msg[7];
  String empty;
  ((String)total).toCharArray(msg, 7);
  mqttClient.publish(TOPIC2, msg);
  pBLEScan->stop();
  pBLEScan->clearResults();
}
