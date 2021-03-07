#ifndef OATtools_H
#define OATtools_H


#include <Arduino.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <ArduinoNvs.h>



static String ssid        = "";
static String password    = "";

void getCredentials();
void connect_wifi();
void repeatedCall();
void firmwareUpdate();
int  FirmwareVersionCheck();



#endif