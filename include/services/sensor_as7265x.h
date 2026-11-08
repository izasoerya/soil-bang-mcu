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
        _sensor.disableIndicator(); // save power, no status blink
        _sensor.setBulbCurrent(AS7265X_LED_CURRENT_LIMIT_25MA, AS7265x_LED_WHITE);
        _sensor.setBulbCurrent(AS7265X_LED_CURRENT_LIMIT_25MA, AS7265x_LED_IR);
        _sensor.setBulbCurrent(AS7265X_LED_CURRENT_LIMIT_25MA, AS7265x_LED_UV);
        return 1;
    }

    Sensor _read(uint8_t ledColor)
    {
        _sensor.enableBulb(ledColor);
        _sensor.takeMeasurements();
        delay(100);

        Sensor sensorData = {
            .ledType = ledColor,

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
            .as72651Temp = (float)_sensor.getTemperature(AS72651_NIR),
            .as72652Temp = (float)_sensor.getTemperature(AS72652_VISIBLE),
            .as72653Temp = (float)_sensor.getTemperature(AS72653_UV)};

        _sensor.disableBulb(ledColor);
        return sensorData;
    }

    Sensor *read()
    {
        static Sensor sensorChannelAgregateResult[3];
        sensorChannelAgregateResult[0] = _read(AS7265x_LED_WHITE);
        sensorChannelAgregateResult[1] = _read(AS7265x_LED_UV);
        sensorChannelAgregateResult[2] = _read(AS7265x_LED_IR);
        return sensorChannelAgregateResult;
    }

    static const Sensor *generateDummy()
    {
        static const Sensor dummy[3] = {
            {// White
             .ledType = AS7265x_LED_WHITE,
             .nm410 = 12.5,
             .nm435 = 14.2,
             .nm460 = 16.8,
             .nm485 = 18.1,
             .nm510 = 20.5,
             .nm535 = 22.3,
             .nm560 = 24.0,
             .nm585 = 25.5,
             .nm610 = 26.2,
             .nm645 = 23.8,
             .nm680 = 21.4,
             .nm705 = 19.7,
             .nm730 = 17.5,
             .nm760 = 15.2,
             .nm810 = 13.9,
             .nm860 = 12.1,
             .nm900 = 10.5,
             .nm940 = 9.2,
             .as72651Temp = 28.5,
             .as72652Temp = 29.1,
             .as72653Temp = 28.8},
            {// UV
             .ledType = AS7265x_LED_UV,
             .nm410 = 48.3,
             .nm435 = 41.7,
             .nm460 = 33.2,
             .nm485 = 25.9,
             .nm510 = 18.4,
             .nm535 = 12.6,
             .nm560 = 9.1,
             .nm585 = 7.4,
             .nm610 = 6.2,
             .nm645 = 5.0,
             .nm680 = 4.1,
             .nm705 = 3.5,
             .nm730 = 2.9,
             .nm760 = 2.4,
             .nm810 = 2.0,
             .nm860 = 1.6,
             .nm900 = 1.3,
             .nm940 = 1.1,
             .as72651Temp = 29.0,
             .as72652Temp = 29.6,
             .as72653Temp = 29.3},
            {// IR
             .ledType = AS7265x_LED_IR,
             .nm410 = 3.2,
             .nm435 = 3.6,
             .nm460 = 4.1,
             .nm485 = 4.8,
             .nm510 = 5.6,
             .nm535 = 6.5,
             .nm560 = 7.9,
             .nm585 = 9.3,
             .nm610 = 11.0,
             .nm645 = 13.4,
             .nm680 = 16.8,
             .nm705 = 20.2,
             .nm730 = 24.7,
             .nm760 = 29.5,
             .nm810 = 35.1,
             .nm860 = 39.8,
             .nm900 = 42.6,
             .nm940 = 44.0,
             .as72651Temp = 28.7,
             .as72652Temp = 29.3,
             .as72653Temp = 29.0},
        };
        return dummy;
    }
};

#endif