#include <ArduinoJson.h>

struct Device
{
    int id;
    String hostName;
    double battery;
    uint32_t freeHeap;
    uint32_t minFreeHeap;
    uint32_t largestBlock;
    String lastResetReason;

    String toJSON() const
    {
        JsonDocument doc;
        doc["type"] = "device";
        doc["id"] = id;
        doc["hostName"] = hostName;
        doc["battery"] = battery;
        doc["freeHeap"] = freeHeap;
        doc["minFreeHeap"] = minFreeHeap;
        doc["largestBlock"] = largestBlock;
        doc["lastResetReason"] = lastResetReason;

        String output;
        serializeJson(doc, output);
        return output;
    }

    static Device fromJson(JsonObjectConst obj)
    {
        Device d;
        d.id = obj["id"].as<int>();
        d.hostName = obj["hostName"].as<String>();
        d.battery = obj["battery"].as<double>();
        d.freeHeap = obj["freeHeap"].as<int>();
        d.minFreeHeap = obj["minFreeHeap"].as<int>();
        d.largestBlock = obj["largestBlock"].as<int>();
        d.lastResetReason = obj["lastResetReason"].as<String>();
        return d;
    }
};