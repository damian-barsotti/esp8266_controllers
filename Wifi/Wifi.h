#ifndef WIFI_H
#define WIFI_H

#include <IPAddress.h>
#include <Arduino.h>

class Wifi
{
public:
    Wifi(Stream &Serial);
    bool begin(const char *ssid, const char *passphrase);
    // With fixed address:
    bool begin(const char *ssid, const char *passphrase,
               IPAddress ip, IPAddress gateway, IPAddress subnet);
    bool disconnect();
    IPAddress localIP();

private:
    const String ssid;
    const String passphrase;
    Stream &Serial;
};

#endif
