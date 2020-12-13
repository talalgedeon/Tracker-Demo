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

#define ColdChain 1
#define Chemical 2
#define Heavy 3
int useCaseVal = 0; // 1 = cold chain, 2= chemical, 3= heavy
int useCaseState(String input); // forward definition for .cpp



void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context); 


void setup()
{
    Tracker::instance().init(); 

    aqSensor.init();

    bme.begin();

    Particle.function("useCase", useCaseState);

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

int useCaseState(String command)
{
  if(command == "cold chain")
  {
      useCaseVal = ColdChain;
  }
  if(command == "chemical")
  {
      useCaseVal = Chemical;
  }
//   if(command == "heavy")
//   {
//       useCaseVal = Heavy;
//   }
}

void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context)
{

int temp = (int)bme.readTemperature();
int humidity = (int)bme.readHumidity();   
int pressure = (int)(bme.readPressure() / 100.0F);
int quality = aqSensor.slope();
// String temp2 = "THIS IS THE CURRENT TEMPERATURE" + temp;
String qual = "None";

switch (useCaseVal) {
        case ColdChain:
            // if (temp >= 20 ){
            //     Particle.publish("High temp");
            // }
            // if (temp == 19){
            //     Particle.publish("Good Temp");
            // }
            // if (temp <= 19){
            //     Particle.publish("Low Temp");
            // }

            writer.name("Temperature").value(temp, 1); 
            writer.name("Humidity").value(humidity, 1);

            
            // Particle.publish("awsEmail", "This is the message" , 60, PRIVATE);
        break;
        case Chemical:
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
            writer.name("Temperature").value(temp, 1);
            writer.name("Humidity").value(humidity, 1); 
            writer.name("Quality").value(qual, 16);

        break;
        // case Heavy:
        //     writer.name("Pressure").value(pressure, 1);
        // break;
    }   
}
