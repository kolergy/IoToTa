#define OTA_TOOLS

#include <ArduinoNvs.h>
#include <vector>
#include <WiFi.h>          // Load Wi-Fi libraries
#include <string>
#include <NTPClient.h>     // by Fabrice Weinberg
#include <TimeLib.h>       // by Paul Stoffregen  // by Michael Margolis
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>


//#include <cstdlib>
 
#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
#endif // LED_BUILTIN

#define MAX_NUMBER_OF_AP      50  // Maximum number of access points

#define ST_RUN                 0
#define ST_WIFI_CONNECT        1
#define ST_INIT                2
#define ST_WIFI_FAIL           3
#define ST_REQUEST_CREDENTIALS 4

#ifndef S_AP_INFO
  #define S_AP_INFO
  struct APInforations {  // The informations regardng an Access Point
    int      rssi;        // The signal level of the AP
    String   ssid;        // The name of the wifi network AP
    String   password;    // The password of the AP 
    String   encription;  // The type of encription of the AP
  };
#endif // S_AP_INFO




#ifndef My_Tools
  #define My_Tools
  int    str_in_array(String, std::vector<String>);
#endif // My_Tools

#ifndef N_T_O 
  #define N_T_O    // Network time object
  /*************************** Definitions **************************************/
  /* Definition des annees: il y a une annee NON bisextile !
        quand l'annee est superieure a zero
                                   |  sauf les multiples de quatres ans
                                   |         |  et tous les sciecles
                                   |         |         |  sauf tous les 4 sciecles
                                   |         |         |           |            */
  #define ANNEE_NON_BISEXTILE(Y) ((Y>0) && !(Y%4) && ((Y%100) || !(Y%400) ) )

  class TO { // Network time object
    public:
      TO();
      void   connect_to_NTP();
      String getFormattedDate();
      String getFormattedTime();
      time_t t_0;                  // The time of the first connection to the NTO server
      float  dt_NTP_s;             // The time to acquire the ntp time
      
    private:
      WiFiUDP           ntpUDP;                 
      NTPClient         timeClient = NTPClient(ntpUDP, "europe.pool.ntp.org", 3600*2, 60000);
  };
  

#endif // N_T_O

#ifndef O_T_A
  #define O_T_A
  class OTA {  // A class to handle OTA activities
    public:
      OTA(const char* deviceNam, const char* curFwVer, const char* FirmwrVer, const char* FirmwrBin, bool debug=false);
      void run();

    private:
      void  initialisation(); 
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
      void  scanWifi(String** inputs, int lin, int col);
      void  RequestCredentials();

      const char*  m_deviceNam; 
      const char*  m_verFile;
      const char*  m_firmwrBin;
      const char*  m_curFwVer;
      bool         m_debug;
      bool         FlagCredentialOk;
      int          state;
      int          connectAttempt;
      int          MaxAttempt = 1;
      int          nAP;
      //int          rssiList[   MAX_NUMBER_OF_AP ];
      //const char*  encripType[ MAX_NUMBER_OF_AP ];
      //String       ssidList[   MAX_NUMBER_OF_AP ];
      String**     inputs;
      String       ssid;
      String       password;
      APInforations APList[MAX_NUMBER_OF_AP];
      
  };
#endif // O_T_A

