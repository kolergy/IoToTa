
#ifndef DATA_MANAGER
  #include "DataManager.h"
#endif // DATA_Manager
  
//static const int   JSONSiz       =  1000;

//StaticJsonDocument<JSONSiz> dataJ;                         // Memoire pour l'arbre JSON  ATTENTION: Augmenter la valeur quand on augmente la taille du JSON


DataManager::DataManager(const char* server_IP, int server_port, const char* mqtt_user, const char* mqtt_password, bool debug, bool signature, bool numbering) {
  this->o_debug         = debug;
  this->o_signature     = signature;
  this->o_numbering     = numbering;
  this->o_server_IP     = server_IP;
  this->o_server_port   = server_port;
  this->o_mqtt_user     = mqtt_user;
  this->o_mqtt_password = mqtt_password;
  this->o_wifi_client   = WiFiClient();
  this->o_mqtt_client   = PubSubClient(o_wifi_client);
  o_mqtt_initialized    = false;

  if(o_debug) Serial.print("Creating the DataManager DM object");
  if(o_numbering == true) {
    o_numbering_counter = 0;
  }
}

void DataManager::mqtt_reconnect() {
  if(o_mqtt_initialized == false) {
    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected MQTT can be initialized");
      //*********************** MQTT Setup *****************************************
      o_mqtt_client.setServer(o_server_IP, o_server_port);
      o_mqtt_initialized = true;
      Serial.println("MQTT can be initialization copmleted");
    }
  }
  while (!o_mqtt_client.connected()) {                                           // Loop until we're reconnected
    if(o_debug) Serial.print("Attempting MQTT connection...");
    if (o_mqtt_client.connect("MQTTClient", o_mqtt_user, o_mqtt_password)) {        // Attempt to connect
      if(o_debug) Serial.println("MQTT connected");
    } else {
      if(o_debug) {
        Serial.print("MQTT conncetion failed, error code=");
        Serial.print(o_mqtt_client.state());
        Serial.println(" try again in 2 seconds");
      }
      delay(2000);
    }
  }
}

void DataManager::pushData(const char*  top_char, String val_str) {       // Publish a float value as a message to the MQTT broker
  mqtt_publish(top_char, val_str);
}

void DataManager::mqtt_publish(const char*  top_char, float val) {       // Publish a float value as a message to the MQTT broker
  mqtt_publish(top_char, String(val));
}


void DataManager::mqtt_publish(const char* top_char, String val_str) {   // Publish a string message to the MQTT broker
  char val_char[  val_str.length() + 1];
  val_str.toCharArray(val_char, val_str.length() + 1);
  mqtt_publish(top_char, val_char);
}


void DataManager::mqtt_publish(const char* top_char, char* val_char) {   // Publish a char array message to the MQTT broker
  if (!o_mqtt_client.connected()) {
    mqtt_reconnect();
  }
  char topic_char[strlen(top_char) + 1];
  strcpy(topic_char, top_char);
  if(o_debug) {
    Serial.println("Publishing to topic : " + String(topic_char) );
    Serial.println("Val : " + String(val_char));
  }
  o_mqtt_client.publish(topic_char,val_char);
}


//void DataManager::mongoConnect() {
//  if(o_debug) Serial.print("DM mongoConnect Not implemented Yet");
//}


//void DataManager::influxConnect() {
//  if(o_debug) Serial.print("DM influxConnect Not implemented Yet");
//}