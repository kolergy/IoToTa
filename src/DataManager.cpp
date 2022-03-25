
#ifndef DATA_MANAGER
  #include "DataManager.h"
#endif // DATA_Manager
  
static const int   JSONSiz       =  1000;

StaticJsonDocument<JSONSiz> dataJ;                         // Memoire pour l'arbre JSON  ATTENTION: Augmenter la valeur quand on augmente la taille du JSON


 
DataManager::DataManager(String topic, String server_IP, int server_port, bool debug, bool signature, bool numbering) {
  o_debug       = debug;
  o_signature   = signature;
  o_numbering   = numbering;
  o_server_IP   = server_IP;
  o_server_port = server_port;
  if(o_debug) Serial.print("Creating the DataManager DM object");
  if(o_numbering == true) {
    o_numbering_counter = 0;
  }
  create_MQTT_client();
}
void DataManager::create_MQTT_client() {
  if(o_debug) Serial.print("Creating the MQTT client");
  o_wifi_client = WiFiClient();
 // o_mqtt_client = PubSubClient(o_wifi_client);
}

void DataManager::MQTT_Send(String topic, String payload) {
  // sends the data to the MQTT broker
  if(o_debug) Serial.print("Sending data to MQTT broker");
  if(o_numbering == true) {
    o_numbering_counter++;
  }
  //o_mqtt_client.publish(topic.c_str(), payload.c_str());
}


//void DataManager::mongoConnect() {
//  if(o_debug) Serial.print("DM mongoConnect Not implemented Yet");
//}


//void DataManager::influxConnect() {
//  if(o_debug) Serial.print("DM influxConnect Not implemented Yet");
//}