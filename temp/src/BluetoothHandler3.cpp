#ifdef ENABLE_BLUETOOTH_HANDLER

#include "BluetoothHandler.h"

NimBLEServer *BluetoothHandler::pServer = nullptr;
NimBLECharacteristic *BluetoothHandler::pTxCharacteristic = nullptr;
NimBLECharacteristic *BluetoothHandler::pRxCharacteristic = nullptr;

// RX characteristic callback implementation
void BluetoothHandler::RxCallback::onWrite(NimBLECharacteristic *pCharacteristic)
{
    std::string receivedValue = pCharacteristic->getValue();
    if (receivedValue.length() > 0)
    {
        debugI("Received: %s", receivedValue.c_str());

        CommandHandler::handleCommand(receivedValue.c_str());

        // Process a command
        // std::string response;

        // if (receivedValue == "hello")
        // {
        //     response = "hello world"; // Respond to 'hello'
        // }
        // else
        // {
        //     response = "Unknown command: " + receivedValue; // Default response
        // }

        // Send the response back using the TX characteristic
        // if (pTxCharacteristic) {
        //     pTxCharacteristic->setValue(response);
        //     pTxCharacteristic->notify();
        //     debugI("Sent response: %s", response.c_str());
        // }

        // Send the response back using the TX characteristic
        if (pTxCharacteristic)
        {
            pTxCharacteristic->setValue(receivedValue);
            pTxCharacteristic->notify();
            debugI("Sent response: %s", receivedValue.c_str());
        }
    }
    else
    {
        debugW("Received an empty message.");
    }
}

// Initialize the Nordic UART Service
void BluetoothHandler::init()
{
    debugI("Initializing BluetoothHandler...");

    // Initialize BLE
    // NimBLEDevice::init("ESP32-NUS");
    NimBLEDevice::init(settings.deviceName.c_str());
    pServer = NimBLEDevice::createServer();

    // Create the Nordic UART Service
    NimBLEService *pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

    // Create TX characteristic (Notify)
    pTxCharacteristic = pService->createCharacteristic(
        "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
        NIMBLE_PROPERTY::NOTIFY);

    // Create RX characteristic (Write)
    pRxCharacteristic = pService->createCharacteristic(
        "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
        NIMBLE_PROPERTY::WRITE);

    // Attach the RX callback
    pRxCharacteristic->setCallbacks(new RxCallback());

    // Start the service and advertising
    pService->start();
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    pAdvertising->start();

    debugI("BluetoothHandler initialized and advertising.");
}

// BLE loop
void BluetoothHandler::loop()
{
    // BLE is event-driven, so this loop can remain empty
}

#endif // ENABLE_BLUETOOTH_HANDLER
