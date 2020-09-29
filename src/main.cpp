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

// Library: Grove_Temperature_And_Humidity_Sensor
AirQualitySensor aqSensor(AQS_PIN);
Adafruit_BME280 bme;


// Sample the temperature sensor every 2 seconds. This is done so the outlier values can be filtered out easily.
const unsigned long CHECK_PERIOD_MS = 2000;
unsigned long lastCheck = 0;
int useCaseState = 0; // useCase 1 = Cold Chain(temp/humid), useCase 2 (AQM) = , useCase 3 = (pressure)



void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context); // Forward declaration


void setup()
{
    Tracker::instance().init();

    aqSensor.init();
    bme.begin();

    Particle.function(thisUseCase);

    // Callback to add key press information to the location publish
    Tracker::instance().location.regLocGenCallback(locationGenerationCallback);

    Particle.connect();
}

void loop()
{
    Tracker::instance().loop();

    if (millis() - lastCheck >= CHECK_PERIOD_MS) {
        lastCheck = millis();
    }
}


thisUseCase (String input){
    if input = "Cold Chain"
        useCaseState = 1;
    
    if input = "heavyTrucking"
        useCaseState = 2;
    
    if input = "railCar"
        useCaseState = 3;
    
}



// put in some function and let it redirect 

void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context)
{
    callback (input){
        parseFunction();
    }

}

parseFunction (input){
    switch (useCaseState){
        case 1: coldChain
        break;

        case 2: railCar
        break;

        case 3: heavyTrucking
        break;
    }
}

coldChain (){
    // Read Measurments from BME 280
    int temp = (int)bme.readTemperature();
    int humidity = (int)bme.readHumidity();

    // Add Names under device page
    writer.name("Temperature").value(temp, 1);
    writer.name("Humidity").value(humidity, 1);
}

railCar (){
    // Read Measurments from BME 280
    int pressure = (int)(bme.readPressure() / 100.0F);
    int humidity = (int)bme.readHumidity();

    // Add Names under device page
    writer.name("Pressure").value(pressure, 1);
    writer.name("Humidity").value(humidity, 1);
=
}

heavyTrucking(){
    // Read Measurments from Air Quality v1.2
    // Persona 2
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

    // Write AQ under Device Page
    writer.name("Quality").value(qual, 15); 

}

