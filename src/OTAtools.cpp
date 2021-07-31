#include <Arduino.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <string>

#include "OTAtools.h"
#include "cert.h"

// the tools to do OTA

unsigned long previousMillis   =     0; // will store last time LED was updated
unsigned long previousMillis_2 =     0;
const    long interval         = 30000;
const    long mini_interval    =  1000;

OTA::OTA(const char* deviceNam, const char* curFwVer, const char* verFile, const char* firmwrBin, bool debug) {
  m_deviceNam      = deviceNam;
  m_verFile        = verFile;
  m_firmwrBin      = firmwrBin;
  m_debug          = debug;
  m_curFwVer       = curFwVer;
  state            = ST_INIT;
  FlagCredentialOk = false;
  ssid             = "";
  password         = "";
  connectAttempt   = 0;
}

void OTA::run(void) {  // run a kind of mini state machine
  Serial.print("Running State:");
  Serial.println(state);
  switch (state) {
   case ST_INIT:
       initialisation();
       checkChangeST_INIT();
       break;
   case ST_REQUEST_CREDENTIALS:
       RequestCredentials();
       checkChangeST_REQUEST_CREDENTIALS();
       break;
   case ST_WIFI_CONNECT:
       connect_wifi();
       checkChangeST_WIFI_CONNECT();
       break;
   case ST_WIFI_FAIL:
       checkChangeST_WIFI_FAIL();
       scanWifi();
       break;   
   case ST_RUN:
       checkChange_ST_RUN();
       if(state !=ST_RUN) break;
       checkUpdate();
       break;
  }
}

void OTA::checkChangeST_INIT() {  // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(FlagCredentialOk)  state = ST_WIFI_CONNECT;
  else                  state = ST_REQUEST_CREDENTIALS;
}

void OTA::checkChangeST_REQUEST_CREDENTIALS() {  // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(FlagCredentialOk)  state = ST_WIFI_CONNECT;
  else                  state = ST_REQUEST_CREDENTIALS;
}

void OTA::checkChangeST_WIFI_CONNECT() {  // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(WiFi.status() == WL_CONNECTED) state = ST_RUN;
  else                              state = ST_WIFI_FAIL;
}

void OTA::checkChangeST_WIFI_FAIL() {  // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(connectAttempt > MaxAttempt)   state = ST_REQUEST_CREDENTIALS;
  else                              state = ST_WIFI_CONNECT;
}

void OTA::checkChange_ST_RUN() { // transition from ST_RUN to: ST_WIFI_FAIL, //
  if(WiFi.status() == WL_CONNECTED) state = ST_RUN;
  else                              state = ST_WIFI_FAIL;
}

void OTA::initialisation() {
  if(m_debug) {  
    Serial.print("Active firmware version:");
    Serial.println(m_curFwVer);
    Serial.print("Collecting wifi credentials from NVS memory");
  }
  NVS.begin();
  getCredentialsFromNVS();
}

void OTA::RequestCredentials() {
  scanWifi();

}



void OTA::checkUpdate() {
  static   int  num           = 0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;  // save the last time you blinked the LED
    if (fwVersionCheck()) firmwareUpdate();
  }
  if ((currentMillis - previousMillis_2) >= mini_interval) {
    previousMillis_2 = currentMillis;
    if(m_debug){
      Serial.print("idle loop...");
      Serial.print(num++);
      Serial.print(" Active fw version:");
      Serial.println(m_curFwVer);
    }
    if(WiFi.status() == WL_CONNECTED) {
      if(m_debug) Serial.println("wifi connected");
      else connect_wifi();
    }
  }
}

char* OTA::getInput() {               // Get user input from Serial
  char        buf[256];
  static char rtrn[256];
  boolean     newD     = false;
  byte ndx             = 0;
  char        endMark  = '\n';
  char rc;
  //for(int i=0;i<255;i++) buf[i] = 0;
  //Serial.println(buf);
  while (newD == false && ndx < 255) {
    if(Serial.available() > 0) {
      rc = Serial.read();
      if (rc != endMark && int(rc) !=13) {
        buf[ndx] = rc;
        Serial.println(buf[ndx]);
        //Serial.println(buf);
        ndx++;
      } else {
        buf[ndx] = '\0'; // terminate the string
        newD     = true;
      }
    } else frqBlink(500, 1, 0.2);
  }
  Serial.println("");
  Serial.println(":" + String(buf) + ":");
  strncpy(rtrn, buf, 255);
  return rtrn;
}

void OTA::getCredentialsFromNVS() {  // Get credentials from NVS memory if available or request them through serial
  ssid     = NVS.getString("ssid"    );
  password = NVS.getString("password");
  if(ssid != "") FlagCredentialOk = true;
  else           FlagCredentialOk = false;
  if(m_debug) {  
    Serial.println("Get Credentials:");
    Serial.println("ssid    :" + ssid    +":");
    Serial.println("password:" + password+":");
  }
}

void OTA::clearCredentials() {
  NVS.setString("ssid"    , "");                   // erase SSID & pass from NVS mem
  NVS.setString("password", "");                   // 
  ssid     = "";
  password = "";
  FlagCredentialOk = false;
  if(m_debug) {  
    Serial.println("Credentials CLEARED!");
  }
}

void OTA::getCredentials() {  // Get credentials from NVS memory if available or request them through serial
  bool res = false;
  delay(200);
  while(Serial.available()) Serial.read();
  Serial.println("GetCredentials:");
  //NVS.setString("ssid"    , "");                 // uncoment these to erase SSID & pass from NVS mem
  //NVS.setString("password", "");                 // 
  String ssid     = NVS.getString("ssid"    );
  String password = NVS.getString("password");
  Serial.println("ssid    :" + ssid    +":");
  Serial.println("password:" + password+":");

  if(ssid == "") {
    Serial.println("Please enter WiFi SSID:");
    ssid = String(getInput());
    res  = NVS.setString("ssid", ssid);
    if(res) Serial.println("SSID updated");
  } 
  else frqBlink(200, 10, 0.2);

  while(Serial.available()) Serial.read();
  if(password == "") {
    Serial.println("Please enter WiFi password:");
    password = String(getInput());
    Serial.println(":" + password + ":");
    res      = NVS.setString("password", password);
    if(res) Serial.println("Password updated");
  } 
}

void OTA::connect_wifi() {
  String ssid     = NVS.getString("ssid"    );
  String password = NVS.getString("password");
  int lenS = ssid.length()     + 1; 
  int lenP = password.length() + 1; 
  char ss[lenS];
  char pa[lenP];
  ssid.toCharArray(    ss, lenS);
  password.toCharArray(pa, lenP);
  Serial.println("Waiting for WiFi");
  Serial.println(ss);
  Serial.println(pa);
  WiFi.setHostname("IoToTa_01");
  WiFi.begin(ss, pa);
  int n = 0;
  while (WiFi.status() != WL_CONNECTED && n < 10) {
    frqBlink(500, 5);
    Serial.print(".");
    n++;
  }
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address & host name: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.getHostname());
  }
}

void OTA::firmwareUpdate(void) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  
  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, m_firmwrBin);
  client.stop();

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}

int OTA::fwVersionCheck(void) {
  String payload;
  int    httpCode = 0;
  String fwurl    = "";
  fwurl          += m_verFile;
  fwurl          += "?";
  fwurl          += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);

  // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
  HTTPClient https;

  if (https.begin( client, fwurl)) {   // HTTPS      
    Serial.print("[HTTPS] GET...\n");  // start connection and send HTTP header
    delay(100);
    httpCode = https.GET();
    delay(100);
    if (httpCode == HTTP_CODE_OK) {    // if version received
      payload = https.getString();     // save received version
    } else {
      Serial.print("error in downloading version file:");
      Serial.println(httpCode);
    }
    https.end();
  }
  client.stop();
      
  if (httpCode == HTTP_CODE_OK) {      // if version received
    payload.trim();
    if (payload.equals(m_curFwVer)) {
      Serial.printf("\nDevice already running latest firmware version:%s\n", (char*)(m_curFwVer));
      return 0;
    } 
    else {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  } 
  return 0;  
}
void OTA::scanWifi() {
  // adapted from https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

   
    Serial.println("scan start");         // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)  Serial.println("no networks found"); 
    else {
        Serial.print(n);
        Serial.println(" networks found");
        if(n > MAX_NUMBER_OF_SSID) Serial.println("WARNING: more SSID than storage");
        n = min(n,MAX_NUMBER_OF_SSID);
        for (int i = 0; i < n; ++i) {     // Print SSID and RSSI for each network found
            //Serial.printf("%d: %s (%d)", (i + 1), WiFi.SSID(i), WiFi.RSSI(i));
            ssidList[i] = WiFi.SSID(i);
            rssiList[i] = WiFi.RSSI(i);
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(ssidList[i]);
            Serial.print(" (");
            Serial.print(rssiList[i]);
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

}


void OTA::frqBlink(int t, float f, float r) {   // blink the builtin led at a given frequency and a certain on ratio
  unsigned long t0 = millis();
  if(f>-0.00001 && f<0.00001) {                 // f=0 les solid on
    digitalWrite(LED_BUILTIN, LOW);
    while(millis()-t0 < t) yield(); 
  }   
  if(f<0) {                                     // f<0 led solid off
    digitalWrite(LED_BUILTIN, HIGH );
    while(millis()-t0 < t) yield(); 
  }   
  else {                                        // f>0 led blinking at freq f
    float         p     = 1/f;                  // half period of the blink
    long          tt[2] = { long(1000*p*r), long(1000*p*(1-r)) };
    bool          st    = true;
    unsigned long tm1   = millis();
    while(millis()-t0 < t) {
      unsigned long tn = millis();
      if(tn-tm1> tt[st]) {
        tm1 = tn;
        st  = !st;
        digitalWrite(LED_BUILTIN, st*HIGH);
      }
    }
  }
}

