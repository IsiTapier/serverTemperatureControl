#ifndef DataCollection_h
#define DataCollection_h

#include "DHT.h"
#include "Adafruit_BME280.h"
#include "../Util.h"


class DataCollection
{
private:
    
public:
    typedef struct sensor_values;

    static int getHumidityBme();
    static int getTempBme();
    static int getPressureBme();
    static int getOldHumidityBme();
    static int getOldTempBme();
    static int getOldPressureBme();

    static int getHumidityDht();
    static int getTempDht();

    static void printMessurements();

    static void setup();
    static void loop();
    static sensor_values bmeValues;
    static sensor_values dhtValues;
};

#endif