#include <Arduino.h>
//#include <PubSubClient.h>

#include "OTAtools.h"

// Settings for the OTA
#define DEVICENAME "IoToTa_01"
#define FWVERSION  "0.1"
#define FwVerTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/bin_version.txt"
#define FwBinTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/firmware.bin"

//const char* mqtt_server = "192.168.1.21";


OTA ota = OTA(DEVICENAME, FWVERSION, FwVerTgt, FwBinTgt, true);  // Creation of theOTA object

double ReadVoltage(byte pin){  // From: https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required



void setup() {
  Serial.begin(9600);
  Serial.flush();
  pinMode(LED_BUILTIN, OUTPUT);
  ota.run();            // Set of the OTA system request through Serial SSID & PS if necessary then connect to WiFi 
  //PubSubClient client(WiFi);
  //long lastMsg = 0;
  //char msg[50];
  //int value = 0;
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {
  ota.run();                   // do the OTA check routine
  Serial.println(ReadVoltage(36),3);
  Serial.println(ReadVoltage(35),3);
  //Serial.println(analogRead(36));
  delay(1000);
  

}