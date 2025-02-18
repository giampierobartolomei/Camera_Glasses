/*
* Capture ESP32 Cam JPEG images into a AVI file and store on SD
* AVI files stored on the SD card can also be selected and streamed to a browser as MJPEG.
*
* s60sc 2020 - 2024

8/02: I tried to add prints on serial for battery voltage status, by modifyng this code, appGlobals.h, globals.h, pheriperals.cpp
Then I try to add WebDav functionalities. I modified also utils.cpp by putting to false use ping and setting ap ip and name.
12/02: I defined as const useMotion and commented all others declarations - it works, no more autorecording
17/02: Now battery voltage is sent on command char every ten seconds (uncomment the serial prints to read in serial monitor) in file BluetoothCamerServer.cpp. try using nRF connect iPhone app (UTF-8)
18/02: i changed max_adc values since it can change with device changing, you can see the comment in bt lib .cpp. i added also a factor for correcting R tollerance.I also modified mjpeg2sd in order to blink orange LED during recording.
*/

#include "appGlobals.h"
//#include "SerialCommandHandler.h" //added by me
#include "BluetoothCameraServer.h" //Added by me, from max code

bool forceRecord = false; // added by me and commented the declaratiion in mjpeg2sd.cpp
//bool useMotion  == false; // whether to use camera for motion detection (with motionDetect.cpp), added by me, but only with changs on motionDetect.cpp it works
const bool useMotion = false; // Variabile costante, non modificabile

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
//It should appear: first battery level voltage(commented), pheripherals setup, total tasks and then the updating of the battery level every interval setted in bt library file .cpp (commented)
void loop() {
  // confirm not blocked in setup
  //float voltage = (float)(smoothAnalog(5)) * 3.3 * 2 / 8191;
  //LOG_INF("Initial battery voltage: %.2fV", voltage);
  LOG_INF("=============== Total tasks: %u ===============\n", uxTaskGetNumberOfTasks() - 1);
  vTaskDelete(NULL); // free 8k ram
}
