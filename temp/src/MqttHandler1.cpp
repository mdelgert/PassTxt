#ifdef ENABLE_MQTT_HANDLER

#include "MqttHandler.h"

// Example broker and topics
static const char* MQTT_BROKER     = "192.168.50.174";
static const uint16_t MQTT_PORT    = 1883;
static const char* MQTT_CLIENT_ID  = "myEsp32S3Client";
static const char* MQTT_SUB_TOPIC  = "testtopic/sub";
static const char* MQTT_PUB_TOPIC  = "testtopic/pub";
static const char* MQTT_USER       = "emqx";
static const char* MQTT_PASSWORD   = "public";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Optional: Use a NonBlockingTimer to avoid continuously retrying in busy loops
static NonBlockingTimer mqttTimer(2000);

void MqttHandler::init()
{
    debugI("MqttHandler: init() called");
    
    // Setup the MQTT server/broker
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(MqttHandler::mqttCallback);

    // Try initial connection
    connectToMqtt();
}

void MqttHandler::loop()
{
    // If not connected, attempt to reconnect every 2 seconds (as set above)
    if (!mqttClient.connected())
    {
        if (mqttTimer.isReady())
        {
            connectToMqtt();
        }
    }
    else
    {
        // Process MQTT messages
        mqttClient.loop();
    }
}

void MqttHandler::publish(const char* topic, const char* message)
{
    // If we are not connected, try reconnecting before publish
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
    debugI("MqttHandler: Attempting MQTT connection to %s:%d", MQTT_BROKER, MQTT_PORT);

    // If both username and password are non-empty, connect with credentials
    bool isConnected = false;
    if (strlen(MQTT_USER) > 0 || strlen(MQTT_PASSWORD) > 0)
    {
        debugI("MqttHandler: Trying MQTT connect with user/pass");
        isConnected = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD);
    }
    else
    {
        debugI("MqttHandler: Trying MQTT connect WITHOUT credentials");
        isConnected = mqttClient.connect(MQTT_CLIENT_ID);
    }

    if (isConnected)
    {
        debugI("MqttHandler: Connected as client [%s]", MQTT_CLIENT_ID);

        // Subscribe on successful connection
        mqttClient.subscribe(MQTT_SUB_TOPIC);
        debugI("MqttHandler: Subscribed to [%s]", MQTT_SUB_TOPIC);

        // Optionally publish a welcome message
        publish(MQTT_PUB_TOPIC, "Hello from ESP32-S3!");
    }
    else
    {
        debugE("MqttHandler: Connection failed, state=%d. Will retry...", mqttClient.state());
    }
}

void MqttHandler::mqttCallback(char* topic, byte* payload, unsigned int length)
{
    // Convert payload to String for easy handling
    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    debugI("MqttHandler: Received on [%s]: %s", topic, message.c_str());

    // Example logic: If the message came on the subscribed topic, echo it back out
    if (String(topic) == MQTT_SUB_TOPIC)
    {
        debugD("MqttHandler: Echoing message to publish topic");
        publish(MQTT_PUB_TOPIC, message.c_str());
    }
}

#endif // ENABLE_MQTT_HANDLER
