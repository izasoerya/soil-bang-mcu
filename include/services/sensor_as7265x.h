#ifndef SENSOR_AS7265X
#define SENSOR_AS7265X

#include "SparkFun_AS7265X.h"
#include "models/sensor.h"

class SensorAS7265X
{
private:
    AS7265X _sensor;
    TwoWire *_wire;

public:
    SensorAS7265X(TwoWire *wire) : _wire(wire) {}
    ~SensorAS7265X() {}

    bool begin()
    {
        _wire->begin(6, 7);
        if (!_sensor.begin())
        {
            return 0;
        }
        return 1;
    }

    static const Sensor generateDummy()
    {
        Sensor dummyData = {
            // AS72653 (UV to VIS) - Simulated Irradiance (µW/cm²)
            .nm410 = 12.5,
            .nm435 = 14.2,
            .nm460 = 16.8,
            .nm485 = 18.1,
            .nm510 = 20.5,
            .nm535 = 22.3,

            // AS72652 (VIS to NIR) - Simulated Irradiance (µW/cm²)
            .nm560 = 24.0,
            .nm585 = 25.5,
            .nm610 = 26.2,
            .nm645 = 23.8,
            .nm680 = 21.4,
            .nm705 = 19.7,

            // AS72651 (NIR) - Simulated Irradiance (µW/cm²)
            .nm730 = 17.5,
            .nm760 = 15.2,
            .nm810 = 13.9,
            .nm860 = 12.1,
            .nm900 = 10.5,
            .nm940 = 9.2,

            // Temperature of the ICs - Simulated roughly at room temp + IC heating
            .as72651Temp = 28.5,
            .as72652Temp = 29.1,
            .as72653Temp = 28.8};

        return dummyData;
    }

    Sensor read()
    {
        _sensor.takeMeasurements();

        Sensor sensorData = {
            // AS72653 (UV to VIS)
            .nm410 = _sensor.getCalibratedA(),
            .nm435 = _sensor.getCalibratedB(),
            .nm460 = _sensor.getCalibratedC(),
            .nm485 = _sensor.getCalibratedD(),
            .nm510 = _sensor.getCalibratedE(),
            .nm535 = _sensor.getCalibratedF(),

            // AS72652 (VIS to NIR)
            .nm560 = _sensor.getCalibratedG(),
            .nm585 = _sensor.getCalibratedH(),
            .nm610 = _sensor.getCalibratedI(),
            .nm645 = _sensor.getCalibratedJ(),
            .nm680 = _sensor.getCalibratedK(),
            .nm705 = _sensor.getCalibratedL(),

            // AS72651 (NIR)
            .nm730 = _sensor.getCalibratedR(),
            .nm760 = _sensor.getCalibratedS(),
            .nm810 = _sensor.getCalibratedT(),
            .nm860 = _sensor.getCalibratedU(),
            .nm900 = _sensor.getCalibratedV(),
            .nm940 = _sensor.getCalibratedW(),

            // Temperature of the ICs
            .as72651Temp = _sensor.getTemperature(1),
            .as72652Temp = _sensor.getTemperature(2),
            .as72653Temp = _sensor.getTemperature(3)};
    }
};

#endif