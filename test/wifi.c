/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
// #include <WiFi.h>
// #include <WiFiAP.h>

#include <stdlib.h>
#define HTML_LEN 29

const char** html = {
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



/*
// Replace with your network credentials
const char* ssid     = "IoTOtA-Access-Point";    // Replace with your preffered credentials
const char* password = NULL;
IPAddress   local_IP(192,168,1,1);
IPAddress   gateway(192,168,255,255);
IPAddress   subnet(255,255,255,0);

WiFiServer  server(80);             // Set web server port number to 80
String      header;                 // Variable to store the HTTP request
String      outputLedState = "off"; // Auxiliar variables to store the current output state

uint8_t     ledPin     =  5;
uint8_t     batteryPin = 35;


void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin    , OUTPUT);                         // Initialize the output variables as outputs
  pinMode(batteryPin, INPUT );
  
  digitalWrite(ledPin, HIGH);                          // Set outputs to LOW
 

  Serial.print("Setting AP (Access Point)…");          // Connect to Wi-Fi network with SSID and password
  WiFi.softAPConfig(local_IP, gateway, subnet);    
  WiFi.softAP(ssid, password , 0, false, 1);           // Remove the password parameter, if you want the AP (Access Point) to be open

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("LED on");
              outputLedState = "on";
              digitalWrite(ledPin, LOW);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("LED off");
              outputLedState = "off";
              digitalWrite(ledPin, HIGH);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>IoTOtA Temporrary Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>LED - State " + outputLedState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (outputLedState=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");
            client.println();                         // The HTTP response ends with another blank line
            // Break out of the while loop
            break;
          } else {               // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
*/