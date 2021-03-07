#include <Arduino.h>

#include "OTAtools.h"

void setup() {
  Serial.begin(9600);

  //NVS.setString("password", "");

  pinMode(LED_BUILTIN, OUTPUT);
  setOTA(true); 

}

void loop() {

  delay(2000);

  checkOTA(true);
}