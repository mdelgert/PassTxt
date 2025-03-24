#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#ifdef ENABLE_MQTT_HANDLER

#include "Globals.h"
#include <WiFi.h>
#include <PubSubClient.h>

class MqttHandler
{
private:
    static void connectToMqtt();
    static void mqttCallback(char* topic, byte* payload, unsigned int length);

public:
    static void init();
    static void loop();
    static void publish(const char* topic, const char* message);
};

#else

// No-op implementation if MQTT_HANDLER is not enabled
class MqttHandler
{
public:
    static void init() {}
    static void loop() {}
    static void publish(const char* topic, const char* message) {}
};

#endif // ENABLE_MQTT_HANDLER
#endif // MQTT_HANDLER_H
