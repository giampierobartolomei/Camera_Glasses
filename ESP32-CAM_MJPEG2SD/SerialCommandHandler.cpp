#include "SerialCommandHandler.h"

void serialCommandTask(void* parameter) {
  // Parametri passati al task
  auto forceRecord = static_cast<bool*>(parameter);

  for (;;) {
    if (Serial.available() > 0) {
      // Lettura del comando
      int command = Serial.parseInt();
      switch (command) {
        case 2:
          *forceRecord = false;
          Serial.println("Comando ricevuto: 2. Registrazione fermata.");
          break;
        case 1:
          *forceRecord = true;
          Serial.println("Comando ricevuto: 1. Registrazione avviata.");
          break;
        default:
          break;
      }
    }
    delay(100); // Ritardo per ridurre il consumo di CPU
  }
}

void startSerialCommandTask(bool &forceRecord) {
  // Creazione del task
  xTaskCreate(
    serialCommandTask,   // Funzione del task
    "SerialCommandTask", // Nome del task
    8192,                // Dimensione dello stack
    &forceRecord,        // Parametro passato al task
    1,                   // Priorità
    NULL                 // Handle del task (non necessario qui)
  );
}
