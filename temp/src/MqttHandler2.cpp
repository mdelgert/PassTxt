#ifdef ENABLE_MQTT_HANDLER

#include "MqttHandler.h"

// Create the Wi-Fi client & MQTT client
//WiFiClient wifiClient;
//PubSubClient mqttClient(wifiClient);

WiFiClientSecure wiFiClientSecure;
PubSubClient mqttClient(wiFiClientSecure);

// Optional: Use a NonBlockingTimer to avoid continuously retrying in busy loops
static NonBlockingTimer mqttTimer(2000);

void MqttHandler::init()
{
    // Point the MQTT client to the broker from settings
    mqttClient.setServer(settings.mqttServer.c_str(), settings.mqttPort);

    // Set the incoming message callback
    mqttClient.setCallback(MqttHandler::mqttCallback);

    // Attempt initial connection
    connectToMqtt();
}

void MqttHandler::loop()
{
    // If not connected, attempt to reconnect every 2 seconds
    if (!mqttClient.connected())
    {
        if (mqttTimer.isReady())
        {
            //debugI("MqttHandler: Not connected, attempting reconnect...");
            //Suspect this is blocking remote debugger terminal windows stops working 
            //when it fails disabling as a test................
            //connectToMqtt();
        }
    }
    else
    {
        // Process the incoming MQTT traffic
        mqttClient.loop();
    }
}

void MqttHandler::publish(const char *topic, const char *message)
{
    // If not connected, attempt to reconnect
    if (!mqttClient.connected())
    {
        debugW("MqttHandler: Not connected, attempting reconnect before publish...");
        connectToMqtt();
    }

    debugI("MqttHandler: Publishing on [%s]: %s", topic, message);
    mqttClient.publish(topic, message);
}

// ------------------- Private functions -------------------

void MqttHandler::connectToMqtt()
{
    debugI("MqttHandler: Attempting MQTT connection to %s:%d", settings.mqttServer.c_str(), settings.mqttPort);
    
    wiFiClientSecure.setCACert(ca_cert);

    bool isConnected = false;

    // If username/password are not empty, use them
    if (!settings.mqttUsername.isEmpty() || !settings.mqttPassword.isEmpty())
    {
        debugI("MqttHandler: Trying MQTT connect with user/pass: [%s]/[****]", settings.mqttUsername.c_str());
        isConnected = mqttClient.connect(settings.deviceName.c_str(), settings.mqttUsername.c_str(), settings.mqttPassword.c_str());
    }
    else
    {
        debugI("MqttHandler: Trying MQTT connect WITHOUT credentials");
        isConnected = mqttClient.connect(settings.deviceName.c_str());
    }

    if (isConnected)
    {
        debugI("MqttHandler: Connected as client [%s]", settings.deviceName.c_str());

        // Subscribe to the topic from settings
        mqttClient.subscribe(settings.mqttSubTopic.c_str());
        debugI("MqttHandler: Subscribed to [%s]", settings.mqttSubTopic.c_str());

        // Optionally publish a "hello" message
        publish(settings.mqttPubTopic.c_str(), "Hello from ESP32-S3!");
    }
    else
    {
        debugE("MqttHandler: Connection failed, state=%d. Will retry...",
               mqttClient.state());
    }
}

void MqttHandler::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    debugI("MqttHandler: Received on [%s]: %s", topic, message.c_str());

    // If incoming message is on the subscribed topic, echo it out
    if (String(topic) == settings.mqttSubTopic)
    {
        debugD("MqttHandler: Echoing message to publish topic");
        publish(settings.mqttPubTopic.c_str(), message.c_str());
    }
}

#endif // ENABLE_MQTT_HANDLER

/*
debugI("MqttHandler: init() called");
debugI("settings.mqttServer: %s", settings.mqttServer.c_str());
debugI("settings.mqttPort: %d", settings.mqttPort);
debugI("settings.mqttUsername: %s", settings.mqttUsername.c_str());
debugI("settings.mqttPassword: %s", settings.mqttPassword.c_str());
*/