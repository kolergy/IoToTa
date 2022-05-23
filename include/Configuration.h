
#ifndef CONFIGURATION
    #define CONFIGURATION

    // Settings for the OTA
    #define LOLIN32
    //#define LOLIN_D32
    //#define LOLIN_D32_PRO


    //#define ADS // Use ADS 1115 for better ADC 
    //#define SENS_SHT31  // use of DHT31 Temp/RH sensor
    #ifdef SENS_SHT31
        #define SHT31_ADDRESS   0x44
    #endif // SENS_SHT31
 #define SHT31_ADDRESS   0x44
    #define DEVICENAME "IoToTa_LVR"
    #define FWVERSION  "0.8"
    #define FwVerTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/bin_version.txt"
    #define FwBinTgt   "https://raw.githubusercontent.com/kolergy/IoToTa/master/Data/firmware.bin"

    #define DEBUG_NTPClient

    #if defined(LOLIN_D32) || defined(LOLIN_D32_PRO)
    #define VCC_ADC_PIN 39
    #define VBAT_ADC_PIN 35
    #endif // LOLIN


#endif //CONFIGURATION