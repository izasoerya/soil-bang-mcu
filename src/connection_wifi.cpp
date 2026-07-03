#include "connection_wifi.h"

WiFiConnection::WiFiConnection(const char *ssid, const char *password, const char *hostname)
    : _wifiClient(WiFiClient()),
      _ssid(ssid),
      _password(password),
      _hostname(hostname) {}

WiFiConnection::~WiFiConnection() {}

void WiFiConnection::_setupMDNS()
{
    if (!MDNS.begin(_hostname))
    {
        Serial.println("Error setting up mDNS responder");
        ESP.restart();
    }
    Serial.print("mDNS responder started: ");
    Serial.print(_hostname);
    Serial.println(".local");
    MDNS.addService("http", "tcp", 80);
}

void WiFiConnection::begin()
{
    WiFi.mode(WIFI_STA);
    WiFi.hostname(_hostname);
    WiFi.begin(_ssid, _password);
}

void WiFiConnection::setupMDNS()
{
    _setupMDNS();
}

void WiFiConnection::reconnect()
{
    const uint32_t currentTime = millis();
    while (millis() < currentTime + 5000)
        if (WiFi.status() == WL_CONNECTED)
            return;

    WiFi.disconnect(true); // force clean state
    delay(100);
    begin();
}

int WiFiConnection::post(String url, String payload)
{
    HTTPClient _http;
    _http.begin(url);
    _http.addHeader("Content-Type", "application/json");
    int _response = _http.POST(payload);
    _http.end();

    return _response;
}

int WiFiConnection::postSupabase(JsonDocument payload)
{
    String _payload;
    HTTPClient _http;
    const String targetUrl = String(_supabase_url) + String(_sensorTable);
    _http.begin(targetUrl);
    _http.addHeader("Content-Type", "application/json");
    _http.addHeader("apikey", _publishable_key);
    _http.addHeader("Authorization", "Bearer " + String(_publishable_key));
    serializeJson(payload, _payload);
    int _response = _http.POST(_payload);
    Serial.println("HTTP Code: " + String(_response));
    _http.end();

    return _response;
}