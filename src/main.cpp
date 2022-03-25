//#include <Arduino.h>
#include <SHT31.h>  // by Rob Tillaart

#ifndef OTA_TOOLS
  #include "OTAtools.h"
#endif // OTA_TOOLS

#ifndef DATA_MANAGER
  #include "DataManager.h"
#endif // DATA_MANAGER

#ifndef I2C_MANAGER
  #include "I2CManager.h"
#endif // I2C_MANAGER

// Settings for the OTA
#define DEVICENAME "IoToTa_01"
#define FWVERSION  "0.3"
#define FwVerTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/bin_version.txt"
#define FwBinTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/firmware.bin"


String topic       = "IoToTa/01/data";
String server_IP   = "192.168.1.1";
int   server_port = 1883;
bool  debug       = true;
bool  signature   = false;
bool  numbering   = true;

#define SHT31_ADDRESS   0x44

#define DEBUG_NTPClient

SHT31       sht30;

OTA         ota   = OTA(DEVICENAME, FWVERSION, FwVerTgt, FwBinTgt, true);  // Creation of theOTA object
//DataManager dataM = DataManager(topic, server_IP, server_port, debug, signature, numbering);

TO         nto   = TO();

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
  I2CScan();
  sht30.begin(SHT31_ADDRESS);
  nto.connect_to_NTP();
  //dataM.create_MQTT_client();
}
 
void loop() {

  ota.run();                   // do the OTA check routine 

  char buffer [33];
  sht30.read(false);
  int r_millis = millis();
  itoa (sht30.readStatus(),buffer,2);
  Serial.printf("VBat  :%6.3fV\n" , (ReadVoltage(35) * 2.0));
  Serial.printf("age   :%d ms\n"  , sht30.lastRead());
  Serial.printf("mils  :%d ms\n"  , r_millis);
  Serial.printf("Temp  :%6.3f°C\n", sht30.getTemperature());
  Serial.printf("RHum  :%6.2f%%\n", sht30.getHumidity());

  //dataM.MQTT_Send(topic, dataJ)
  delay(5000);
  //status:1000000000010000
}
