#include <Arduino.h>
#include "BLEDevice.h"

#define ServiceCharacteristic(S, C) ThisClient->getService(S)->getCharacteristic(C)

static BLEDevice *ThisDevice;
static BLEClient *ThisClient = ThisDevice->createClient();
BLEScan *ThisScan = ThisDevice->getScan();
static BLEUUID ServiceUUID((uint16_t)0xFEA6);
static BLEUUID CommandWriteCharacteristicUUID("b5f90072-aa8d-11e3-9046-0002a5d5c51b");
static bool IsConnected = false;
static bool IsRecording = false;

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
    Serial.println("GoPro Control Ready");
    Serial.println("Commands:");
    Serial.println("'connect' - Connect to GoPro");
    Serial.println("'start' - Start recording");
    Serial.println("'stop' - Stop recording");
    Serial.println("'power' - Power off GoPro");
    Serial.println("'status' - Show connection and recording status");
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
                IsConnected = false;
            }
        }
        else if (command == "start" && IsConnected && !IsRecording)
        {
            Serial.println("Starting recording...");
            ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID)->writeValue({0x03, 0x01, 0x01, 0x01});
            IsRecording = true;
            Serial.println("Recording started");
        }
        else if (command == "stop" && IsConnected && IsRecording)
        {
            Serial.println("Stopping recording...");
            ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID)->writeValue({0x03, 0x01, 0x01, 0x00});
            IsRecording = false;
            Serial.println("Recording stopped");
        }
        else if (command == "power" && IsConnected)
        {
            Serial.println("Powering off GoPro...");
            ServiceCharacteristic(ServiceUUID, CommandWriteCharacteristicUUID)->writeValue({0x01, 0x05});
            IsConnected = false;
            IsRecording = false;
            Serial.println("GoPro powered off");
        }
        else if (command == "status")
        {
            Serial.print("Connection status: ");
            Serial.println(IsConnected ? "Connected" : "Disconnected");
            Serial.print("Recording status: ");
            Serial.println(IsRecording ? "Recording" : "Stopped");
        }
    }
}