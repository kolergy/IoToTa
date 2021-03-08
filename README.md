# IoToTa
Wifi IoT with GitHub OTA for the ESP32 

The ESP32 will connect to wifi then regularely check for new version of the firmware in GitHub, if successfull will perform an OTA update. 
For security reasons it uses HTTPS & WiFi credentials are not stored in the firmware.

Initially based on https://github.com/programmer131/ESP8266_ESP32_SelfUpdate
- Adapted to be used with PlatformIO
- Adapted to move the heavy lifting in the OATtools library & refactored into the OTA class
- Modified to remove the SSID & password from the firmware bin 
  - The credentials are stored in the long term memory of the ESP32 (NVS)
  - These will be requested through Serial connection if they are not available 
  - So they are never included in the program file available in Github 

## Usage
### Board set-up for OTA
- Partition your board for OAT, this can be done with the Arduino IDE:
  - In tools setup your board & port
  - In tools select the partition scheme: 'minimal SPIFFS (larrge APPS with OTA)'  
  - Upload any sketch like blink & your board will be partitionned

### Code
- Add: '#include "OTAtools.h"' at the start of your sketch
- add: OTA object 'OTA ota = OTA(DEVICENAME, FirmwrVer, FirmwrBin, true);' with your settings
- Add: call 'ota.set();' in your 'setup()' function
- Add: call 'ota.check();' in your 'loop()' function

## Errors
### Update http Status:
- 200: Success
- 301: Link permanantly moved
- 404: File not found
- 408: Timeout
- 418 I'm a teapot (RFC 2324, RFC 7168) (with IoT one never know what can be on the other end)
- ...
- Global HTTP status list available on Wikipedia:https://en.wikipedia.org/wiki/List_of_HTTP_status_codes


### Update errors:
- WiFi.status() != WL_CONNECTED -> ESP32 memory not partitionned adequately

## WARNING active work ongoing not yet fully stabilised & tested
