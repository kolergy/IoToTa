#define WEB_SERV


//include <Arduino.h>
#include <WiFi.h>    // Load Wi-Fi libraries
#include <WiFiAP.h>
#include <stdlib.h>

#define HTML_LEN 29

#ifndef W_AP_SERV
  #define W_AP_SERV
  class WIFI_AP_Serv {

    public:
      WIFI_AP_Serv(bool debug=false);
      void setup();
      void serve(String** input, int lin, int col, String* output, int nOutput);
 
    private:
      bool       m_debug;
      WiFiClient client;
      IPAddress  IP;
      /*inline*/ static constexpr  char* http_response_code_ok  = "HTTP/1.1 200 OK\r\n";
      /*inline*/ static constexpr  char* http_html_content_type = "Content-type:text/html\r\n";
      /*inline*/ static constexpr  char* http_connection_end    = "Connection: close\r\n\r\n";

  };
#endif // W_AP_SERV

/*
const char** html_chars = {
    "<!DOCTYPE html>",
    "<html lang=\"fr-FR\">",
    "<head>",
    "<meta charset=\"UTF-8\">",
    "<style>",
    "body {font-size: 16px; font-family: sans-serif; text-align: center;}",
    "body form :nth-child(n) {padding: .5em; margin: .5em auto; width: 15em; border-radius: .5em; display: block;}",
    "body form :first-child {margin-top: 0;}",
    "body form :last-child {margin-bottom: 0;}",
    "option[value=placeholder] {color: rgba(0, 0, 0, .5); font-style: italic;}",
    "</style>",
    "<title>IoTOTA - Se connecter</title>",
    "</head>",
    "<body>",
    "<h1>Se connecter à un réseau WIFI</h1>",
    "<p>Pour pouvoir accéder aux fonctionalités de IOT-OTA, veuillez vous connecter à un réseau wifi.</p>",
    "<form action=\"POST\">",
    "<select name=\"ssid\" id=\"ssid\">",
    "<option value=\"placeholder\"> -- SSID -- </option>",
    "<option value=\"x1\">x1</option>",
    "<option value=\"x1\">x1</option>",
    "<option value=\"x1\">x1</option>",
    "<option value=\"x1\">x1</option>",
    "<option value=\"x1\">x1</option>",
    "</select>",
    "<input type=\"password\" name=\"wifi-password\" id=\"wifi-password\" placeholder=\"Mot de passe\">",
    "<button type=\"submit\">Se connecter</button>",
    "</form>",
    "</body></html>"
};
*/
