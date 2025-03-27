// MqttHandler.cpp

#ifdef ENABLE_MQTT_HANDLER

#include "MqttHandler.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LittleFS.h>

static NonBlockingTimer mqttTimer(60000);
WiFiClient wiFiClient;
WiFiClientSecure wiFiClientSecure;
PubSubClient mqttClient;

void MqttHandler::init()
{
    if(!settings.mqtt.enabled)return;
    registerCommands();
    connectToMqtt();
}

void MqttHandler::loop()
{
    if(!settings.mqtt.enabled) return;
    
    // If not connected, attempt to reconnect
    if (!mqttClient.connected())
    {
        if (mqttTimer.isReady())
        {
            debugW("MqttHandler: Not connected, attempting reconnect...");
            settings.mqtt.isConnected = false;
            connectToMqtt();
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

void MqttHandler::connectToMqtt()
{
    // Use the appropriate client based on SSL settings
    if (settings.mqtt.ssl)
    {
        debugI("MqttHandler: Using secure WiFi client.");

        // Mount LittleFS (if not already mounted)
        if (!LittleFS.begin())
        {
            debugE("MqttHandler: Failed to initialize LittleFS.");
            return;
        }

        // Open the certificate file
        File certFile = LittleFS.open(EMQX_CERT_FILE, "r");
        if (!certFile)
        {
            debugE("MqttHandler: Failed to open certificate file: %s", EMQX_CERT_FILE);
            return;
        }

        // Read the certificate content
        String certContent = certFile.readString();
        debugV("MqttHandler: Read %d bytes from certificate file.", certContent.length());
        debugV("MqttHandler: Certificate content: %s", certContent.c_str());

        // Close the certificate file
        certFile.close();
        wiFiClientSecure.setCACert(certContent.c_str());
        mqttClient.setClient(wiFiClientSecure);
    }
    else
    {
        debugI("MqttHandler: Using non-secure WiFi client.");
        mqttClient.setClient(wiFiClient);
    }

    // Point the MQTT client to the broker from settings
    mqttClient.setServer(settings.mqtt.server.c_str(), settings.mqtt.port);

    // Set the incoming message callback
    mqttClient.setCallback(MqttHandler::mqttCallback);

    debugI("MqttHandler: Attempting MQTT connection to %s:%d", settings.mqtt.server.c_str(), settings.mqtt.port);

    // Attempt to connect to the MQTT broker if username/password are not empty, use them
    if (!settings.mqtt.username.isEmpty() || !settings.mqtt.password.isEmpty())
    {
        debugI("MqttHandler: Trying MQTT connect with user/pass: [%s]/[****]", settings.mqtt.username.c_str());
        mqttClient.connect(settings.device.name.c_str(), settings.mqtt.username.c_str(), settings.mqtt.password.c_str());
    }
    else
    {
        debugI("MqttHandler: Trying MQTT connect WITHOUT credentials");
        mqttClient.connect(settings.device.name.c_str());
    }
    
    if (mqttClient.connected())
    {
        // Subscribe to the topic from settings
        debugI("MqttHandler: Connected as client [%s]", settings.device.name.c_str());
        mqttClient.subscribe(settings.mqtt.subTopic.c_str());
        debugI("MqttHandler: Subscribed to [%s]", settings.mqtt.subTopic.c_str());
        String helloMessage = "Device: " + settings.device.name + " connected.";
        publish(settings.mqtt.pubTopic.c_str(), helloMessage.c_str());
        settings.mqtt.isConnected = true;
    }
    else
    {
        debugE("MqttHandler: Connection failed, state=%d. Will retry...", mqttClient.state());
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
    if (String(topic) == settings.mqtt.subTopic)
    {
        debugD("MqttHandler: Echoing message to publish topic");
        publish(settings.mqtt.pubTopic.c_str(), message.c_str());
        // Debug.wsOnReceive(message.c_str());
        CommandHandler::handleCommand(message);
    }
}

void MqttHandler::registerCommands()
{
    CommandHandler::registerCommand("MQTT", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "MSG")) {
            MqttHandler::publish(settings.mqtt.pubTopic.c_str(), args.c_str());
        }
        else if (CommandHandler::equalsIgnoreCase(cmd, "TOPIC")) {
            int delimiterPos = args.indexOf(' ');
            String topic = args.substring(0, delimiterPos);
            String message = args.substring(delimiterPos + 1);
            MqttHandler::publish(topic.c_str(), message.c_str());
        } 
        else {
            debugW("Unknown MQTT subcommand: %s", cmd.c_str());
        } }, "Handles MQTT commands. Usage: MQTT <subcommand> [args]\n"
                                         "  Subcommands:\n"
                                         "  msg <message> - Publishes a message to the MQTT broker default topic\n"
                                         "  topic <topic> <message> - Publishes a message to the specified topic");
}

#endif // ENABLE_MQTT_HANDLER