#define MQTT_VERSION MQTT_VERSION_3_1_1

#include <PubSubClient.h>
#include <WiFiClient.h>
#include "Mqtt.h"

WiFiClient wifiClient;

PubSubClient client(wifiClient);

static const int max_attempt = 10;

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

    // Loop until we're reconnected
    _attempt = 0;

    Serial.println("");
    while (_attempt < max_attempt && !client.connected())
    {
        Serial.println("INFO: Attempting MQTT connection...");
        // Attempt to connect
        if (!client.connect(client_id, user, pwd))
        {
            Serial.print("ERROR: failed, rc=");
            Serial.println(client.state());
            Serial.println("DEBUG: try again in 2 seconds");
            // Wait 2 seconds before retrying
            delay(2000);
        }
        _attempt++;
    }

    _error = _attempt >= max_attempt || !subscribe(log_topic);

    for (std::size_t i = 0; i < ntopics; i++)
        subscribe(topics[i]);

    return !_error;
}

bool Mqtt::subscribe(const char *topic)
{
    _error = _error || !client.connected() || !client.subscribe(topic);

    if (_error)
        log(String("Cannot subscribe to topic ") + topic);
    else
        log(String("Subscribed to ") + topic + " topic");

    return !_error;
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
        serial_print("Failed MQTT connection in logger, rc=");
        Serial.println(client.state());
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
    if (!client.loop())
    {
        reset();
        return client.loop();
    }
    else
        return true;
}

bool Mqtt::error() { return _error; }

int Mqtt::attempt() { return _attempt; }

void Mqtt::serial_print(const char *msg)
{
    Serial.print("Mqtt: ");
    Serial.print(msg);
}

void Mqtt::serial_println(const char *msg)
{
    serial_print(msg);
    Serial.println("");
}
