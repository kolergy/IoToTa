
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

WIFI_AP_Serv::WIFI_AP_Serv(bool debug) {
  m_debug = debug;
  if(m_debug) Serial.println("Creating the WIFI_AP_Serv on temporary ssid: " + String(ssid));
}

void WIFI_AP_Serv::setup() {
  // Initialize the WiFi module and set the WiFi mode to AP
  // To a given local IP address, and luanch the web server
  if(m_debug) Serial.println("Setting AP (Access Point)…");          // Connect to Wi-Fi network with SSID and password
  WiFi.softAPConfig(local_IP, gateway, subnet);    
  WiFi.softAP(ssid, password , 0, false, 1);           // Remove the password parameter, if you want the AP (Access Point) to be open

  IPAddress IP = WiFi.softAPIP(); 
  //if(m_debug) Serial.print("AP IP address: %s", IP.toString().c_str());  
  if(m_debug) Serial.print("Ready to serve on host: " + IP.toString());
  server.begin();
}

void WIFI_AP_Serv::serve(String** input, int lin, int col, String* output, int nOutput){   
  // Wifi server inspired from: Rui Santos https://randomnerdtutorials.com  
  // serve a web page which display the input as a table for exemple a list of WI-FI available networks,
  // their encriptions type, and their associated RSSI (Received Signal Strength Indicator)
  // As an output this function collects the line number selected by the user from the table and an associated 
  // string eg: the password of the selected wifi network.
  //Serial.println("Ready to serve on host: " + WiFi.softAPIP().toString());
  //Serial.println(char(lin + 1) + " Wifi networks found");
  client = server.available();              // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client
        char c       = client.read();       // read a byte, then
        if (c !='\r' && c !='\n' ) {
          currentLine += c; 
        } else if (c == '\r') {   
          //Serial.print("#_RRR_#");
        } else if (c == '\n') {               // if the byte is a newline character
          Serial.println("<< " + currentLine);
          header     += currentLine;
          if (currentLine.length() != 0) {
            // Check to see if the first word of the current line is "GET /?ssid=" to collect the ssid & password
            if (currentLine.startsWith("GET /")) {
              // Extract the ssid and password from the current line
              int s0 = currentLine.indexOf("?ssid=");
              if (s0 > 0) {
                s0 += 6;
                String ssid     = currentLine.substring(s0, currentLine.indexOf("&pword="));
                String password = currentLine.substring(currentLine.indexOf("&pword=") + 7, currentLine.indexOf(" HTTP/1.1"));
                Serial.println("ssid: "     + ssid    + ", password: " + password); 
                output[0] = ssid;
                output[1] = password;
                break; // got the data we wanted
              }      
            }
            currentLine = "";      
          } else {
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println(http_response_code_ok);
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            //// Web Page Heading
            client.println("<div class=\"topnav\">"                );
            client.println("<h1>IoToTa Temporrary Web Server</h1>" );
            client.println("</div>"                                );
            client.println("<div class=\"content\">"               );
            client.println("<div class=\"card-grid\">"             );
            client.println("<div class=\"card\">"                  );
            client.println("<form action=\"/\" method=\"get\" enctype=\"text/plain\">"   );
            client.println("<p>"                                   );

            // list SSIDs
            client.println("<label for=\"ssid_select\">Choose a Wifi Network:</label>"   ); 
            client.println("<select name=\"ssid\" id=\"ssid_select\">"                   );
            client.println("<option value=\"\">--Please choose a Wifi Network--</option>");
            for(int l  = 0; l < lin; l++){
                client.print("<option value=\"" + input[l][0] +"\">" + input[l][0] + ", rssi:" + input[l][1] + ", " + input[l][2] + "</option>");
            }
            client.println("</select>"                                                   ); // end of select
            client.println("<br>"                                                        ); // end of line

            // password input
            client.println("<label for=\"password_input\">Password:</label>"); // label
            client.println("<textarea name=\"pword\" id=\"password_input\" rows=“1” cols=“30”>"                  ); // textarea
            client.println("</textarea>"                                    ); // end of password input
            client.println("<br>"                                           ); // end of line

            // end of the form
            client.println("<input type =\"submit\" value =\"Submit\">"); // Submit button
            client.println("</p>"                                      ); // end of the form
            client.println("</form>"                                   ); // close the form
            client.println("</div>"                                    ); // close the card
            client.println("</div>"                                    ); // close the card-grid
            client.println("</div>"                                    ); // close the content
            client.println("</body>"                                   ); // close the body
            client.println("</html>"                                   ); // close the html
            client.println();                  // The HTTP response ends with another blank line
            break;                             // break out of the while loop

          }                              
        }
      }
    }
    
    header = "";            // Clear the header variable
    client.stop();          // Close the connection
    if(m_debug) Serial.println("Client disconnected.\n");
  }
} 