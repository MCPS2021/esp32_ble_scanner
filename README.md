# esp32_ble_scanner

This is an Arduino sketch for ESP32 devices to be able to scan BLE devices

## Description
The goal of this repo is to have an ESP32 that scan the BLE devices availables
and if they are "Safe Skiing" BLE Devices, it gets the UUID and RSSI (Battery State)
and it publishes them to a MQTT Broker

## Installation
You need an ESP32 Dev board and Arduino IDE with the following packages:
- "esp32" in Board Configurator by Espressif Systems
- "ESP32 BLE Arduino" by Neil Kolban
