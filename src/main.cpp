#include <Arduino.h>

#ifndef CONFIGURATION
  #include "Configuration.h"
#endif // CONFIGURATION

#ifndef CREDENTIALS_H
  #include "credentials.h"
#endif // CREDENTIALS_H

#ifndef OTA_TOOLS
  #include "OTAtools.h"
#endif // OTA_TOOLS

#ifndef I2C_MANAGER
  #include "I2CManager.h"
#endif // I2C_MANAGER

#ifndef DATA_ACQUISITION
  #include "DataAcquisition.h"
#endif // DATA_ACQUISITION

#ifndef DATA_MANAGER
  #include "DataManager.h"
#endif // DATA_MANAGER

#include <SHT31.h>  // by Rob Tillaart

const char* mqtt_topic_temp_C   = "IoToTa/LVR/temp_C";
const char* mqtt_topic_RH_pct   = "IoToTa/LVR/Rel_hum_pct";
const char* mqtt_topic_P_amb_pa = "IoToTa/LVR/P_amb_Pa";
const char* mqtt_topic_V_bat_V  = "IoToTa/LVR/V_bat_V";

bool  debug       = true;
bool  signature   = false;
bool  numbering   = true;

OTA             ota   = OTA(DEVICENAME, FWVERSION, FwVerTgt, FwBinTgt, true);  // Creation of theOTA object
//TO              nto   = TO();
DataAcquisition DA    = DataAcquisition();
DataManager     dataM = DataManager(mqtt_server, mqtt_port, mqtt_user, mqtt_password, debug, signature, numbering);
SHT31           sht30;

void setup() {
  //analogSetClockDiv(255); // 1338mS  for acurate ADC
  //adcAttachPin(35);
  Serial.begin(115200);
  Serial.flush();
  Serial.println("Welcome to IOTOTA!!!");
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
  //nto.connect_to_NTP();
  //dataM.create_MQTT_client();

  #ifdef PA_TOURNER
    #ifdef PLATFORMIO
      Serial.println("def PLATFORMIO");
    #endif // PLATFORMIO

    #ifdef ARDUINO_LOLIN_D32
      Serial.println("def ARDUINO_LOLIN_D32");
    #endif // ARDUINO_LOLIN_D32

    #ifdef HAVE_CONFIG_H
      Serial.println("def HAVE_CONFIG_H");
    #endif // HAVE_CONFIG_H

    #ifdef MBEDTLS_CONFIG_FILE
      Serial.println("def MBEDTLS_CONFIG_FILE");
    #endif // MBEDTLS_CONFIG_FILE

    #ifdef UNITY_INCLUDE_CONFIG_H
      Serial.println("def UNITY_INCLUDE_CONFIG_H");
    #endif // UNITY_INCLUDE_CONFIG_H

    #ifdef ESP32
      Serial.println("def ESP32");
    #endif // ESP32

    #ifdef F_CPU
      Serial.println("def F_CPU");
    #endif // ESP_PLATFORM

    #ifdef ESP_PLATFORM
      Serial.println("def ESP_PLATFORM");
    #endif // ESP_PLATFORM

    #ifdef ARDUINO_BOARD
      Serial.print("def ARDUINO_BOARD: ");
      Serial.println(ARDUINO_BOARD);
    #endif // ARDUINO_BOARD
  #endif // PA_TOURNER
}
 
void loop() {

  ota.run();                   // do the OTA check routine 

  char buffer [33];
  //sht30.read(false);
  //int r_millis = millis();
  //itoa (sht30.readStatus(),buffer,2);
  //Serial.printf("VBat  :%6.3fV\n" , (DA.ReadVoltage(35) * 2.0));
  //Serial.printf("age   :%d ms\n"  , sht30.lastRead());
  //Serial.printf("mils  :%d ms\n"  , r_millis);
  //Serial.printf("Temp  :%6.3f°C\n", sht30.getTemperature());
  //Serial.printf("RHum  :%6.2f%%\n", sht30.getHumidity());
  //DA.display_espD32_pins();
  //DA.display_esp32_pins();
  //DA.read_sensors();
  sht30.read(true);         // default = true/fast       slow = false
  float temp_c = sht30.getTemperature();
  Serial.printf("Temperature: %f °C\n", temp_c);
  float rh_pct = sht30.getHumidity();
  Serial.printf("Humidity: %f %%\n", rh_pct);

  dataM.pushData(mqtt_topic_temp_C, String(temp_c));
  dataM.pushData(mqtt_topic_RH_pct, String(rh_pct));
  //dataM.MQTT_Send(topic, dataJ)
  delay(5000);
  //status:1000000000010000
}
