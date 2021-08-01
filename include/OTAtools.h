#pragma once

#include <ArduinoNvs.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
#endif

#define MAX_NUMBER_OF_SSID   100

#define ST_INIT                0
#define ST_REQUEST_CREDENTIALS 1
#define ST_WIFI_CONNECT        2
#define ST_WIFI_FAIL           3
#define ST_RUN                 4

class OTA {  // A class to handle OTA activities
  public:
    OTA(const char* deviceNam, const char* curFwVer, const char* FirmwrVer, const char* FirmwrBin, bool debug=false);
    void run();

  private:
    void  initialisation(); 
    void  getCredentials();
    void  connect_wifi();
    void  checkUpdate();
    void  firmwareUpdate();
    int   fwVersionCheck();
    void  frqBlink(int t, float f, float r=0.5);
    void  checkChangeST_INIT();
    void  checkChangeST_REQUEST_CREDENTIALS();
    void  checkChangeST_WIFI_CONNECT();
    void  checkChangeST_WIFI_FAIL();
    void  checkChange_ST_RUN();
    char* getInput();
    void  getCredentialsFromNVS();
    void  clearCredentials();
    void  scanWifi();
    void  RequestCredentials();

    const char*  m_deviceNam; 
    const char*  m_verFile;
    const char*  m_firmwrBin;
    const char*  m_curFwVer;
    bool         m_debug;
    bool         FlagCredentialOk;
    int          state;
    int          connectAttempt;
    int          MaxAttempt = 4;
    int          rssiList[   MAX_NUMBER_OF_SSID ];
    const char*  encripType[ MAX_NUMBER_OF_SSID ];
    String       ssidList[   MAX_NUMBER_OF_SSID ];
    String       ssid;
    String       password;


};


