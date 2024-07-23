#ifndef WIFI_H
#define WIFI_H

#include <IPAddress.h>
#include <Arduino.h>

#include "../Controller/Controller.h"


class Wifi: public Controller
{
public:
    Wifi(Stream &Serial, const char *ssid, const char *passphrase);
    // With fixed address:
    Wifi(Stream &Serial, const char *ssid, const char *passphrase,
         IPAddress ip, IPAddress gateway, IPAddress subnet);
    bool begin();
    bool reset();
    bool beginLoop();
    bool error();
    bool disconnect();
    IPAddress localIP();

private:
    Stream &Serial;
    const char *ssid;
    const char *passphrase;
    IPAddress ip, gateway, subnet;
    bool _error;
};

#endif
