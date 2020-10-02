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
#define Truck 2
#define Rail 3
int useCaseVal = 0; // 1 = cold chain, 2= truck, 3= rail
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
  if(command == "truck")
  {
      useCaseVal = Truck;
  }
  if(command == "rail car")
  {
      useCaseVal = Rail;
  }
}

void locationGenerationCallback(JSONWriter &writer, LocationPoint &point, const void *context)
{

int temp = (int)bme.readTemperature();
int humidity = (int)bme.readHumidity();   
int pressure = (int)(bme.readPressure() / 100.0F);
int quality = aqSensor.slope();
String qual = "None";



switch (useCaseVal) {
        case ColdChain:
            writer.name("Temperature").value(temp, 1); 
            writer.name("Humidity").value(humidity, 1);
        break;
        case Truck:
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
            writer.name("Quality").value(qual, 16); 
        break;
        case Rail:
            writer.name("Pressure").value(pressure, 1);
            writer.name("Humidity").value(humidity, 1);
        break;
    }   
}



// void coldChain (){



// }

// void railCar (){


// }

// void heavyTrucking(){



//   switch (quality) {
//     case AirQualitySensor::FORCE_SIGNAL:
//       strncpy(qual, "Danger", sizeof(qual));
//       break;

//     case AirQualitySensor::HIGH_POLLUTION:
//       strncpy(qual, "High Pollution", sizeof(qual));
//       break;

//     case AirQualitySensor::LOW_POLLUTION:
//       strncpy(qual, "Low Pollution", sizeof(qual));
//       break;

//     case AirQualitySensor::FRESH_AIR:
//       strncpy(qual, "Fresh Air", sizeof(qual));
//       break;
    
//     default:
//       strncpy(qual, "None", sizeof(qual));
//   }
//   writer.name("Quality").value(qual, sizeof(qual));
// }