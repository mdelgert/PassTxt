#ifdef ENABLE_BLUETOOTH_HANDLER

#include "BluetoothHandler.h"

NimBLEServer* BluetoothHandler::pServer = nullptr;
NimBLECharacteristic* BluetoothHandler::pCharacteristic = nullptr;
static NonBlockingTimer bleTimer(5000); // Example timer for periodic BLE tasks

// Custom onWrite callback implementation
void BluetoothHandler::WriteCallback::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string receivedValue = pCharacteristic->getValue();
    if (receivedValue.length() > 0) {
        debugI("Received message: %s", receivedValue.c_str());
    } else {
        debugW("Received an empty write.");
    }
}

// Initialize BLE
void BluetoothHandler::init() {
    debugI("Initializing BluetoothHandler...");

    // Initialize BLE
    NimBLEDevice::init("ESP32-BLE");
    pServer = NimBLEDevice::createServer();

    // Create service and characteristic
    NimBLEService* pService = pServer->createService("0000181C-0000-1000-8000-00805F9B34FB");
    pCharacteristic = pService->createCharacteristic(
        "00002A57-0000-1000-8000-00805F9B34FB",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );

    // Set the write callback
    pCharacteristic->setCallbacks(new WriteCallback());

    // Start the service
    pService->start();

    // Start advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("0000181C-0000-1000-8000-00805F9B34FB");
    pAdvertising->start();

    debugI("BluetoothHandler initialized and advertising started.");
}

// BLE loop to handle periodic tasks
void BluetoothHandler::loop() {
    if (bleTimer.isReady()) {
        //debugD("BluetoothHandler loop running...");
        // Additional periodic tasks can be placed here
    }
}

#endif // ENABLE_BLUETOOTH_HANDLER
