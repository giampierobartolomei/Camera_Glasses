#ifndef SERIALCOMMANDHANDLER_H
#define SERIALCOMMANDHANDLER_H

#include <Arduino.h>

// Dichiarazione della funzione per avviare il task di gestione comandi seriali
void startSerialCommandTask(bool &forceRecord);

#endif
