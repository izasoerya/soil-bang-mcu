#ifndef SENSOR_BATTERY
#define SENSOR_BATTERY

#include "models/sensor.h"

class SensorBattery
{
private:
public:
    SensorBattery() {}
    ~SensorBattery() {}

    static const float read(uint8_t gpioPin)
    {
        const uint16_t R1 = 1000; // TODO: CHANGE THIS LATER
        const uint16_t R2 = 1000; // TODO: CHANGE THIS LATER

        uint16_t rawReading = analogRead(gpioPin);
        float voltage = (rawReading / 4095.0) * 3.3;
        float result = voltage * (R1 + R2) / R2;

        return result;
    }
};

#endif