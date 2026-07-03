#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ElegantOTA.h>
#include "services/parser_json.h"
#include "services/sensor_as7265x.h"
#include <services/sensor_battery.h>
#include <models/device.h>
#include "connection_wifi.h"
#include "../utils/utils.h"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

const char *deviceName = "soil-bang-1";
const char *ssid = "Subhanallah";
const char *password = "muhammadnabiyullah";
const uint8_t batteryPin = 1;

WiFiConnection wifi(ssid, password, deviceName);
NimBLEServer *pServer = nullptr;
NimBLECharacteristic *pTxCharacteristic = nullptr;
SensorAS7265X sensor(&Wire);
bool isDevMode = false;
bool isOTAMode = false;
AsyncWebServer *server = nullptr;

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

			CommandType type = ParserJSON::parseCommand(rxValue.c_str());
			switch (type)
			{
			case CommandType::sampling:
				Sensor payload;
				if (isDevMode)
					payload = SensorAS7265X::generateDummy();
				else
					payload = sensor.read();

				pTxCharacteristic->setValue(payload.toJSON().c_str());
				if (pTxCharacteristic->notify())
					Serial.println("Success sending data!");
				else
					Serial.println("Failed sending data!");
				break;

			case CommandType::upload:
				// go into wifi ota mode
				isOTAMode = true;
				break;

			default:
				break;
			}
		}
	}
};

uint64_t prevTime = 0;
uint64_t prevWifiBegin = 0;

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
	pRxCharacteristic->setCallbacks(new RxCallbacks());

	NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->start();

	if (!sensor.begin())
	{
		Serial.println("No I2C Device Found, Dev Mode Activated");
		isDevMode = true;
	}

	prevTime = millis();
	prevWifiBegin = millis();
}

void loop()
{
	if (millis() - prevTime > 10000)
	{
		if (pServer->getConnectedCount() > 0)
		{
			Device d;
			d.id = 1;
			d.hostName = String(deviceName);
			d.battery = SensorBattery::read(batteryPin);
			d.freeHeap = ESP.getFreeHeap();
			d.minFreeHeap = ESP.getMinFreeHeap();
			d.largestBlock = ESP.getMaxAllocHeap();
			d.lastResetReason = esp_reset_reason_to_string(esp_reset_reason());

			String jsonStr = d.toJSON();
			pTxCharacteristic->setValue(jsonStr.c_str());
			if (pTxCharacteristic->notify())
				Serial.printf("✓ Device status sent: %s\n", jsonStr.c_str());
			else
				Serial.println("✗ Failed sending device status");
		}

		prevTime = millis();
	}

	if (isOTAMode)
	{
		if (WiFi.status() == WL_CONNECTED)
		{
			if (server == nullptr)
			{
				wifi.setupMDNS();
				server = new AsyncWebServer(80);
				ElegantOTA.begin(server);
				ElegantOTA.setAutoReboot(true);
				server->begin();
			}
			else
			{
				ElegantOTA.loop();
			}
		}
		else
		{
			if (millis() - prevWifiBegin > 10000)
			{
				wifi.begin();
				prevWifiBegin = millis();
			}
		}
	}
}