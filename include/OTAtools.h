#pragma once

#include <ArduinoNvs.h>



static String ssid        = "";
static String password    = "";
static String FirmwareVer = {  "0.1" };

void setOTA(bool debug=false);
void getCredentials();
void connect_wifi();
void checkOTA(bool debug=false);
void firmwareUpdate();
int  FirmwareVersionCheck();



