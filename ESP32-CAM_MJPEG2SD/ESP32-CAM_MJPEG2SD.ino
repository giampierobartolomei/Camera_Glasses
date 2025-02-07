/*
* Capture ESP32 Cam JPEG images into a AVI file and store on SD
* AVI files stored on the SD card can also be selected and streamed to a browser as MJPEG.
*
* s60sc 2020 - 2024
*/

#include "appGlobals.h"
//#include "SerialCommandHandler.h" //added by me
#include "BluetoothCameraServer.h" //Added by me, from max code

bool forceRecord = false; // added by me and commented the declaratiion in mjpeg2sd.cpp
bool useMotion  = false; // whether to use camera for motion detection (with motionDetect.cpp)

void setup() {
  logSetup();
  LOG_INF("Selected board %s", CAM_BOARD);
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

void loop() {
  // confirm not blocked in setup
  LOG_INF("=============== Total tasks: %u ===============\n", uxTaskGetNumberOfTasks() - 1);
  float currentVoltage_print = (float)(smoothAnalog(18)) * 3.3 * 2 / 4095;
  LOG_INF("Battery Voltage: ", smoothAnalog(4));
  delay(1000);
  vTaskDelete(NULL); // free 8k ram
}
