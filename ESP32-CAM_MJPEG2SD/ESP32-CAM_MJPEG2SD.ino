/*
* Capture ESP32 Cam JPEG images into a AVI file and store on SD
* AVI files stored on the SD card can also be selected and streamed to a browser as MJPEG.
*
* s60sc 2020 - 2024

8/02: I tried to add prints on serial for battery voltage status, by modifyng this code, appGlobals.h, globals.h, pheriperals.cpp
Then I try to add WebDav functionalities. I modified also utils.cpp by putting to false use ping and setting ap ip and name.
*/

#include "appGlobals.h"
//#include "SerialCommandHandler.h" //added by me
#include "BluetoothCameraServer.h" //Added by me, from max code

bool forceRecord = false; // added by me and commented the declaratiion in mjpeg2sd.cpp
bool useMotion  = false; // whether to use camera for motion detection (with motionDetect.cpp), added by me, but only with changs on motionDetect.cpp it works

void setup() {
  logSetup();
  LOG_INF("Selected board %s", CAM_BOARD);
  // Legge e stampa subito la tensione della batteria
  float voltage = readVoltage();
  LOG_INF("Initial battery voltage: %.2fV", voltage);
  // prep storage
  if (startStorage()) {
    // Load saved user configuration
    if (loadConfig()) {
#ifndef AUXILIARY
      // initialise camera
      if (psramFound()) {
        if (ESP.getPsramSize() > 3 * ONEMEG) prepCam();
        else snprintf(startupFailure, SF_LEN, STARTUP_FAIL "Insufficient PSRAM for app: %s", fmtSize(ESP.getPsramSize()));
      } else snprintf(startupFailure, SF_LEN, STARTUP_FAIL "Need PSRAM to be enabled");
#else
      LOG_INF("AUXILIARY mode without camera");
#endif
    }
  }
  
#ifdef DEV_ONLY
  devSetup();
#endif

  // connect wifi or start config AP if router details not available
  startWifi();

  startWebServer();
  if (strlen(startupFailure)) LOG_WRN("%s", startupFailure);
  else {
    // start rest of services
#ifndef AUXILIARY
    startSustainTasks(); 
#endif
#if INCLUDE_SMTP
    prepSMTP(); 
#endif
#if INCLUDE_FTP_HFS
    prepUpload();
#endif
#if INCLUDE_UART
    prepUart();
#endif
#if INCLUDE_PERIPH
    prepPeripherals();
  #if INCLUDE_MCPWM 
    prepMotors();
  #endif
#endif
#if INCLUDE_AUDIO
    prepAudio(); 
#endif
#if INCLUDE_TGRAM
    prepTelegram();
#endif
#ifndef AUXILIARY
    prepRecording(); 
#endif
#if INCLUDE_I2C
  prepI2C();
  #if INCLUDE_TELEM
    prepTelemetry();
  #endif
#endif
#if INCLUDE_PERIPH
    startHeartbeat();
#endif
#if INCLUDE_RTSP
    prepRTSP();
#endif
    checkMemory("Setup");
    if (DEBUG_MEM) runTaskStats();
  } 
  
  setup_BLE_ESP32(); //from max code
  //startSerialCommandTask(forceRecord); //added by me for serial input start/stop recording and video saving
}
//It should appears: first battery level voltage, pheripherals setup, total tasks and then the updating of the battery level every 'VoltInterval' seconds
void loop() {
  // confirm not blocked in setup
  LOG_INF("=============== Total tasks: %u ===============\n", uxTaskGetNumberOfTasks() - 1);
  vTaskDelete(NULL); // free 8k ram, I think it can be commented if we want some actions in the loop
}
