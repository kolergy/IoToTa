
#ifndef WEB_SERV
  #include "WebServ.h"
#endif // WEB_SERV

const char* ssid     = "IoTOtA-Access-Point";          // Replace with your preffered credentials
const char* password = NULL;

IPAddress   local_IP(192,168,  1,  1);
IPAddress   gateway( 192,168,255,255);
IPAddress   subnet(  255,255,255,  0);

WiFiServer  server(80);                                // Set web server port number to 80
String      header;                                    // Variable to store the HTTP request
String      outputLedState = "off";                    // Auxiliar variables to store the current output state

WIFI_AP_Serv::WIFI_AP_Serv(bool debug) {
  if(m_debug) Serial.print("Creating the WIFI_AP_Serv object");
}

void WIFI_AP_Serv::setup() {
  Serial.print("Setting AP (Access Point)…");          // Connect to Wi-Fi network with SSID and password
  WiFi.softAPConfig(local_IP, gateway, subnet);    
  WiFi.softAP(ssid, password , 0, false, 1);           // Remove the password parameter, if you want the AP (Access Point) to be open

  IPAddress IP = WiFi.softAPIP(); 
  Serial.print("AP IP address: ");
  Serial.println(IP);
     
  server.begin();
}

void WIFI_AP_Serv::serve(String** input, int lin, int col, String* output, int nOutput){   // Wifi server inspired from: Rui Santos https://randomnerdtutorials.com  
  client = server.available();              // Listen for incoming clients
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
              //digitalWrite(ledPin, LOW);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("LED off");
              outputLedState = "off";
              //digitalWrite(ledPin, HIGH);
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