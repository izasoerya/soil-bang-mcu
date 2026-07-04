#ifndef BLUETOOTH_CALLBACK_H
#define BLUETOOTH_CALLBACK_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <utils/enum.h>
#include <models/sensor.h>
#include "parser_json.h"
#include "sensor_as7265x.h"

class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
    {
        Serial.println("Client connected!");
        pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override
    {
        Serial.println("Client disconnected - restarting advertising");
        NimBLEDevice::startAdvertising();
    }
};

class RxCallbacks : public NimBLECharacteristicCallbacks
{
private:
    void (*_sensorCb)();
    void (*_stateCb)();

public:
    RxCallbacks(void (*sensorCb)(), void (*stateCb)())
        : _sensorCb(sensorCb), _stateCb(stateCb) {}
    ~RxCallbacks() {}

    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            Serial.print("Received from App: ");
            Serial.println(rxValue.c_str());

            CommandType type = ParserJSON::parseCommand(rxValue.c_str());
            switch (type)
            {
            case CommandType::sampling:
                _sensorCb();
                break;

            case CommandType::upload:
                _stateCb();
                break;

            default:
                break;
            }
        }
    }
};

#endif