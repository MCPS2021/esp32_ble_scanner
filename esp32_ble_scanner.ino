#include <Arduino.h>

#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <PubSubClient.h>

#include <WiFi.h>
#include "wifi_config.h"

#define SCAN_LENGTH 9               // scan length
#define MQTT_NAME "Station1"
#define TOPIC1 "station1/UUIDs"      // MQTT topic 1
#define TOPIC2 "station1/totalPeople" //MQTT topic 2

//MQTT 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

NimBLEScan *pBLEScan;

void setup() {
  Serial.begin(9600);
  
  //connecting to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  //turning on BLE
  NimBLEDevice::init("");
  pBLEScan = NimBLEDevice::getScan(); //create new scan
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

String getPayload(String str){
  String start = "manufacturer data: ";
  String finish = "\n";
  int locStart = str.indexOf(start);
  if (locStart==-1) return "";
  locStart += start.length();
  return str.substring(locStart, locStart+50);
}

void loop() {
  Serial.println("Started BLE Scan");
  NimBLEScanResults devices = pBLEScan->start(SCAN_LENGTH, false);
  uint8_t total = devices.getCount();
  
  String fullmsg = "";
  int totalSSD = 0;
  for (uint8_t i=0; i<total; i++){
      NimBLEAdvertisedDevice device = devices.getDevice(i);
      String dname = device.getName().c_str();
      
      if (dname =="SafeSkiing"){
        String payload = getPayload(device.toString().c_str());
        if (payload != ""){
          String uuid = payload.substring(8, 40);
          String battery = payload.substring(48,50);
          fullmsg += (uuid + (String)"," + battery + (String)";");
          totalSSD++;
        }
      }
  }
  //publish to topics
  const char * fullmsg2 = fullmsg.c_str();
  mqttClient.publish(TOPIC1, fullmsg2);
  
  Serial.println("SafeSkiing Devices: " + (String) totalSSD);
  
  mqttClient.publish(TOPIC2, ((String)totalSSD).c_str());
  delay(1000);
}
