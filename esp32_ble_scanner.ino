#include <Arduino.h>
#include <sstream>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCAN_LENGTH 30     // scan length

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice device)
    {
      String addr = device.getAddress().toString().c_str();
      String rssi = (String) device.getRSSI();
      Serial.println("Found " + addr + " with RSSI: " + rssi);
    }
};

void setup() {
  Serial.begin(9600);
  //enabling BLE
  Serial.println("Enabling BLE");
  BLEDevice::init("");

  BLEScan *pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(0x50);
  pBLEScan->setWindow(0x30);

  BLEScanResults foundDevices = pBLEScan->start(SCAN_LENGTH, false);
  int count = foundDevices.getCount();
  Serial.println("Found " + (String) count + " BLE devices");

}

void loop() {
  
}
