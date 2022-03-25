#include "DataCollection.h"

#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BME280 bme(4, 3);

typedef struct DataCollection::sensor_values {
    int temperature = 0;
    int humidity = 0;
    int pressure = 0;
    int oldTemperature;
    int oldHumidity;
    int oldPressure;
} sensor_values; 

sensor_values DataCollection::bmeValues;
sensor_values DataCollection::dhtValues;

void DataCollection::setup() {
    // dht.begin();
    if (bme.begin(0x76)) {
        Serial.println(F("BME-Sensor initialized"));
    } else {
        Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    }
}

void DataCollection::loop() {
    dhtValues.oldTemperature = dhtValues.temperature;
    dhtValues.oldHumidity = dhtValues.humidity;
    dhtValues.temperature = dht.readTemperature();
    dhtValues.humidity = 0; //dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(dhtValues.humidity) || isnan(dhtValues.temperature) || isnan(dhtValues.pressure)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        // return;
    }
    bmeValues.oldTemperature = bmeValues.temperature;
    bmeValues.oldHumidity = bmeValues.humidity;
    bmeValues.oldPressure = bmeValues.pressure;
    bmeValues.temperature = bme.readTemperature();
    bmeValues.humidity = bme.readHumidity();
    bmeValues.pressure = bme.readPressure()/100;
    printMessurements();
}

int DataCollection::getHumidityDht() {
    return dhtValues.humidity;
}


int DataCollection::getTempDht() {
    return dhtValues.temperature;
}


int DataCollection::getHumidityBme() {
    return bmeValues.humidity;
}

int DataCollection::getTempBme() {
    return bmeValues.temperature;
}

int DataCollection::getPressureBme() {
    return bmeValues.pressure;
}

int DataCollection::getOldHumidityBme() {
    return bmeValues.oldHumidity;
}

int DataCollection::getOldTempBme() {
    return bmeValues.oldTemperature;
}

int DataCollection::getOldPressureBme() {
    return bmeValues.oldPressure;
}

void DataCollection::printMessurements() {
    Serial.print(F("other Sensor: "));
    Serial.print(F("Humidity: "));
    Serial.print(dhtValues.humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(dhtValues.temperature);
    Serial.print(F("°C "));
    Serial.print(F("BME: "));
    Serial.print(F("Humidity: "));
    Serial.print(bmeValues.humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(bmeValues.temperature);
    Serial.print(F("°C Pressure: "));
    Serial.print(bmeValues.pressure);
    Serial.println(F("mbar"));
}