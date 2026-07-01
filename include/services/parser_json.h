#ifndef PARSER_JSON_H
#define PARSER_JSON_H

#include <Arduino.h>
#include <ArduinoJson.h>

class ParserJSON
{
public:
    ParserJSON() {}
    ~ParserJSON() {}

    static const bool isCommandTakeSample(const char *incoming)
    {
        JsonDocument doc;
        deserializeJson(doc, incoming);

        if (doc["command"] == "sampling")
        {
            return true;
        }
        return false;
    }
};

#endif