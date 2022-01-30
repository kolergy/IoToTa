#include <arduino.h>

#ifndef DATA_MANAGER
  #define DATA_MANAGER

#include <stdlib.h>

//const char* mqtt_server = "192.168.1.21";
// 

class DataManager {

    public:
      DataManager(char* topic, bool debug=false, bool signature=true, bool numbering=true);
      void dataConnect();
      void pushData();

    private:
      bool       o_topic;
      bool       o_debug;
      bool       o_signature;
      bool       o_numbering;
      void MQTTConnect();
      void mongoConnect();
      void influxConnect();


};
  







#endif // DATA_TO_CLOUD
