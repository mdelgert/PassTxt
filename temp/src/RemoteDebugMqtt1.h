#pragma once

#include <RemoteDebug.h>
#include "MqttHandler.h" // or your MQTT code

class RemoteDebugMQTT : public RemoteDebug {
public:
    // Override the single-character write
    virtual size_t write(uint8_t c) override {
        // Convert single char to string and publish
        String msg((char)c);
        //sendToMqtt(msg);
        MqttHandler::publish("testtopic/logs", msg.c_str());
        // Call the base class to preserve normal functionality
        return RemoteDebug::write(c);
    }

    // Override the buffer write
    virtual size_t write(const uint8_t *buffer, size_t size) override {
        // Convert buffer to string and publish
        String msg((const char*)buffer, size);
        //sendToMqtt(msg);
        MqttHandler::publish("testtopic/logs", msg.c_str());
        // Call the base class
        return RemoteDebug::write(buffer, size);
    }

private:
    void sendToMqtt(const String &msg) {
        // You can buffer lines until '\n' if you want line-based sending,
        // or just send each chunk as-is:
        //MqttHandler::publish("remotedebug/logs", msg);
        MqttHandler::publish("testtopic/logs", msg.c_str());
    }
};
