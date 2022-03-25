#include <arduino.h>

#ifndef DATA_MANAGER
  #define DATA_MANAGER

#include <stdlib.h>
#include <wifi.h>
//#include <PubSubClient.h> // by Nick O'Leary
#include <ArduinoJson.h>  // by Benoit Blanchon

class DataManager {

  public:
    DataManager(String topic, String server_IP, int server_port, bool debug=false, bool signature=true, bool numbering=true);
    //void dataConnect();
    //void pushData();
    void create_MQTT_client();
    void MQTT_Send(String topic, String payload);
  
  private:
    String        o_topic;
    bool          o_debug;
    bool          o_signature;
    bool          o_numbering;
    String        o_server_IP;
    int           o_server_port;
    unsigned long o_numbering_counter;
    WiFiClient    o_wifi_client;
    //PubSubClient  o_mqtt_client;


    //void mongoConnect();
    //void influxConnect();


};
  







#endif // DATA_TO_CLOUD
