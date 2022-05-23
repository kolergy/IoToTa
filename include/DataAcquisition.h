

#ifndef DATA_ACQUISITION
  #define DATA_ACQUISITION

  #include <Arduino.h>

  #ifndef CONFIGURATION
    #include "Configuration.h"
  #endif // CONFIGURATION




  #ifdef SENS_SHT31
    //#include <SHT31.h>  // by Rob Tillaart
  #endif //SENS_SHT31

  #ifdef ADS
    #include <ADS1115.h>
  #endif //ADS

  class DataAcquisition {
    public:
      DataAcquisition();
      float ReadVoltage(byte pin);
      void  display_esp32_pins();
      void  display_espD32_pins(); 
      void  read_sensors();
      #ifdef MPX
        float readMPX_raw(byte pin);
        float readMPX(byte pin);
      #endif //MPX
      
    private:
      #ifdef SENS_SHT31
        SHT31       sht30;
      #endif //SENS_SHT31
  };

#endif // DATA_ACQUISITION
