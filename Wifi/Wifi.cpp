#include "Wifi.h"

#include <ESP8266WiFi.h>

Wifi::Wifi(Stream &Serial)
: Serial(Serial)
{
}

bool
Wifi::begin(const char* ssid, const char *passphrase, 
    IPAddress ip, IPAddress gateway, IPAddress subnet)
{

    WiFi.mode(WIFI_STA);
    //  Enable light sleep
    wifi_set_sleep_type(LIGHT_SLEEP_T);

    if (ip != IPAddress(0, 0, 0, 0) && !WiFi.config(ip, gateway, subnet)){
        Serial.println("STA Failed to configure fixed IP");
        return false;    
    }

    return begin(ssid, passphrase);
}

bool
Wifi::begin(const char* ssid, const char *passphrase)
{

    WiFi.begin(ssid, passphrase);
    
    static const int max_attempt = 100;
    int attempt = 0;
    while (attempt < max_attempt && WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        attempt++;
    }
    Serial.println("");

    return (attempt < max_attempt);

}

bool Wifi::disconnect(){
    return WiFi.disconnect();
}

IPAddress Wifi::localIP()
{
    return WiFi.localIP();
}
