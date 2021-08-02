#include <Arduino.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <string>


#ifndef CERT_H
  #include "cert.h"
#endif // CERT_H

#ifndef OTA_TOOLS
  #include "OTAtools.h"
#endif // OTA_TOOLS

#ifndef WEB_SERV
  #include "WebServ.h"
#endif // WEB_SERV

// The tools to do OTA for the IoT including an accesspoint & webserver to select wifi ssid & insert password

unsigned long previousMillis   =     0; // will store last time LED was updated
unsigned long previousMillis_2 =     0;
const    long interval         = 30000;
const    long mini_interval    =  1000;

                             //        0                     1                  2               3            4            
static const char *ST_Names[]  = {"Initialisation", "Request Credentials", "WiFi Connect", "WiFi Failed", "Runnng"};
static const int  nST_Names    = 5;
                             //     0      1        2           3            4                5               6          7          8           9     
static const char *WiFiCrypt[] = {"Open", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 Enterprise", "Unknown", "Unknown", "Unknown",  "Unknown"};
static const int  nWiFiCrypt   = 10;

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
  Serial.flush();
  if(m_debug) { 
     if(state<=nST_Names) Serial.printf("\nState%2d:%20s\n", state, ST_Names[state]);
     else Serial.printf("WARNING State%2d: Unknown\n", state);
  }
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
       //scanWifi();
       break;   
   case ST_RUN:
       checkChange_ST_RUN();
       if(state !=ST_RUN) break;
       checkUpdate();
       break;
  }
}

void OTA::checkChangeST_INIT() {                        // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(FlagCredentialOk)  state = ST_WIFI_CONNECT;
  else                  state = ST_REQUEST_CREDENTIALS;
}

void OTA::checkChangeST_REQUEST_CREDENTIALS() {         // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(FlagCredentialOk)  state = ST_INIT;
  else                  state = ST_REQUEST_CREDENTIALS;
}

void OTA::checkChangeST_WIFI_CONNECT() {               // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(WiFi.status() == WL_CONNECTED) state = ST_RUN;
  else                              state = ST_WIFI_FAIL;
}

void OTA::checkChangeST_WIFI_FAIL() {                  // transition from ST_WIFI_CONNECT to: ST_RUN, ST_WIFI_FAIL, //
  if(m_debug) Serial.printf("checkChangeST_WIFI_FAIL connectAttempt: %d / MaxAttempt: %d\n", connectAttempt, MaxAttempt); 
  if(connectAttempt >= MaxAttempt)   state = ST_REQUEST_CREDENTIALS;
  else                              state = ST_WIFI_CONNECT;
}

void OTA::checkChange_ST_RUN() {                       // transition from ST_RUN to: ST_WIFI_FAIL, //
  if(WiFi.status() == WL_CONNECTED) state = ST_RUN;
  else                              state = ST_WIFI_FAIL;
}

void OTA::initialisation() {
  if(m_debug) Serial.printf("Active firmware version: %s, Collecting wifi credentials from NVS memory\n", m_curFwVer);
  NVS.begin();
  getCredentialsFromNVS();
}

void OTA::RequestCredentials() {
  bool resS        = false;
  bool resP        = false;
  int      nResult = 2; // [0]:SSID, [1]:Pass
  String*  result  = new String[nResult];
  result[0]        = "";
  result[1]        = "";
  APInforations newCred;
  newCred.ssid     = "";
  newCred.password = "";
  //if(Serial) {                 // if user is connected via serial request ia serial
  if(false) {                    // test credential request via the wifi AP with serial connected
    if(m_debug) Serial.printf("Serial is availabe\n"); 
    delay(200);
    Serial.println("Requesting the credentials via Serial:");
    while(Serial.available()) Serial.read();     // clear anything on the serial input
    if(newCred.ssid == "") {
      Serial.println("Please enter WiFi SSID:");
      newCred.ssid = String(getInput());
      resS         = NVS.setString("ssid", newCred.ssid);
      if(resS) Serial.println("SSID updated");
    } 

    while(Serial.available()) Serial.read();    // clear anything on the serial input
    if(newCred.password == "") {
      Serial.println("Please enter WiFi password:");
      newCred.password = String(getInput());
      resP             = NVS.setString("password", newCred.password);
      Serial.println(":" + newCred.password + ":");
      if(resP) Serial.println("Password updated");
    }
    if(resS && resP) {
      Serial.printf("Credentials successfuly updated\n"); 
      FlagCredentialOk = true;
    }
  } else {            // Serial is not available seting up a wifi accesspoint to request credentials 
    if(m_debug) Serial.println("Scanning the wifi to generate list for the AP");
    scanWifi();
    int nInp = 2;
    //inputs   = new String[nAP][nInp];
    inputs = new String*[nAP];
    for(int i = 0; i < nAP; ++i) inputs[i] = new String[nInp];

    if(m_debug) Serial.println("Starting the AP");
    WIFI_AP_Serv AP = WIFI_AP_Serv();
    AP.setup();
    do {
      AP.serve(inputs, nAP, nInp, result, nResult);
      //delay(1000);
    } while(result[0] == "" || result[1] == "" );
    newCred.ssid     = result[0];
    newCred.password = result[1];
    resS             = NVS.setString("ssid", newCred.ssid);
    Serial.print(":" + newCred.ssid + ":");
    if(resS) Serial.println(" SSID updated");
    resP             = NVS.setString("password", newCred.password);
    Serial.print(":" + newCred.password + ":");
    if(resP) Serial.println(" Password updated");
    delete [] result;                                  // Cean up the mess
    for(int i = 0; i < nAP; ++i) delete [] inputs[i];  // Cean up the mess
  }
} 

void OTA::checkUpdate() {
  static   int  num           = 0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;                          // save the last time you blinked the LED
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
  //NVS.setString("ssid"    , "");                   // erase SSID & pass from NVS mem
  //NVS.setString("password", "");                   // 
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


void OTA::connect_wifi() {
  int lenS = ssid.length()     + 1; 
  int lenP = password.length() + 1; 
  char ss[lenS];
  char pa[lenP];
  ssid.toCharArray(    ss, lenS);
  password.toCharArray(pa, lenP);
  if(m_debug) Serial.printf("Waiting for WiFi - SSID: %s Pass: %s\n", ss, pa); 

  WiFi.setHostname(m_deviceNam);
  WiFi.begin(ss, pa);
  int n = 0;
  while (WiFi.status() != WL_CONNECTED && n < 10) {
    frqBlink(500, 5);
    if(m_debug) Serial.print(".");
    n++;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    if(m_debug) Serial.printf("\nWiFi successfully connected to: %s after %d retries, local IP: %d.%d.%d.%d, Host Name: %s\n", ss, n, WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3], WiFi.getHostname()); 
    connectAttempt=0;
  } else {
    if(m_debug) Serial.printf("\nWiFi Failed to connect to: %s after %d retries\n", ss, n);
    connectAttempt++; 
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
    if(m_debug) Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    if(m_debug) Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    if(m_debug) Serial.println("HTTP_UPDATE_OK");
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
  if(m_debug) Serial.println(fwurl);
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);

  HTTPClient https;   // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 

  if (https.begin( client, fwurl)) {   // HTTPS      
      if(m_debug) Serial.print("[HTTPS] GET...\n");  // start connection and send HTTP header
    delay(100);
    httpCode = https.GET();
    delay(100);
    if (httpCode == HTTP_CODE_OK) {    // if version received
      payload = https.getString();     // save received version
    } else {
      if(m_debug) { 
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
    }
    https.end();
  }
  client.stop();
      
  if (httpCode == HTTP_CODE_OK) {      // if version received
    payload.trim();
    if (payload.equals(m_curFwVer)) {
        if(m_debug) Serial.printf("\nDevice already running latest firmware version:%s\n", (char*)(m_curFwVer));
      return 0;
    } 
    else {
      if(m_debug) { 
        Serial.println(payload);
        Serial.println("New firmware detected");
      }
      return 1;
    }
  } 
  return 0;  
}

void OTA::scanWifi() {
  // inspired from https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
  //String s;

  WiFi.mode(WIFI_STA);                        // Set WiFi to station mode 
  WiFi.disconnect();                          // disconnect from an AP if it was previously connected
  delay(100); 

  if(m_debug) Serial.println("Scan start");        
  nAP = WiFi.scanNetworks();                  // WiFi.scanNetworks will return the number of networks access points found
  if(m_debug) Serial.println("Scan completed");
  if (nAP == 0) {
    if(m_debug) Serial.println("No networks found"); 
  } else {
    if(m_debug) {
      Serial.printf("Scan identified %d networks\n", nAP);
      if(nAP > MAX_NUMBER_OF_AP) Serial.println("WARNING: more SSID than storage");
    }
    nAP = min(nAP, MAX_NUMBER_OF_AP);
    for (int i = 0; i < nAP; ++i) {           // Print SSID and RSSI for each network found
      APList[i].ssid          = WiFi.SSID(i);
      APList[i].rssi          = WiFi.RSSI(i);
      wifi_auth_mode_t cry = WiFi.encryptionType(i);
      if(cry<=nWiFiCrypt) APList[i].encription = WiFiCrypt[cry];
      else {
        APList[i].encription = "Out of Bounds";
        if(m_debug) Serial.println("WARNING Crypto unknown"); 
      }
      if(m_debug) {
        int lenS = APList[i].ssid.length()     + 1; 
        int lenE = APList[i].encription.length() + 1; 
        char   ss[lenS];
        char   en[lenE];
        APList[i].ssid.toCharArray(ss,lenS);
        APList[i].encription.toCharArray(en,lenE);
        Serial.printf("%3d: %20s [%3d], Crypto:%16s\n", (i + 1), ss, APList[i].rssi, en);
      }
    }
  }
  if(m_debug) Serial.println("");
}

void OTA::frqBlink(int t, float f, float r) {   // blink the builtin led at a given frequency and a certain on ratio
  unsigned long t0 = millis();
  if(f>-0.00001 && f<0.00001) {                 // f=0 led solid on
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

