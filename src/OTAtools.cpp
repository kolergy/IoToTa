#include <Arduino.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

#include "OTAtools.h"
#include "cert.h"

// the tools to do OTA

unsigned long previousMillis   = 0; // will store last time LED was updated
unsigned long previousMillis_2 = 0;
const    long interval         = 30000;
const    long mini_interval    = 1000;

OTA::OTA(char* deviceNam, char* verFile, char* firmwrBin, bool debug) {
  m_deviceNam = deviceNam;
  m_verFile   = verFile;
  m_firmwrBin = firmwrBin;
  m_debug     = debug;

}

void OTA::set(char* curFwVer) {
  m_curFwVer = curFwVer;
  //void setOTA(bool debug) {
  if(m_debug) {  
    Serial.print("Active firmware version:");
    Serial.println(curFwVer);
  }
  NVS.begin();
  getCredentials();
  delay(200);
  connect_wifi();
}


void OTA::check() {
  static   int  num           = 0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;  // save the last time you blinked the LED
    if (fwVersionCheck()) {
      firmwareUpdate();
    }
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
      if(m_debug){
        Serial.println("wifi connected");
      }
    } else {
      connect_wifi();
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
void OTA::frqBlink(int t, float f, float r) {   // blink the builtin led at a given frequency and a certain on ratio
  unsigned long t0 = millis();
  if(f>-0.00001 && f<0.00001) {         // f=0 les solid on
    digitalWrite(LED_BUILTIN, LOW);
    while(millis()-t0 < t) yield(); 
  }   
  if(f<0) {                             // f<0 led solid off
    digitalWrite(LED_BUILTIN, HIGH );
    while(millis()-t0 < t) yield(); 
  }   
  else {                                // f>0 led blinking at freq f
    float         p    = 1/f;                  // half period of the blink
    long          tt[2] = { long(1000*p*r), long(1000*p*(1-r)) };
    bool          st   = true;
    unsigned long tm1  = millis();
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

