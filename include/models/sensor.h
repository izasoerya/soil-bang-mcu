#include <ArduinoJson.h>

struct Sensor
{
    float nm410;
    float nm435;
    float nm460;
    float nm485;
    float nm510;
    float nm535;

    // AS72652 (VIS to NIR)
    float nm560;
    float nm585;
    float nm610;
    float nm645;
    float nm680;
    float nm705;

    // AS72651 (NIR)
    float nm730;
    float nm760;
    float nm810;
    float nm860;
    float nm900;
    float nm940;

    // Temperature of the IC
    float as72651Temp;
    float as72652Temp;
    float as72653Temp;

    String toJSON()
    {
        JsonDocument doc;

        // 1. Discriminator Key
        doc["type"] = "sensor";

        // 2. AS72653 (UV to VIS)
        doc["nm410"] = nm410;
        doc["nm435"] = nm435;
        doc["nm460"] = nm460;
        doc["nm485"] = nm485;
        doc["nm510"] = nm510;
        doc["nm535"] = nm535;

        // 3. AS72652 (VIS to NIR)
        doc["nm560"] = nm560;
        doc["nm585"] = nm585;
        doc["nm610"] = nm610;
        doc["nm645"] = nm645;
        doc["nm680"] = nm680;
        doc["nm705"] = nm705;

        // 4. AS72651 (NIR)
        doc["nm730"] = nm730;
        doc["nm760"] = nm760;
        doc["nm810"] = nm810;
        doc["nm860"] = nm860;
        doc["nm900"] = nm900;
        doc["nm940"] = nm940;

        // 5. IC Temperatures
        doc["as72651Temp"] = as72651Temp;
        doc["as72652Temp"] = as72652Temp;
        doc["as72653Temp"] = as72653Temp;

        // Serialize to String and return
        String output;
        serializeJson(doc, output);
        output += "\n";
        return output;
    }
};