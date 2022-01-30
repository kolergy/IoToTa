//#include <Arduino.h>


#ifndef OTA_TOOLS
  #include "OTAtools.h"
#endif // OTA_TOOLS

#ifndef DATA_MANAGER
  #include "DataManager.h"
#endif // DATA_MANAGER

// Settings for the OTA
#define DEVICENAME "IoToTa_01"
#define FWVERSION  "0.1"
#define FwVerTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/bin_version.txt"
#define FwBinTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/firmware.bin"



OTA ota = OTA(DEVICENAME, FWVERSION, FwVerTgt, FwBinTgt, true);  // Creation of theOTA object

double ReadVoltage(byte pin){                   // From: https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino
  double reading = analogRead(pin);             // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return 0;
  //return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}                                               // Added an improved polynomial, use either, comment out as required


void setup() {
  analogSetClockDiv(255); // 1338mS  for acurate ADC
  adcAttachPin(35);
  Serial.begin(9600);
  Serial.flush();
  pinMode(LED_BUILTIN, OUTPUT);
  //for (int i = 0; i < 50; ++i) Serial.printf("Pin %2d :%6.3fV\n", i, ReadVoltage(i));
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
  Serial.printf("VBat  :%6.3fV\n", (ReadVoltage(35) * 2.0));
  delay(1000);
  

}
