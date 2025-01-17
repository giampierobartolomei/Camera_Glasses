# Camera_Glass
17/01/25

List of Components:

- Xiao ESP32S3 Sense Module [Look Here for first use](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- FAT 32 formatted microSD (copy data folder in it before first installation)
- Lipo Battery LP502030
- 3D printed case (.gcode ready for print, or downlad .stl to do some changes)

- Firmware: runs in Arduino Espressif core 3.1.1 [Installation Guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)

adaption from ESP32-CAM_MJPEG2SD repo, look at their ReadMe (in ESP32-CAM_MJPEG2SD folder) for further information. Added Serial Command Libraries (not used, they was a little test for starting/stopping rec from serial commands) and BT Libraries. Just send to Charatteristic "rec" or "stp" string to start/stop rec.
Open the project in Arduiino uno and once connected the device, select XIAO_ESP32S3 with the relative usb-c port.
Then click on Tools -> PSRAM -> "OPI_PSRAM" and Tools -> Partition Scheme -> 3 MB APP.
If you have some trouble to upload code (check with blink Example from Arduino) disconnect the device, press and hold Boot button and while doing it reconnect the device to the computer and finally release it.

On first installation, the application will start in wifi AP mode - connect to SSID: **ESP-CAM_MJPEG_...**, to allow router to be selected and router password entered via the web page on `192.168.4.1`. The configuration data file (except passwords) is automatically created, and the application web pages automatically downloaded from GitHub to the SD card **/data** folder when an internet connection is available. Now you can read the new IP address that you can use from the wifi network that you are using. At this point the interface allows you without modify the code to: decide Video Settings, DISABLE MOTION DETECT!! (this setting autorecord when recognize a movement, disable it in first use), Start a stream to check the video recording, start manually the recording and finally have acces to SD memory in order to see or delete files. Now Echo App can connect to this device and start/stop the recording simultaneously to the toy data.

- Eye Contact Video Recording Settings: SVGA (600X800) 20 fps. then save the .avi file from sd and run this command in terminal: 

'ffmpeg -i input.avi -c:v libx264 -crf 23 -preset medium -c:a aac -b:a 128k output.mp4'

now the video is ready for the Computer Vision Program.



