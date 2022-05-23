
#ifndef DATA_ACQUISITION
  #include "DataAcquisition.h"
#endif // DATA_ACQUISITION

DataAcquisition::DataAcquisition() {
  #ifdef SENS_SHT31
    //sht30 = SHT31();
    //sht30.begin(SHT31_ADDRESS);
  #endif //SENS_SHT31
}

void DataAcquisition::read_sensors() {
  #ifdef SENS_SHT31
    float temp_c = sht30.readTemperature();
    Serial.printf("Temperature: %f °C\n", temp_c);
    float rh_pct = sht30.readHumidity();
    Serial.printf("Humidity: %f %%\n", rh_pct);
  #endif //SENS_SHT31
  #ifdef VBAT_ADC_PIN
    float v_bat_v = ReadVoltage(VBAT_ADC_PIN);
    Serial.printf("V_bat_V: %f V\n", v_bat_v);
  #endif //VBAT_ADC_PIN
}


#ifdef ADS // use of ADS to perform ADC 
    float DataAcquisition::ReadVoltage(byte pin) {
    return ADS.toVoltage(ADS.readADC(pin));
    }
#else // NO ADS available use the ESP32 noisy ADC   
    float DataAcquisition::ReadVoltage(byte pin){                   // From: https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino
      float reading = analogRead(pin);                              // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
      if(     reading < 0   ) return -999999.9;
      else if(reading < 1   ) return       0.0;
      else if(reading > 4095) return  999999.9;
      //return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
      //return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089; // Added an improved polynomial, use either, comment out as required
      return 3.3 * reading / 4095;   // polynomeal dose not match 3.3v
    }                                                           
#endif // ADS

#ifdef MPX
    float DataAcquisition::readMPX_raw(byte pin) {
    // https://www.nxp.com/docs/en/data-sheet/MPXV7002.pdf
    // https://makersportal.com/blog/2019/02/06/arduino-pitot-tube-wind-speed-theory-and-experiment#datasheet
    
    float U_mes_V   = ReadVoltage(pin);
    #ifdef ESP32
    float U_ref_V   = ReadVoltage(VCC_ADC_PIN);
    #endif
    float P_Pa      = 1000*(5*U_mes_V/U_ref_V-5/2);
    #ifdef ESP32
        Serial.printf("U Reference    : %6.3f V\n" , U_ref_V);
        Serial.printf("U Mesured      : %6.3f V\n" , U_mes_V);
        Serial.printf("Delta Pressure : %6.3f Pa\n", P_Pa   );
    #else
        Serial.println("U Reference   :   " + String(U_ref_V) + " V" );
        Serial.println("U Mesured     :   " + String(U_mes_V) + " V" );
        Serial.println("Delta Pressure: " + String(P_Pa   ) + " Pa");
    #endif // ESP32

    return(P_Pa);
    }

    float readMPX(byte pin) {
    return(readMPX_raw(pin) - P_calib_Pa[pin]);
    }
#endif //MPX

void DataAcquisition::display_espD32_pins() {
  //Serial.printf("pin[00] : %6.3f V\n" , 3.3 * analogRead( 0) / 4095);
  //Serial.printf("pin[02] : %6.3f V\n" , 3.3 * analogRead( 2) / 4095);
  //Serial.printf("pin[04] : %6.3f V\n" , 3.3 * analogRead( 4) / 4095);

  //Serial.printf("pin[12] : %6.3f V\n" , 3.3 * analogRead(12) / 4095);
  //Serial.printf("pin[13] : %6.3f V\n" , 3.3 * analogRead(13) / 4095);
  //Serial.printf("pin[14] : %6.3f V\n" , 3.3 * analogRead(14) / 4095);
  //Serial.printf("pin[15] : %6.3f V\n" , 3.3 * analogRead(15) / 4095);

  //Serial.printf("pin[25] : %6.3f V\n" , 3.3 * analogRead(25) / 4095);
  //Serial.printf("pin[26] : %6.3f V\n" , 3.3 * analogRead(26) / 4095);
  //Serial.printf("pin[27] : %6.3f V\n" , 3.3 * analogRead(27) / 4095);

  Serial.printf("pin[32] : %6.3f V\n" , 3.3 * analogRead(32) / 4095);
  Serial.printf("pin[33] : %6.3f V\n" , 3.3 * analogRead(33) / 4095);
  Serial.printf("pin[34] : %6.3f V\n" , 3.3 * analogRead(34) / 4095);
  Serial.printf("pin[35] : %6.3f V\n" , 3.3 * analogRead(35) / 4095);

  Serial.printf("pin[36] : %6.3f V\n" , 3.3 * analogRead(36) / 4095);
  Serial.printf("pin[37] : %6.3f V\n" , 3.3 * analogRead(37) / 4095);
  Serial.printf("pin[38] : %6.3f V\n" , 3.3 * analogRead(38) / 4095);
  Serial.printf("pin[39] : %6.3f V\n" , 3.3 * analogRead(39) / 4095);
}


void DataAcquisition::display_esp32_pins(){
    Serial.println("ReadVoltage");
    #if defined(LOLIN_D32) || defined(LOLIN_D32_PRO) 
    int pins[] = {0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39};
    #elif defined(LOLIN32)
    //int pins[] = {0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,38,39};
    int pins[] = {32,33,34,35,36,37,38,39};
    #else
    int pins[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
    #endif // ADC pins

    for(int i=0;i< sizeof(pins) / sizeof(pins[0]) ; i++) {
        int p = pins[i];
        Serial.printf("pin[%2.0d] : %6.3f V\n" , p, ReadVoltage(p));
    }
}
