#pragma once

#include <ArduinoNvs.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
#endif

class OTA {  // A class to handle OTA activities
  public:
    OTA(char* deviceNam, char* FirmwrVer, char* FirmwrBin, bool debug=false);
    void set(char* curFwVer); 
    void getCredentials();
    void connect_wifi();
    void check();
    void firmwareUpdate();
    int  fwVersionCheck();
    void frqBlink(int t, float f, float r=0.5);

  private:
    char* m_deviceNam; 
    char* m_verFile;
    char* m_firmwrBin;
    char* m_curFwVer;
    bool  m_debug;
    char* getInput();
};


