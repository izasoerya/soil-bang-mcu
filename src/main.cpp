#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ElegantOTA.h>
#include "services/parser_json.h"
#include "services/sensor_as7265x.h"
#include <services/sensor_battery.h>
#include <models/device.h>
#include "connection_wifi.h"
#include "../utils/utils.h"
#include "task.h"
#include <services/loop_job.h>
#include "services/bluetooth_callback.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
const char *deviceName = "soil-bang-1";
const char *ssid = "NodeSensorWiFi";
const char *password = "muhammadnabiyullah";
const uint8_t batteryPin = 1;

NimBLEServer *pServer = nullptr;
NimBLECharacteristic *pTxCharacteristic = nullptr;
TaskRunner runner;

void readSensorCallback()
{
	static SensorAS7265X sensor(&Wire);
	static ReadSensorMode *sensorMode = nullptr;

	if (sensorMode == nullptr)
	{
		sensorMode = new ReadSensorMode;
		if (sensor.begin())
			*sensorMode = ReadSensorMode::sensorMode;
		else
			*sensorMode = ReadSensorMode::devMode;
	}
	Sensor payload;
	if (*sensorMode == ReadSensorMode::sensorMode)
		payload = sensor.read();
	else
		payload = sensor.generateDummy();

	pTxCharacteristic->setValue(payload.toJSON().c_str());
	if (pTxCharacteristic->notify())
		Serial.println("Success sending data!");
	else
		Serial.println("Failed sending data!");
}

void activateOTACallback()
{
	static DeviceOTA *configOTA = nullptr;
	if (configOTA == nullptr)
	{
		configOTA = new DeviceOTA(ssid, password, deviceName, 0);
		runner.addTask(configOTA);
	}
}

RxCallbacks rxCallback(readSensorCallback, activateOTACallback);

void setup()
{
	Serial.begin(115200);

	NimBLEDevice::init(deviceName);
	NimBLEDevice::setMTU(512);

	pServer = NimBLEDevice::createServer();
	pServer->setCallbacks(new ServerCallbacks());

	NimBLEService *pService = pServer->createService(SERVICE_UUID);
	pTxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_TX,
		NIMBLE_PROPERTY::NOTIFY);

	NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_RX,
		NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
	pRxCharacteristic->setCallbacks(&rxCallback);

	NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->start();

	static DeviceDataBroadcast deviceData = DeviceDataBroadcast(deviceName, pTxCharacteristic, batteryPin, 10000);
	runner.addTask(&deviceData);
}

void loop()
{
	if (pServer->getConnectedCount() > 0)
	{
		runner.loopTask();
	}
}