#ifndef BLUETOOTH_HANDLER_H
#define BLUETOOTH_HANDLER_H

#ifdef ENABLE_BLUETOOTH_HANDLER

#include "Globals.h"
#include <NimBLEDevice.h>

class BluetoothHandler
{
private:
    static NimBLEServer* pServer;
    static NimBLECharacteristic* pCharacteristic;

    // Custom callback class for handling writes
    class WriteCallback : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic) override;
    };

public:
    static void init();
    static void loop();
};

#else

class BluetoothHandler {
public: // No-op implementation of BluetoothHandler
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_BLUETOOTH_HANDLER
#endif // BLUETOOTH_HANDLER_H
