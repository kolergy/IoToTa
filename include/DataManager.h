

#ifndef DATA_MANAGER
  #define DATA_MANAGER
  
  #include <Arduino.h>
  #include <stdlib.h>
  #include <WiFi.h>
  #include <PubSubClient.h> // by Nick O'Leary
  #include <ArduinoJson.h>  // by Benoit Blanchon

  

  class DataManager {

  public:
    DataManager(const char* server_IP, int server_port, const char* mqtt_user, const char* mqtt_password, bool debug=false, bool signature=true, bool numbering=true);
    void pushData(const char*, String);


  private:
    bool          o_debug;
    bool          o_signature;
    bool          o_numbering;
    bool          o_mqtt_initialized;
    const char*   o_server_IP;
    const char*   o_mqtt_user;
    const char*   o_mqtt_password;
    int     o_server_port;
    unsigned long o_numbering_counter;
    WiFiClient    o_wifi_client;
    PubSubClient  o_mqtt_client;

    void          mqtt_reconnect();
    void          mqtt_publish(const char*, String);
    void          mqtt_publish(const char*, float );
    void          mqtt_publish(const char*, char* );

    //void mongoConnect();
    //void influxConnect();


  };








#endif // DATA_TO_CLOUD
