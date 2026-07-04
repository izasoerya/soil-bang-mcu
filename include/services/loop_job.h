#if !defined(LOOP_JOB_H)
#define LOOP_JOB_H

#include "task.h"

#include <NimBLECharacteristic.h>
#include <models/device.h>
#include "sensor_battery.h"
#include "../utils/utils.h"

class DeviceDataBroadcast : public Task
{
private:
    const char *_deviceName;
    const uint8_t _battPin;
    NimBLECharacteristic *_txBle;

public:
    DeviceDataBroadcast(const char *dName, NimBLECharacteristic *txBle, const uint8_t battPin, const uint32_t delay)
        : _deviceName(dName), _txBle(txBle), _battPin(battPin)
    {
        setDelay(delay);
    }
    ~DeviceDataBroadcast() {}

    void init() override {}

    void loop() override
    {
        Device d;
        d.id = 1;
        d.hostName = String(_deviceName);
        d.battery = SensorBattery::read(_battPin);
        d.freeHeap = ESP.getFreeHeap();
        d.minFreeHeap = ESP.getMinFreeHeap();
        d.largestBlock = ESP.getMaxAllocHeap();
        d.lastResetReason = esp_reset_reason_to_string(esp_reset_reason());

        String jsonStr = d.toJSON();
        _txBle->setValue(jsonStr.c_str());
        if (_txBle->notify())
            Serial.printf("✓ Device status sent: %s\n", jsonStr.c_str());
        else
            Serial.println("✗ Failed sending device status");
    }
};

#include "connection_wifi.h"
#include <ElegantOTA.h>

class DeviceOTA : public Task
{
private:
    AsyncWebServer *_server = nullptr;
    WiFiConnection _wifi;

public:
    DeviceOTA(const char *ssid, const char *password, const char *hostname, const uint32_t delay) : _wifi(ssid, password, hostname)
    {
        setDelay(delay);
    }
    ~DeviceOTA()
    {
        delete _server;
    }

    void init() override
    {
        _wifi.begin();
        _wifi.setupMDNS();
        _server = new AsyncWebServer(80);
        ElegantOTA.begin(_server);
        ElegantOTA.setAutoReboot(true);
        _server->begin();
    }

    void loop() override
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            ElegantOTA.loop();
        }
    }
};

#endif // LOOP_JOB_H
