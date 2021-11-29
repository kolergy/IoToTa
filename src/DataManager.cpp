
#ifndef DATA_MANAGER
  #include "DataManager.h"
#endif // DATA_Manager

//#include <PubSubClient.h>

DataManager::DataManager(bool debug=false, bool signature=true, bool numbering=true) {
   o_debug     = debug;
   o_signature = signature;
   o_numbering = numbering;
  if(o_debug) Serial.print("Creating the DataManager DM object");

}

void DataManager::MQTTConnect() {
  if(o_debug) Serial.print("DM MQTTConnect Not implemented Yet");
}


void DataManager::mongoConnect() {
  if(o_debug) Serial.print("DM mongoConnect Not implemented Yet");
}


void DataManager::influxConnect() {
  if(o_debug) Serial.print("DM influxConnect Not implemented Yet");
}