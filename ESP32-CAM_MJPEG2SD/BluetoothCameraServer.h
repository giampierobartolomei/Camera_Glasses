#ifndef BLUETOOTH_CAMERASERVER_H
#define BLUETOOTH_CAMERASERVER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>

// UUID dei servizi BLE
#define GENERAL_SERVICE_UUID               "19B10010-E8F2-537E-4F6C-D104768A1214"
#define COMMAND_CHARACTERISTIC_UUID        "19b10012-e8f2-537e-4f6c-d104768a1214"

// Variabili globali
extern BLEService *generalService;
extern BLECharacteristic *commandCharacteristic;
//extern bool forceRecord;

// Funzioni
void setup_BLE_ESP32();
void parse_commands(char *commands);

// Callback del server BLE
class ServerCallbacksHandler : public BLEServerCallbacks
{
  void onConnect(BLEServer* pserver) override
  {
    Serial.print("Connected event, device: ");
    Serial.println(pserver->getConnId());
  }

  void onDisconnect(BLEServer* pserver) override
  {
    Serial.println("Disconnected event");
    BLEDevice::startAdvertising();
  }
};

// Callback delle caratteristiche BLE
class CharacteristicCallback : public BLECharacteristicCallbacks
{
void onWrite(BLECharacteristic *pCharacteristic) override
{
    // Ottieni il valore come std::string
    String rxValue = pCharacteristic->getValue();

    // Usa il valore come char* per la funzione di parsing
    if (pCharacteristic->getUUID().toString() == COMMAND_CHARACTERISTIC_UUID)
    {
        Serial.println("Command arrived via BLE:");
        parse_commands((char *)rxValue.c_str()); // Passa il char* al parser
    }
}
};

#endif
