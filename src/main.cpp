#include <Arduino.h>

#include "OTAtools.h"


struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button_boot = {  0,  0,  false };

void IRAM_ATTR isr() {
  button_boot.numberKeyPresses += 1;
  button_boot.pressed = true;
}

void setup() {
  //pinMode(button_boot.PIN, INPUT);
  //attachInterrupt(button_boot.PIN, isr, RISING);
  NVS.begin();
  Serial.begin(9600);
  Serial.print("Active firmware version:");
  //Serial.println(FirmwareVer);
  //NVS.setString("password", "");
  getCredentials();
  delay(200);
  pinMode(LED_BUILTIN, OUTPUT);
  connect_wifi();

}

void loop() {

  delay(2000);

  if (button_boot.pressed) { //to connect wifi via Android esp touch app 
    Serial.println("Firmware update Starting..");
    firmwareUpdate();
    button_boot.pressed = false;
  }
  repeatedCall();
}