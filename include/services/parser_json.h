#ifndef PARSER_JSON_H
#define PARSER_JSON_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <utils/enum.h>

class ParserJSON
{
public:
    ParserJSON() {}
    ~ParserJSON() {}

    static const CommandType parseCommand(const char *incoming)
    {
        JsonDocument doc;
        deserializeJson(doc, incoming);

        if (doc["command"] == "sampling")
            return CommandType::sampling;
        else if (doc["command"] == "upload")
            return CommandType::upload;
        }
};

#endif