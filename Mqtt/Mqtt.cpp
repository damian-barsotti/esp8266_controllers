#define MQTT_VERSION MQTT_VERSION_3_1_1

#include <PubSubClient.h>
#include <WiFiClient.h>
#include "Mqtt.h"

static const char *state_msgs[] = {
    "Server didn't respond within the keepalive time",
    "Network connection was broken",
    "Network connection failed",
    "Client is disconnected cleanly",
    "Client is connected",
    "Server doesn't support the requested version of MQTT",
    "Server rejected the client identifier",
    "Server was unable to accept the connection",
    "Username/password were rejected",
    "Client was not authorized to connect"};

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void serial_print(const char *msg)
{
    Serial.print("Mqtt: ");
    Serial.print(msg);
}

void serial_println(const char *msg)
{
    serial_print(msg);
    Serial.println("");
}

void serial_println_status()
{
    int st = client.state();
    serial_print(state_msgs[st + 4]);
    Serial.print(" (rc=");
    Serial.print(st);
    Serial.println(")");
}

Mqtt::Mqtt(Stream &Serial,
           const char *ip, int port,
           const char *client_id, const char *user,
           const char *pwd, const char *log_topic,
           const char **topics, std::size_t ntopics,
           mqtt_callback_type callback)
    : Serial(Serial),
      ip(ip), port(port),
      client_id(client_id), user(user),
      pwd(pwd), log_topic(log_topic),
      topics(topics), ntopics(ntopics),
      callback(callback), _attempt(0)
{
}

bool Mqtt::begin()
{
    client.setServer(ip, port);
    client.setCallback(callback);
    return connect();
}

bool Mqtt::connect()
{
    _error = false;

    ++_attempt;

    if (client.connect(client_id, user, pwd))
    {
        serial_print("Connected #");
        Serial.println(String(_attempt));
        if (subscribes())
        {
            _attempt = 0;
            return true;
        }
        else
        {
            log(String("Cannot subscribe #") + _attempt + " .Disconnecting.");
            client.disconnect();
        }
    }
    else
        log(String("Cannot connect #") + _attempt);

    _error = true;

    return false;
}

bool Mqtt::subscribes()
{
    if (!subscribe(log_topic))
        return false;

    std::size_t i = 0;
    while (i < ntopics && subscribe(topics[i]))
        i++;

    return i >= ntopics;
}

bool Mqtt::subscribe(const char *topic)
{
    !client.subscribe(topic);

    if (client.subscribe(topic))
    {
        log(String("Subscribed to ") + topic + " topic");
        return true;
    }
    else
    {
        log(String("Cannot subscribe to topic ") + topic);
        return false;
    }
}

bool Mqtt::publish(DynamicJsonDocument root, const char *topic)
{

    String data;

    serializeJson(root, data);

    serial_print("Publish in topic ");
    Serial.println(topic);

    Serial.println(data.c_str());

    return client.publish(topic, data.c_str(), true);
}

// function to log throught Serial and mqtt topic
bool Mqtt::log(String msg)
{

    if (client.connected())
    {
        DynamicJsonDocument root(msg.length() + 17);
        root["log"] = msg;
        publish(root, log_topic);
        return true;
    }
    else
    {
        serial_println(msg.c_str());
        serial_println("Failed MQTT connection in logger.");
        serial_println_status();
        return false;
    }
}

bool Mqtt::reset()
{
    log("Reset");
    for (std::size_t i = 0; i < ntopics; i++)
        client.unsubscribe(topics[i]);
    client.disconnect();
    _attempt = 0;
    return connect();
}

bool Mqtt::beginLoop()
{
    if (!client.connected())
    {
        serial_println("Not connected in loop. Reconnect...");
        serial_println_status();
        _error = !connect();
    }
    else
    {
        _error = false;
        client.loop();
    }

    return !_error;
}

bool Mqtt::error() { return _error; }

int Mqtt::attempt() { return _attempt; }
