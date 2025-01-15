#include "appGlobals.h"
#include "BluetoothCameraServer.h"

// Dichiarazione delle variabili globali
BLEService *generalService;
BLECharacteristic *commandCharacteristic;
extern bool forceRecord;
// Setup del BLE
void setup_BLE_ESP32()
{
  BLEDevice::init("CameraModule");
  BLEDevice::setMTU(512);
  BLEServer *bleServer = BLEDevice::createServer(); // Create server
  bleServer->setCallbacks(new ServerCallbacksHandler()); // Set callbacks for connections and disconnections

  CharacteristicCallback *charCallback = new CharacteristicCallback();
  generalService = bleServer->createService(GENERAL_SERVICE_UUID);
 
  // Command characteristic
  commandCharacteristic = generalService->createCharacteristic(
      COMMAND_CHARACTERISTIC_UUID, 
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  commandCharacteristic->setCallbacks(charCallback);
 
  generalService->start();

  // Add the service to our "advertisement" and start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(GENERAL_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined!");
}

// Funzione per il parsing dei comandi
void parse_commands(char* commands)
{
  char* token = strtok(commands, ","); // Token iniziale

  while (token != NULL)
  {
    std::string command = token;

    if (command.find("rec") == 0)
    {
      Serial.println("Command: start recording");
      forceRecord = true;
    }
    else if (command.find("stp") == 0)
    {
      Serial.println("Command: stop recording");
      forceRecord = false;
    }
    else
    {
      Serial.print("Unknown command: ");
      Serial.println(command.c_str());
    }

    token = strtok(NULL, ","); // Leggi il prossimo token
  }
}

