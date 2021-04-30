#include <Arduino.h>

#include <NimBLEDevice.h>
#include <NimBLEScan.h>

#define SCAN_LENGTH 5               // scan length

NimBLEScan *pBLEScan;

void setup() {
  Serial.begin(9600);

  //turning on BLE
  NimBLEDevice::init("");
  pBLEScan = NimBLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);
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
          fullmsg += (uuid + (String)"," + (String)device.getRSSI() + (String)"," + battery + (String)";");
          totalSSD++;
        }
      }
  }
  
  Serial.println("SafeSkiing Devices: " + (String) totalSSD);
  Serial.println(fullmsg);
  Serial.println("YOU CAN MOVE NOW");
  delay(5000);
}
