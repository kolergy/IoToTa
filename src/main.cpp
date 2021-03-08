#include <Arduino.h>

#include "OTAtools.h"

// Settings for the OTA
#define DEVICENAME "IoToTa_01"
#define FWVERSION  "0.1"
#define FirmwrVer  "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/bin_version.txt"
#define FirmwrBin  "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/firmware.bin"

OTA ota = OTA(DEVICENAME, FirmwrVer, FirmwrBin, true);  // Creation of theOTA object

void setup() {
  Serial.begin(9600);

  //NVS.setString("password", "");

  pinMode(LED_BUILTIN, OUTPUT);
  ota.set(FWVERSION);            // Set of the OTA system request through Serial SSID & PS if necessary then connect to WiFi 

}

void loop() {

  delay(2000);

  ota.check();                   // do the OTA check routine
}