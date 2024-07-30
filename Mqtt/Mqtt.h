#ifndef MQTT_H
#define MQTT_H

#include <ArduinoJson.h>
#include "../Controller/Controller.h"

using mqtt_callback_type = std::function<void(char *, uint8_t *, unsigned int)>;

class Mqtt : public Controller
{
public:
    Mqtt(Stream &Serial,
         const char *ip, int port,
         const char *client_id, const char *user,
         const char *pwd, const char *log_topic,
         const char **topics, std::size_t ntopics,
         mqtt_callback_type callback);
    bool begin();
    bool reset();
    bool beginLoop();
    bool error();
    bool publish(DynamicJsonDocument root, const char *topic);
    bool log(String msg);
    int attempt();

private:
    bool _error = true;
    Stream &Serial;
    const char *ip;
    int port;
    const char *client_id, *user, *pwd, *log_topic;
    const char **topics;
    std::size_t ntopics;
    mqtt_callback_type callback;
    int _attempt = 0;
    bool subscribes();
    bool subscribe(const char *topic);
    bool connect();
};

// Callback function to be defined
void callback(char *topic, byte *payload, unsigned int length);

#endif