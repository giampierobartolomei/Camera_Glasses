#include "appGlobals.h"
#include "BluetoothCameraServer.h"
//#include "globals.h"
// Dichiarazione delle variabili globali
BLEService *generalService;
BLECharacteristic *commandCharacteristic;
extern bool forceRecord;
// Dichiarazione del task per l'aggiornamento della tensione
void voltageUpdateTask(void *parameter);
// Setup del BLE
void setup_BLE_ESP32()
{
  BLEDevice::init("CameraModule"); //cambiare nome in CameraModule2 per la seconda camera
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


      // Creazione del task per aggiornare la tensione
  xTaskCreatePinnedToCore(
      voltageUpdateTask,   // Funzione del task
      "VoltageUpdateTask", // Nome del task
      2048,               // Dimensione dello stack
      NULL,               // Parametro
      1,                  // Priorità
      NULL,               // Handle del task
      0                   // Core su cui eseguirlo
  );
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

void voltageUpdateTask(void *parameter)
{
    for (;;)
    {
        float voltage = (float)(smoothAnalog(5)) * 3.3 * 2 / 8191; //prendere valore da peripherals.cpp MAX_ADC 8191 O 4095 a seconda se adc bit 12 o 13
        voltage *= (3.50/3.28); //fattore di conversione dovuto a tolleranze

        // Converti il valore in una stringa
        char voltageStr[16];
        dtostrf(voltage, 1, 4, voltageStr); // Converte il float in stringa con 4 decimali

        // Invia la stringa come UTF-8
        commandCharacteristic->setValue((uint8_t *)voltageStr, strlen(voltageStr));
        commandCharacteristic->notify();

        //Serial.print("Voltage [V]: ");
        //Serial.println(voltage);
        vTaskDelay(10000 / portTICK_PERIOD_MS); // Aggiornamento ogni 5 secondi
    }
}