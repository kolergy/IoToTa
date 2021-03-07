#include <Arduino.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <ArduinoNvs.h>

#include "cert.h"

String FirmwareVer = {  "0.3" };
static String ssid        = "";
static String password    = "";
#define URL_fw_Version "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/bin_version.txt"
#define URL_fw_Bin     "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/firmware.bin"

void getCredentials();
void connect_wifi();
void firmwareUpdate();
int  FirmwareVersionCheck();

unsigned long previousMillis   = 0; // will store last time LED was updated
unsigned long previousMillis_2 = 0;
const    long interval         = 60000;
const    long mini_interval    = 1000;

void repeatedCall() {
  static   int  num           = 0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;  // save the last time you blinked the LED
    if (FirmwareVersionCheck()) {
      firmwareUpdate();
    }
  }
  if ((currentMillis - previousMillis_2) >= mini_interval) {
    previousMillis_2 = currentMillis;
    Serial.print("idle loop...");
    Serial.print(num++);
    Serial.print(" Active fw version:");
    Serial.println(FirmwareVer);
   if(WiFi.status() == WL_CONNECTED) {
       Serial.println("wifi connected");
   }
   else {
    connect_wifi();
   }
  }
}

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
  Serial.println(FirmwareVer);
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
char* getInput() {
  static char        buf[256];
  boolean     newD     = false;
  static byte ndx      = 0;
  char        endMark  = '\n';
  char rc;
  while (newD == false) {
    if(Serial.available() > 0) {
      rc = Serial.read();
      if (rc != endMark && int(rc) !=13) {
        buf[ndx] = rc;
        Serial.print(rc);
        ndx++;
        if (ndx >= 256) ndx = 256 - 1;
      } else {
        buf[ndx] = '\0'; // terminate the string
        newD     = true;
      }
    }
  }
  Serial.println("");
  Serial.println(":" + String(buf) + ":");
  return buf;
}

void getCredentials() {
  bool res = false;
  Serial.println("GetCredentials:");
  ssid     = NVS.getString("ssid"    );
  password = NVS.getString("password");
  Serial.println("ssid    :" + ssid    +":");
  Serial.println("password:" + password+":");

  if(ssid == "") {
    Serial.println("Please enter WiFi SSID:");
    ssid = String(getInput());
    res  = NVS.setString("ssid", ssid);
    if(res) Serial.println("SSID updated");
  } 
  Serial.flush();
  delay(200);
  if(password == "") {
    Serial.println("Please enter WiFi password:");
    password = String(getInput());
    Serial.println(":" + password + ":");
    res      = NVS.setString("password", password);
    if(res) Serial.println("Password updated");
  } 
}

void connect_wifi() {
  int lenS = ssid.length()     + 1; 
  int lenP = password.length() + 1; 
  char ss[lenS];
  char pa[lenP];
  ssid.toCharArray(    ss, lenS);
  password.toCharArray(pa, lenP);
  Serial.println("Waiting for WiFi");
  Serial.println(ss);
  Serial.println(pa);
  WiFi.begin(ss, pa);
  int n = 0;
  while (WiFi.status() != WL_CONNECTED && n < 10) {
    delay(500);
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void firmwareUpdate(void) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

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

int FirmwareVersionCheck(void) {
  String payload;
  int    httpCode = 0;
  String fwurl    = "";
  fwurl          += URL_fw_Version;
  fwurl          += "?";
  fwurl          += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;
 

  if (client) 
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin( * client, fwurl)) { // HTTPS      
      Serial.print("[HTTPS] GET...\n");  // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) {  // if version received
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }
      
  if (httpCode == HTTP_CODE_OK) {  // if version received
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already running latest firmware version:%s\n", FirmwareVer);
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