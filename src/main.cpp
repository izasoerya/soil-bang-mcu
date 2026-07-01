#include <Arduino.h>
#include <NimBLEDevice.h>
#include "services/parser_json.h"
#include "services/sensor_as7265x.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

NimBLEServer *pServer = nullptr;
NimBLECharacteristic *pTxCharacteristic = nullptr;

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
	void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
	{
		std::string rxValue = pCharacteristic->getValue();
		if (rxValue.length() > 0)
		{
			Serial.print("Received from App: ");
			Serial.println(rxValue.c_str());

			bool flagSampling = ParserJSON::isCommandTakeSample(rxValue.c_str());
			if (flagSampling)
			{
				Sensor payload = SensorAS7265X::generateDummy();
				pTxCharacteristic->setValue(payload.toJSON().c_str());
				if (pTxCharacteristic->notify())
					Serial.println("Success sending data!");
				else
					Serial.println("Failed sending data!");

				flagSampling = !flagSampling;
			}
		}
	}
};

void setup()
{
	Serial.begin(115200);

	NimBLEDevice::init("SOIL-BANG");
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
	pRxCharacteristic->setCallbacks(new RxCallbacks());

	NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->start();
}

void loop() {}