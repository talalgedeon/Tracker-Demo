#include "Particle.h"
#include "tracker_config.h"
#include "tracker.h"
#include "Air_Quality_Sensor.h"
#include "Adafruit_BME280.h"
#define AQS_PIN A2

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);
PRODUCT_ID(TRACKER_PRODUCT_ID); 
PRODUCT_VERSION(TRACKER_PRODUCT_VERSION); 

SerialLogHandler logHandler(115200, LOG_LEVEL_TRACE, {
    { "app.gps.nmea", LOG_LEVEL_INFO },
    { "app.gps.ubx",  LOG_LEVEL_INFO },
    { "ncp.at", LOG_LEVEL_INFO },
    { "net.ppp.client", LOG_LEVEL_INFO },
}); 

AirQualitySensor aqSensor(AQS_PIN); 
Adafruit_BME280 bme; 

const unsigned long CHECK_PERIOD_MS = 2000;
unsigned long lastCheck = 0;
int useCaseState(int input); // useCase 1 = Cold Chain(temp/humid), useCase 2 (AQM) = , useCase 3 = (pressure) 


void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context); // Forward declaration :)


void setup()
{
    Tracker::instance().init(); 

    aqSensor.init();

    bme.begin();

    Particle.function("Switch Case", useCaseState);

    Tracker::instance().location.regLocGenCallback(locationGenerationCallback);

    Particle.connect(); 
}


void loop()
{
    Tracker::instance().loop(); 

    if (millis() - lastCheck >= CHECK_PERIOD_MS) {

        // Should i do anything here?
        lastCheck = millis(); 

    }
}

void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context)
{
    callback (input){ // not sure i need this
        parseFunction();
    }
    
}
// Should void be added here? is the switch case right, when 1 is called in function useCaseState the device will adpot coldChian method
void parseFunction (input){
    switch (useCaseState){ 
        case 1: coldChain() 
        break;

        case 2: railCar() 
        break;

        case 3: heavyTrucking()
        break;
    }
}




void coldChain (){
    int temp = (int)bme.readTemperature();
    int humidity = (int)bme.readHumidity();

    writer.name("Temperature").value(temp, 1); 
    writer.name("Humidity").value(humidity, 1);
}

void railCar (){
    int pressure = (int)(bme.readPressure() / 100.0F);
    int humidity = (int)bme.readHumidity();

    writer.name("Pressure").value(pressure, 1);
    writer.name("Humidity").value(humidity, 1);
}

void heavyTrucking(){

    int quality = aqSensor.slope();
    String qual = "None";

    if (quality == AirQualitySensor::FORCE_SIGNAL)
    {
        qual = "Danger";
    }
    else if (quality == AirQualitySensor::HIGH_POLLUTION)
    {
        qual = "High Pollution";
    }
    else if (quality == AirQualitySensor::LOW_POLLUTION)
    {
        qual = "Low Pollution";
    }
    else if (quality == AirQualitySensor::FRESH_AIR)
    {
        qual = "Fresh Air";
    }

    // Where do i add writer, does it only work in location call back?
    writer.name("Quality").value(qual, 15); 
}