#include "Wifi.h"

#include <ESP8266WiFi.h>

Wifi::Wifi(Stream &Serial, const char *ssid, const char *passphrase)
    : Serial(Serial), ssid(ssid), passphrase(passphrase),
    ip(IPAddress(0, 0, 0, 0)), _error(false)
{
}

Wifi::Wifi(Stream &Serial, const char *ssid, const char *passphrase,
           IPAddress ip, IPAddress gateway, IPAddress subnet)
    : Serial(Serial), ssid(ssid), passphrase(passphrase),
    ip(ip), gateway(gateway), subnet(subnet), _error(false)
{
}

bool Wifi::begin()
{
    WiFi.mode(WIFI_STA);
    //  Enable light sleep
    wifi_set_sleep_type(LIGHT_SLEEP_T);

    if (ip != IPAddress(0, 0, 0, 0) && !WiFi.config(ip, gateway, subnet))
    {
        Serial.println("STA Failed to configure fixed IP");
        _error = true;
        return false;
    }

    WiFi.begin(ssid, passphrase);

    static const int max_attempt = 100;
    int attempt = 0;
    while (attempt < max_attempt && WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        attempt++;
    }
    Serial.println("");

    _error = (attempt >= max_attempt);
    return !_error;
}

bool Wifi::reset()
{
    _error = !disconnect();
    return !_error;
}

bool Wifi::beginLoop() { return !_error; }

bool Wifi::error() { return _error; }

bool Wifi::disconnect()
{
    return WiFi.disconnect();
}

IPAddress Wifi::localIP()
{
    return WiFi.localIP();
}
