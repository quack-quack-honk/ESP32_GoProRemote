#include <Arduino.h>
#include "BLEDevice.h"

#define ServiceCharacteristic(S, C) ThisClient->getService(S)->getCharacteristic(C)

static BLEDevice *ThisDevice;
static BLEClient *ThisClient = ThisDevice->createClient();
BLEScan *ThisScan = ThisDevice->getScan();
static BLEUUID ServiceUUID((uint16_t)0xFEA6);
static BLEUUID CommandWriteCharacteristicUUID("b5f90072-aa8d-11e3-9046-0002a5d5c51b");
static bool IsConnected = false;

bool ScanAndConnect(void)
{
    ThisScan->clearResults();
    ThisScan->start(3);
    for (int i = 0; i < ThisScan->getResults().getCount(); i++)
        if (ThisScan->getResults().getDevice(i).haveServiceUUID() && ThisScan->getResults().getDevice(i).isAdvertisingService(BLEUUID(ServiceUUID)))
        {
            ThisScan->stop();
            if (ThisClient->connect(new BLEAdvertisedDevice(ThisScan->getResults().getDevice(i))))
            {
                IsConnected = true;
                return true;
            }
        }
    return false;
}

void setup(void)
{
    ThisDevice->init("");
    ThisDevice->setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    Serial.begin(115200);
    Serial.println("GoPro Control Ready");    Serial.println("Commands:");
    Serial.println("'connect' - Connect to GoPro");
    Serial.println("'video' - Switch to video mode");
    Serial.println("'photo' - Switch to photo mode");
    Serial.println("'timelapse' - Switch to timelapse mode");
    Serial.println("'trigger' - Start/take photo or video");
    Serial.println("'stop' - Stop recording");
    Serial.println("'power' - Power off GoPro");
}

void loop(void)
{
    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == "connect" && !IsConnected)
        {
            Serial.println("Connecting to GoPro...");
            if (ScanAndConnect())
            {
                Serial.println("Connected to GoPro successfully");
            }
            else
            {
                Serial.println("Failed to connect to GoPro");
                IsConnected = false;        }        }
        else if (command == "video" && IsConnected)
        {
            Serial.println("Switching to video mode...");            
            auto* characteristic = ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID);
            characteristic->writeValue({0x04, 0x3E, 0x02, 0x03, 0xE8});
            std::string response = characteristic->readValue();
            Serial.print("Response: ");
            for(int i = 0; i < response.length(); i++) {
                Serial.printf("%02X ", (uint8_t)response[i]);
            }
            Serial.println();
        }
        else if (command == "photo" && IsConnected)
        {
            Serial.println("Switching to photo mode...");            
            auto* characteristic = ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID);
            characteristic->writeValue({0x04, 0x3E, 0x02, 0x03, 0xE9});
            std::string response = characteristic->readValue();
            Serial.print("Response: ");
            for(int i = 0; i < response.length(); i++) {
                Serial.printf("%02X ", (uint8_t)response[i]);
            }
            Serial.println();
        }
        else if (command == "timelapse" && IsConnected)
        {
            Serial.println("Switching to timelapse mode...");            
            auto* characteristic = ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID);
            characteristic->writeValue({0x04, 0x3E, 0x02, 0x03, 0xEA});
            std::string response = characteristic->readValue();
            Serial.print("Response: ");
            for(int i = 0; i < response.length(); i++) {
                Serial.printf("%02X ", (uint8_t)response[i]);
            }
            Serial.println();
        }
        
        else if (command == "trigger" && IsConnected)
        {            
            Serial.println("Triggering camera...");            
            auto* characteristic = ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID);
            characteristic->writeValue({0x03, 0x01, 0x01, 0x01});
            std::string response = characteristic->readValue();
            Serial.print("Response: ");
            for(int i = 0; i < response.length(); i++) {
                Serial.printf("%02X ", (uint8_t)response[i]);
            }
            Serial.println();
        }
        else if (command == "stop" && IsConnected)
        {
            Serial.println("Stopping recording...");            
            auto* characteristic = ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID);
            characteristic->writeValue({0x03, 0x01, 0x01, 0x00});
            std::string response = characteristic->readValue();
            Serial.print("Response: ");
            for(int i = 0; i < response.length(); i++) {
                Serial.printf("%02X ", (uint8_t)response[i]);
            }
            Serial.println();
        }
        else if (command == "power" && IsConnected)
        {
            Serial.println("Powering off GoPro...");            
            auto* characteristic = ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID);
            characteristic->writeValue({0x01, 0x05});
            std::string response = characteristic->readValue();
            Serial.print("Response: ");
            for(int i = 0; i < response.length(); i++) {
                Serial.printf("%02X ", (uint8_t)response[i]);
            }
            Serial.println();
            IsConnected = false;
            Serial.println("GoPro powered off");
        }
        else if (command == "status")
        {
            Serial.print("Connection status: ");
            Serial.println(IsConnected ? "Connected" : "Disconnected");
        }
    }
}