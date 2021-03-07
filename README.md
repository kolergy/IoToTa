# IoToTa
IoT with OTA for ESP32
Initially based on https://github.com/programmer131/ESP8266_ESP32_SelfUpdate
- Adapted to be used with PlatformIO
- Adapted to move the heavy lifting in the OATtools library
- Modified to remove the SSID & password from the firmware bin 
  - These will be requested to the user if they are not available 
  - So they are never included in the program file available in Github 
  - The credentials are stored in the long term memory of the ESP32 (NVS)

Update http Status:
- 200: Success
- 301: Link permanantly moved
- 404: File not found
- 408: Timeout
- 418 I'm a teapot (RFC 2324, RFC 7168) (with IoT one never know what can be on the other end)
- ...

Update errors:
WiFi.status() != WL_CONNECTED -> ESP32 memory not partitionned adequately


Global HTTP status list available on Wikipedia:https://en.wikipedia.org/wiki/List_of_HTTP_status_codes

WARNING work ongoing not yet fully tested
